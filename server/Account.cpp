// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Info.h>

#include <common/debug.h>

#include <rulesets/Character.h>

#include "Account.h"
#include "Connection_methods.h"
#include "WorldRouter.h"
#include "ServerRouting.h"
#include "Lobby.h"
#include "ExternalMind.h"

static const bool debug_flag = false;

using Atlas::Message::Object;

Account::Account(Connection * conn, const std::string & username,
                 const std::string& passwd)
                 : world(NULL), connection(conn),
                   password(passwd), type("account")
{
    setId(username);

    subscribe("logout", OP_LOGOUT);
    subscribe("create", OP_CREATE);
    subscribe("imaginary", OP_IMAGINARY);
    subscribe("talk", OP_TALK);
    subscribe("look", OP_LOOK);
}

Account::~Account()
{
}

BaseEntity * Account::addCharacter(const std::string & typestr,
                                   const Object::MapType & ent)
{
    debug(std::cout << "Account::Add_character" << std::endl << std::flush;);
    Entity * chr = world->addObject(typestr, ent);
    debug(std::cout << "Added" << std::endl << std::flush;);
    if (!chr->location.isValid()) {
        debug(std::cout << "Setting location" << std::endl << std::flush;);
        chr->location.ref = &world->gameWorld;
        chr->location.coords = Vector3D(0, 0, 0);
    }
    debug(std::cout << "Location set to: " << chr->location << std::endl << std::flush;);
    Character * pchar = dynamic_cast<Character *>(chr);
    if (pchar != NULL) {
        pchar->externalMind = new ExternalMind(*connection, pchar->getId(), pchar->getName());
        // Only genuinely playable characters should go in here. Otherwise
        // if a normal entity gets into the account, and connection, it
        // starts getting hard to tell whether or not they exist.
        charactersDict[chr->getId()] = chr;
        connection->addObject(chr);
    }

    // Hack in default objects
    // This needs to be done in a generic way
    Object::MapType entmap;
    entmap["parents"] = Object::ListType(1,"coin");
    entmap["pos"] = Vector3D(0,0,0).asObject();
    entmap["loc"] = chr->getId();
    entmap["name"] = "coin";
    for(int i=0; i < 10; i++) {
        Create * c = new Create(Create::Instantiate());
        c->SetArgs(Object::ListType(1,entmap));
        c->SetTo(chr->getId());
        world->message(*c, chr);
    }

    Create c = Create::Instantiate();
    c.SetArgs(Object::ListType(1,chr->asObject()));

    Sight * s = new Sight(Sight::Instantiate());
    s->SetArgs(Object::ListType(1,c.AsObject()));

    world->message(*s, chr);

    return chr;
}

OpVector Account::LogoutOperation(const Logout & op)
{
    debug(std::cout << "Account logout: " << getId() << std::endl;);
    Info info = Info(Info::Instantiate());
    info.SetArgs(Object::ListType(1,asObject()));
    info.SetRefno(op.GetSerialno());
    info.SetSerialno(connection->server.getSerialNo());
    info.SetFrom(getId());
    info.SetTo(getId());
    connection->send(&info);
    connection->destroy();
    return OpVector();
}

void Account::addToObject(Object::MapType & omap) const
{
    omap["id"] = Object(getId());
    omap["username"] = Object(getId());
    omap["name"] = Object(getId());
    if (!password.empty()) {
        omap["password"] = Object(password);
    }
    omap["parents"] = Object(Object::ListType(1,Object(type)));
    Object::ListType charlist;
    EntityDict::const_iterator I;
    for(I = charactersDict.begin(); I != charactersDict.end(); I++) {
        charlist.push_back(Object(I->first));
    }
    omap["characters"] = Object(charlist);
    // No need to call BaseEntity::addToObject, as none of the default
    // attributes (location, contains etc.) are relevant to accounts
}

OpVector Account::CreateOperation(const Create & op)
{
    debug(std::cout << "Account::Operation(create)" << std::endl << std::flush;);
    const Object & ent = op.GetArgs().front();
    if (!ent.IsMap()) {
        return error(op, "Invalid character");
    }
    const Object::MapType & entmap = ent.AsMap();
    Object::MapType::const_iterator I = entmap.find("parents");
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
    info->SetArgs(Object::ListType(1,obj->asObject()));
    info->SetRefno(op.GetSerialno());
    info->SetSerialno(connection->server.getSerialNo());

    return OpVector(1,info);
}

OpVector Account::ImaginaryOperation(const Imaginary & op)
{
    const Object::ListType & args = op.GetArgs();
    if ((!args.empty()) && (args.front().IsMap())) {
        const Object::MapType & arg = args.front().AsMap();
        Object::MapType::const_iterator I = arg.find("loc");
        if (I != arg.end()) {
            Sight s(Sight::Instantiate());
            s.SetArgs(Object::ListType(1,op.AsObject()));
            s.SetTo(I->second.AsString());
            s.SetFrom(getId());
            s.SetSerialno(connection->server.getSerialNo());
            return connection->server.lobby.operation(s);
        }
    }
    return OpVector();
}

OpVector Account::TalkOperation(const Talk & op)
{
    const Object::ListType & args = op.GetArgs();
    if ((!args.empty()) && (args.front().IsMap())) {
        Sound s(Sound::Instantiate());
        s.SetArgs(Object::ListType(1,op.AsObject()));
        s.SetFrom(getId());
        s.SetSerialno(connection->server.getSerialNo());
        const Object::MapType & arg = args.front().AsMap();
        Object::MapType::const_iterator I = arg.find("loc");
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
    const Object::ListType & args = op.GetArgs();
    if (args.empty()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        s->SetArgs(Object::ListType(1,connection->server.lobby.asObject()));
        s->SetSerialno(connection->server.getSerialNo());
        return OpVector(1,s);
    }
    Object::MapType::const_iterator I = args.front().AsMap().find("id");
    if ((I == args.front().AsMap().end()) || (!I->second.IsString())) {
        return error(op, "No target for look");
    }
    const std::string & to = I->second.AsString();
    EntityDict::const_iterator J = charactersDict.find(to);
    if (J != charactersDict.end()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        s->SetArgs(Object::ListType(1,J->second->asObject()));
        s->SetSerialno(connection->server.getSerialNo());
        return OpVector(1,s);
    }
    const AccountDict & accounts = connection->server.lobby.getAccounts();
    AccountDict::const_iterator K = accounts.find(to);
    if (K != accounts.end()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        s->SetArgs(Object::ListType(1,K->second->asObject()));
        s->SetSerialno(connection->server.getSerialNo());
        return OpVector(1,s);
    }
    return error(op, "Unknown look target");
}

void Account::checkCharacters()
{
    if (world == NULL) {
        std::cerr << "WARNING: Account " << getId()
                  << " beging asked to check characters" << std::endl
                  << "but it is not currently connected to a world"
                  << std::endl << std::flush;
        return;
    }
    std::set<std::string> obsoleteChars;
    const EntityDict & worldEntities = world->getObjects();
    EntityDict::iterator I = charactersDict.begin();
    for(; I != charactersDict.end(); I++) {
        const std::string & charId = I->first;
        if (worldEntities.find(charId) == worldEntities.end()) {
            obsoleteChars.insert(charId);
        }
    }
    std::set<std::string>::iterator J = obsoleteChars.begin();
    for(; J != obsoleteChars.end(); J++) {
        std::cout << "Removing character " << *J << " from account "
                  << getId() << std::endl << std::flush;
        charactersDict.erase(*J);
    }
}
