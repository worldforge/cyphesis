// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Create.h>

#include "Player.h"

using Atlas::Message::Object;

Player::Player(Connection* conn, const std::string& username,
               const std::string& passwd) : Account(conn, username, passwd)
{
    type = "player";
}

Player::~Player() { }

oplist Player::characterError(const Create& op,const Object::MapType& ent) const
{
    Object::MapType::const_iterator I = ent.find("name");
    if ((I == ent.end()) || !I->second.IsString()) {
        return error(op, "Object to be created has no name");
    }

#if defined(__GNUC__) && __GNUC__ < 3
    if (!I->second.AsString().compare("admin",0,5)) {
        return error(op, "Object to be created cannot start with admin");
    }
#else
    if (!I->second.AsString().compare(0,5,"admin")) {
        return error(op, "Object to be created cannot start with admin");
    }
#endif

    const std::string& type= ent.find("parents")->second.AsList().front().AsString(); 
    if ((type!="character") && (type!="archer") && (type!="druid") &&
        (type!="farmer") && (type!="khatinid") && (type!="guard") &&
        (type!="butcher") && (type!="merchant") && (type!="seller") &&
        (type!="swineherd") && (type!="warrior") && (type!="watermage")) {
        return error(op, "Object of that type cannot be created by this account");
    }
    return oplist();
}
