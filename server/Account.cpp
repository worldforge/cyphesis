// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Account.h"

#include "Connection_methods.h"
#include "WorldRouter.h"
#include "ServerRouting.h"
#include "Lobby.h"
#include "ExternalMind.h"
#include "Persistance.h"

#include <rulesets/Character.h>

#include <common/log.h>
#include <common/debug.h>
#include <common/Database.h>

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
                               const Fragment::MapType & ent)
{
    WorldRouter & world = connection->server.world;
    debug(std::cout << "Account::Add_character" << std::endl << std::flush;);
    Entity * chr = world.addObject(typestr, ent);
    debug(std::cout << "Added" << std::endl << std::flush;);
    if (!chr->location.isValid()) {
        debug(std::cout << "Setting location" << std::endl << std::flush;);
        chr->location.ref = &world.gameWorld;
        chr->location.coords = Vector3D(0, 0, 0);
    }
    debug(std::cout << "Location set to: " << chr->location << std::endl << std::flush;);
    Character * pchar = dynamic_cast<Character *>(chr);
    if (pchar != 0) {
        pchar->externalMind = new ExternalMind(*connection, pchar->getId(), pchar->getName());
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
    Fragment::MapType entmap;
    entmap["parents"] = Fragment::ListType(1,"coin");
    entmap["pos"] = Vector3D(0,0,0).asObject();
    entmap["loc"] = chr->getId();
    entmap["name"] = "coin";
    for(int i = 0; i < 10; i++) {
        Create * c = new Create(Create::Instantiate());
        c->SetArgs(Fragment::ListType(1,entmap));
        c->SetTo(chr->getId());
        world.message(*c, chr);
    }

    Create c = Create::Instantiate();
    c.SetArgs(Fragment::ListType(1,chr->asObject()));

    Sight * s = new Sight(Sight::Instantiate());
    s->SetArgs(Fragment::ListType(1,c.AsObject()));

    world.message(*s, chr);

    return chr;
}

OpVector Account::LogoutOperation(const Logout & op)
{
    debug(std::cout << "Account logout: " << getId() << std::endl;);
    Info info = Info(Info::Instantiate());
    info.SetArgs(Fragment::ListType(1,op.AsObject()));
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

void Account::addToObject(Fragment::MapType & omap) const
{
    omap["id"] = getId();
    omap["username"] = username;
    omap["name"] = username;
    if (!password.empty()) {
        omap["password"] = password;
    }
    omap["parents"] = Fragment::ListType(1,getType());
    Fragment::ListType charlist;
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
    const Fragment & ent = op.GetArgs().front();
    if (!ent.IsMap()) {
        return error(op, "Invalid character");
    }
    const Fragment::MapType & entmap = ent.AsMap();
    Fragment::MapType::const_iterator I = entmap.find("parents");
    if ((I == entmap.end()) || !(I->second.IsList()) ||
        (I->second.AsList().empty()) ||
        !(I->second.AsList().front().IsString()) ) {
        return error(op, "Character has no type");
    }
    
    OpVector error = characterError(op, entmap);
    if (!error.empty()) {
        return error;
    }
    const std::string & typestr = I->second.AsList().front().AsString();
    debug( std::cout << "Account creating a " << typestr << " object"
                     << std::endl << std::flush; );

    BaseEntity * obj = addCharacter(typestr, entmap);
    //log.inform("Player "+Account::id+" adds character "+`obj`,op);
    Info * info = new Info(Info::Instantiate());
    info->SetArgs(Fragment::ListType(1,obj->asObject()));
    info->SetRefno(op.GetSerialno());
    info->SetSerialno(connection->server.getSerialNo());

    return OpVector(1,info);
}

OpVector Account::ImaginaryOperation(const Imaginary & op)
{
    const Fragment::ListType & args = op.GetArgs();
    if ((!args.empty()) && (args.front().IsMap())) {
        Sight s(Sight::Instantiate());
        s.SetArgs(Fragment::ListType(1,op.AsObject()));
        s.SetFrom(getId());
        s.SetSerialno(connection->server.getSerialNo());
        setRefnoOp(&s, op);
        const Fragment::MapType & arg = args.front().AsMap();
        Fragment::MapType::const_iterator I = arg.find("loc");
        if (I != arg.end()) {
            s.SetTo(I->second.AsString());
        } else {
            s.SetTo(op.GetTo());
        }
        return connection->server.lobby.operation(s);
    }
    return OpVector();
}

OpVector Account::TalkOperation(const Talk & op)
{
    const Fragment::ListType & args = op.GetArgs();
    if ((!args.empty()) && (args.front().IsMap())) {
        Sound s(Sound::Instantiate());
        s.SetArgs(Fragment::ListType(1,op.AsObject()));
        s.SetFrom(getId());
        s.SetSerialno(connection->server.getSerialNo());
        setRefnoOp(&s, op);
        const Fragment::MapType & arg = args.front().AsMap();
        Fragment::MapType::const_iterator I = arg.find("loc");
        if (I != arg.end()) {
            s.SetTo(I->second.AsString());
        } else {
            s.SetTo(op.GetTo());
        }
        return connection->server.lobby.operation(s);
    }
    return OpVector();
}

OpVector Account::LookOperation(const Look & op)
{
    const Fragment::ListType & args = op.GetArgs();
    if (args.empty()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        s->SetArgs(Fragment::ListType(1,connection->server.lobby.asObject()));
        s->SetSerialno(connection->server.getSerialNo());
        setRefnoOp(s, op);
        return OpVector(1,s);
    }
    Fragment::MapType::const_iterator I = args.front().AsMap().find("id");
    if ((I == args.front().AsMap().end()) || (!I->second.IsString())) {
        return error(op, "No target for look");
    }
    const std::string & to = I->second.AsString();
    EntityDict::const_iterator J = charactersDict.find(to);
    if (J != charactersDict.end()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        s->SetArgs(Fragment::ListType(1,J->second->asObject()));
        s->SetSerialno(connection->server.getSerialNo());
        setRefnoOp(s, op);
        return OpVector(1,s);
    }
    const AccountDict & accounts = connection->server.lobby.getAccounts();
    AccountDict::const_iterator K = accounts.find(to);
    if (K != accounts.end()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        s->SetArgs(Fragment::ListType(1,K->second->asObject()));
        s->SetSerialno(connection->server.getSerialNo());
        setRefnoOp(s, op);
        return OpVector(1,s);
    }
    return error(op, "Unknown look target");
}
