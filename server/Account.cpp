// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Info.h>

#include <common/log.h>
#include <common/debug.h>

#include <rulesets/Character.h>
#include <rulesets/World.h>

#include "Account.h"
#include "Connection_methods.h"
#include "WorldRouter.h"
#include "ServerRouting.h"
#include "Lobby.h"
#include "ExternalMind.h"

static const bool debug_flag = false;

using Atlas::Message::Object;
using Atlas::Objects::Operation::Info;

Account::Account(Connection * conn, const std::string & username,
                 const std::string& passwd)
                 : world(NULL), connection(conn), password(passwd),
                   type("account")
{
    setId(username);
}

Account::~Account()
{
}

BaseEntity * Account::addCharacter(const std::string & typestr, const Object & ent)
{
    debug(cout << "Account::Add_character" << endl << flush;);
    Entity * chr = world->addObject(typestr, ent);
    debug(cout << "Added" << endl << flush;);
    if (!chr->location) {
        debug(cout << "Setting location" << endl << flush;);
        chr->location.ref = &world->gameWorld;
        chr->location.coords = Vector3D(0, 0, 0);
    }
    debug(cout << "Location set to: " << chr->location << endl << flush;);
    if (chr->isCharacter == true) {
        Character * pchar = (Character *)chr;
        pchar->externalMind = new ExternalMind(*connection, pchar->getId(), pchar->name);
    }
    charactersDict[chr->getId()]=chr;
    connection->addObject(chr);

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

oplist Account::LogoutOperation(const Logout & op)
{
    debug(cout << "Account logout: " << getId() << endl;);
    connection->destroy();
    return oplist();
}

void Account::addToObject(Object::MapType & omap) const
{
    omap["id"] = Object(getId());
    if (password.size() != 0) {
        omap["password"] = Object(password);
    }
    omap["parents"] = Object(Object::ListType(1,Object(type)));
    Object::ListType charlist;
    edict_t::const_iterator I;
    for(I = charactersDict.begin(); I != charactersDict.end(); I++) {
        charlist.push_back(Object(I->first));
    }
    omap["characters"] = Object(charlist);
    // No need to call BaseEntity::addToObject, as none of the default
    // attributes (location, contains etc.) are relevant to accounts
}

oplist Account::CreateOperation(const Create & op)
{
    debug(cout << "Account::Operation(create)" << endl << flush;);
    const Object & ent = op.GetArgs().front();
    if (!ent.IsMap()) {
        return error(op, "Invalid character");
    }
    const Object::MapType & entmap = ent.AsMap();
    Object::MapType::const_iterator I = entmap.find("parents");
    if ((I == entmap.end()) || !(I->second.IsList()) ||
        (I->second.AsList().size()==0) ||
        !(I->second.AsList().front().IsString()) ) {
        return error(op, "Character has no type");
    }
    
    oplist error = characterError(op, entmap);
    if (error.size() != 0) {
        return error;
    }
    const std::string & typestr = I->second.AsList().front().AsString();
    debug(cout << "Account creating a " << typestr << " object" << endl << flush;);

    BaseEntity * obj = addCharacter(typestr, ent);
    //log.inform("Player "+Account::id+" adds character "+`obj`,op);
    Info * info = new Info(Info::Instantiate());
    info->SetArgs(Object::ListType(1,obj->asObject()));
    info->SetRefno(op.GetSerialno());

    return oplist(1,info);
}

oplist Account::ImaginaryOperation(const Imaginary & op)
{
    Sight s(Sight::Instantiate());
    s.SetArgs(Object::ListType(1,op.AsObject()));
    s.SetTo(op.GetTo());
    s.SetFrom(getId());
    return connection->server.lobby.operation(s);
}

oplist Account::TalkOperation(const Talk & op)
{
    Sound s(Sound::Instantiate());
    s.SetArgs(Object::ListType(1,op.AsObject()));
    s.SetTo(op.GetTo());
    s.SetFrom(getId());
    return connection->server.lobby.operation(s);
}

oplist Account::LookOperation(const Look & op)
{
    const std::string & to = op.GetTo();
    if (to.empty()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        s->SetArgs(Object::ListType(1,connection->server.lobby.asObject()));
        return oplist(1,s);
    }
    edict_t::const_iterator I = charactersDict.find(to);
    if (I != charactersDict.end()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        s->SetArgs(Object::ListType(1,I->second->asObject()));
        return oplist(1,s);
    }
    const adict_t & accounts = connection->server.lobby.getAccounts();
    adict_t::const_iterator J = accounts.find(to);
    if (J != accounts.end()) {
        Sight * s = new Sight(Sight::Instantiate());
        s->SetTo(getId());
        s->SetArgs(Object::ListType(1,J->second->asObject()));
        return oplist(1,s);
    }
    return error(op, "Unknown look target");
}
