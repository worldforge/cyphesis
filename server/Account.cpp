// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Account.h"

#include "Connection_methods.h"
#include "ServerRouting.h"
#include "Lobby.h"
#include "ExternalMind.h"
#include "Persistance.h"

#include "rulesets/Character.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/Database.h"
#include "common/BaseWorld.h"

#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Info.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

static const bool debug_flag = false;

Account::Account(Connection * conn, const std::string & uname,
                 const std::string& passwd, const std::string & id)
                 : OOGThing(id.empty() ? Database::instance()->getEntityId() : id),
                   connection(conn), username(uname), password(passwd)
{
    subscribe("logout", OP_LOGOUT);
    subscribe("create", OP_CREATE);
    subscribe("imaginary", OP_IMAGINARY);
    subscribe("talk", OP_TALK);
    subscribe("look", OP_LOOK);
}

Account::~Account()
{
    ConMap::const_iterator J = destroyedConnections.begin();
    for(; J != destroyedConnections.end(); J++) {
        J->second->disconnect();
        delete J->second;
    }
}

void Account::characterDestroyed(std::string id)
{
    charactersDict.erase(id);
    ConMap::iterator I = destroyedConnections.find(id);
    if (I != destroyedConnections.end()) {
        delete I->second;
        destroyedConnections.erase(I);
    }
    Persistance::instance()->delCharacter(id);
}

void Account::addCharacter(Entity * chr)
{
    Character * pchar = dynamic_cast<Character *>(chr);
    if (pchar == 0) {
        return;
    }
    charactersDict[chr->getId()] = chr;
    SigC::Connection * con = new SigC::Connection(chr->destroyed.connect(SigC::bind<std::string>(slot(*this, &Account::characterDestroyed), chr->getId())));
    destroyedConnections[chr->getId()] = con;
}

Entity * Account::addCharacter(const std::string & typestr,
                               const Element::MapType & ent)
{
    BaseWorld & world = connection->server.world;
    debug(std::cout << "Account::Add_character" << std::endl << std::flush;);
    Entity * chr = world.addObject(typestr, ent);
    debug(std::cout << "Added" << std::endl << std::flush;);
    if (!chr->m_location.isValid()) {
        debug(std::cout << "Setting location" << std::endl << std::flush;);
        chr->m_location.m_loc = &world.gameWorld;
        chr->m_location.m_pos = Vector3D(0, 0, 0);
    }
    debug(std::cout << "Location set to: " << chr->m_location << std::endl << std::flush;);
    Character * pchar = dynamic_cast<Character *>(chr);
    if (pchar != 0) {
        pchar->m_externalMind = new ExternalMind(*connection, pchar->getId(), pchar->getName());
        // Only genuinely playable characters should go in here. Otherwise
        // if a normal entity gets into the account, and connection, it
        // starts getting hard to tell whether or not they exist.
        charactersDict[chr->getId()] = chr;
        SigC::Connection * con = new SigC::Connection(chr->destroyed.connect(SigC::bind<std::string>(slot(*this, &Account::characterDestroyed), chr->getId())));
        destroyedConnections[chr->getId()] = con;
        connection->addObject(chr);
        Persistance::instance()->addCharacter(*this, *chr);
    }

    // Hack in default objects
    // This needs to be done in a generic way
    Element::MapType entmap;
    entmap["parents"] = Element::ListType(1,"coin");
    entmap["pos"] = Vector3D(0,0,0).asObject();
    entmap["loc"] = chr->getId();
    entmap["name"] = "coin";
    for(int i = 0; i < 10; i++) {
        Create * c = new Create(Create::Instantiate());
        Element::ListType & args = c->GetArgs();
        args.push_back(entmap);
        c->SetTo(chr->getId());
        world.message(*c, chr);
    }

    Create c = Create::Instantiate();
    Element::ListType & cargs = c.GetArgs();
    cargs.push_back(Element::MapType());
    chr->addToObject(cargs.front().AsMap());

    Sight * s = new Sight(Sight::Instantiate());
    Element::ListType & args = s->GetArgs();
    args.push_back(c.AsObject());

    world.message(*s, chr);

    return chr;
}

OpVector Account::LogoutOperation(const Logout & op)
{
    debug(std::cout << "Account logout: " << getId() << std::endl;);
    Info info = Info(Info::Instantiate());
    Element::ListType & args = info.GetArgs();
    args.push_back(op.AsObject());
    info.SetRefno(op.GetSerialno());
    info.SetSerialno(connection->server.getSerialNo());
    info.SetFrom(getId());
    info.SetTo(getId());
    connection->send(info);
    connection->close();
    
    return OpVector();
}

const char * Account::getType() const
{
    return "account";
}

void Account::addToObject(Element::MapType & omap) const
{
    omap["objtype"] = "object";
    omap["id"] = getId();
    omap["username"] = username;
    omap["name"] = username;
    if (!password.empty()) {
        omap["password"] = password;
    }
    omap["parents"] = Element::ListType(1,getType());
    Element::ListType charlist;
    EntityDict::const_iterator I;
    for(I = charactersDict.begin(); I != charactersDict.end(); I++) {
        charlist.push_back(I->first);
    }
    omap["characters"] = charlist;
    // No need to call BaseEntity::addToObject, as none of the default
    // attributes (location, contains etc.) are relevant to accounts
}

OpVector Account::CreateOperation(const Create & op)
{
    debug(std::cout << "Account::Operation(create)" << std::endl << std::flush;);
    const Element::ListType & args = op.GetArgs();
    if ((args.empty()) || (!args.front().IsMap())) {
        return OpVector();
    }

    const Element::MapType & entmap = args.front().AsMap();

    OpVector er = characterError(op, entmap);
    if (!er.empty()) {
        return er;
    }

    Element::MapType::const_iterator I = entmap.find("parents");
    if ((I == entmap.end()) || !(I->second.IsList()) ||
        (I->second.AsList().empty()) ||
        !(I->second.AsList().front().IsString()) ) {
        return error(op, "Character has no type");
    }
    
    const std::string & typestr = I->second.AsList().front().AsString();
    debug( std::cout << "Account creating a " << typestr << " object"
                     << std::endl << std::flush; );

    BaseEntity * obj = addCharacter(typestr, entmap);

    Info * info = new Info(Info::Instantiate());
    Element::ListType & info_args = info->GetArgs();
    info_args.push_back(Element::MapType());
    obj->addToObject(info_args.front().AsMap());
    info->SetRefno(op.GetSerialno());
    info->SetSerialno(connection->server.getSerialNo());

    return OpVector(1,info);
}

OpVector Account::ImaginaryOperation(const Imaginary & op)
{
    const Element::ListType & args = op.GetArgs();
    if ((args.empty()) || (!args.front().IsMap())) {
        return OpVector();
    }

    Sight s(Sight::Instantiate());
    Element::ListType & sargs = s.GetArgs();
    sargs.push_back(op.AsObject());
    s.SetFrom(getId());
    s.SetSerialno(connection->server.getSerialNo());
    setRefnoOp(&s, op);
    const Element::MapType & arg = args.front().AsMap();
    Element::MapType::const_iterator I = arg.find("loc");
    if (I != arg.end()) {
        s.SetTo(I->second.AsString());
    } else {
        s.SetTo(op.GetTo());
    }
    return connection->server.lobby.operation(s);
}

OpVector Account::TalkOperation(const Talk & op)
{
    const Element::ListType & args = op.GetArgs();
    if ((args.empty()) || (!args.front().IsMap())) {
        return OpVector();
    }

    Sound s(Sound::Instantiate());
    Element::ListType & sargs = s.GetArgs();
    sargs.push_back(op.AsObject());
    s.SetFrom(getId());
    s.SetSerialno(connection->server.getSerialNo());
    setRefnoOp(&s, op);
    const Element::MapType & arg = args.front().AsMap();
    Element::MapType::const_iterator I = arg.find("loc");
    if (I != arg.end()) {
        s.SetTo(I->second.AsString());
    } else {
        s.SetTo(op.GetTo());
    }
    return connection->server.lobby.operation(s);
}

OpVector Account::LookOperation(const Look & op)
{
    const Element::ListType & args = op.GetArgs();
    if (args.empty()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        Element::ListType & s_args = s->GetArgs();
        s_args.push_back(Element::MapType());
        connection->server.lobby.addToObject(s_args.front().AsMap());
        s->SetSerialno(connection->server.getSerialNo());
        setRefnoOp(s, op);
        return OpVector(1,s);
    }
    Element::MapType::const_iterator I = args.front().AsMap().find("id");
    if ((I == args.front().AsMap().end()) || (!I->second.IsString())) {
        return error(op, "No target for look");
    }
    const std::string & to = I->second.AsString();
    EntityDict::const_iterator J = charactersDict.find(to);
    if (J != charactersDict.end()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        Element::ListType & s_args = s->GetArgs();
        s_args.push_back(Element::MapType());
        J->second->addToObject(s_args.front().AsMap());
        s->SetSerialno(connection->server.getSerialNo());
        setRefnoOp(s, op);
        return OpVector(1,s);
    }
    const AccountDict & accounts = connection->server.lobby.getAccounts();
    AccountDict::const_iterator K = accounts.find(to);
    if (K != accounts.end()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        Element::ListType & s_args = s->GetArgs();
        s_args.push_back(Element::MapType());
        K->second->addToObject(s_args.front().AsMap());
        s->SetSerialno(connection->server.getSerialNo());
        setRefnoOp(s, op);
        return OpVector(1,s);
    }
    return error(op, "Unknown look target");
}
