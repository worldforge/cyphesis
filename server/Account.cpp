// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Info.h>

#include <common/Chop.h>
#include <common/Cut.h>
#include <common/Eat.h>
#include <common/Fire.h>

#include <common/log.h>
#include <common/debug.h>

#include <rulesets/Character.h>
#include <rulesets/World.h>
#include <rulesets/ExternalMind.h>

#include "Account.h"
#include "Connection_methods.h"
#include "WorldRouter.h"

static const bool debug_flag = false;

using Atlas::Message::Object;
using Atlas::Objects::Operation::Info;

Account::Account(Connection* conn, const string& username, const string& passwd)
               : world(NULL), connection(conn), password(passwd),
                 type("account")
{
    fullid = username;
}

Account::~Account()
{
}

BaseEntity * Account::addCharacter(const string & typestr, const Object & ent)
{
    debug(cout << "Account::Add_character" << endl << flush;);
    Thing * chr = world->addObject(typestr, ent);
    debug(cout << "Added" << endl << flush;);
    if (!chr->location) {
        debug(cout << "Setting location" << endl << flush;);
        chr->location.ref = &world->gameWorld;
        chr->location.coords = Vector3D(0, 0, 0);
    }
    debug(cout << "Location set to: " << chr->location << endl << flush;);
    if (chr->isCharacter == true) {
        Character * pchar = (Character *)chr;
        pchar->externalMind = new ExternalMind(*connection, pchar->fullid, pchar->name);
    }
    charactersDict[chr->fullid]=chr;
    connection->addObject(chr);

    Create c = Create::Instantiate();

    Object::ListType cargs(1,chr->asObject());
    c.SetArgs(cargs);

    Sight * s = new Sight(Sight::Instantiate());
    
    Object::ListType sargs(1,c.AsObject());
    s->SetArgs(sargs);

    world->message(*s, chr);

    return(chr);
}

oplist Account::Operation(const Logout & op)
{
    debug(cout << "Account logout: " << fullid << endl;);
    connection->destroy();
    return oplist();
}

void Account::addToObject(Object & obj) const
{
    Object::MapType & omap = obj.AsMap();
    omap["id"] = Object(fullid);
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

oplist Account::Operation(const Create & op)
{
    debug(cout << "Account::Operation(create)" << endl << flush;);
    const Object & ent = op.GetArgs().front();
    if (!ent.IsMap()) {
        return(error(op, "Invalid character"));
    }
    const Object::MapType & entmap = ent.AsMap();
    Object::MapType::const_iterator I = entmap.find("parents");
    if ((I == entmap.end()) || !(I->second.IsList()) ||
        (I->second.AsList().size()==0) ||
        !(I->second.AsList().front().IsString()) ) {
        return(error(op, "Character has no type"));
    }
    
    oplist error = characterError(op, entmap);
    if (error.size() != 0) {
        return(error);
    }
    const string & typestr = I->second.AsList().front().AsString();
    debug(cout << "Account creating a " << typestr << " object" << endl << flush;);

    BaseEntity * obj = addCharacter(typestr, ent);
    //log.inform("Player "+Account::id+" adds character "+`obj`,op);
    Info * info = new Info(Info::Instantiate());
    Object::ListType args(1,obj->asObject());
    info->SetArgs(args);
    info->SetRefno(op.GetSerialno());

    return oplist(1,info);
}
