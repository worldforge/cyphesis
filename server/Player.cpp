// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Player.h"

#include <Atlas/Objects/Operation/Create.h>

using Atlas::Message::Object;

std::set<std::string> Player::playableTypes;

Player::Player(Connection * conn, const std::string& username,
               const std::string& passwd) : Account(conn, username, passwd)
{
    type = "player";
}

Player::~Player() { }

void Player::addToObject(Object::MapType & omap) const
{
    Account::addToObject(omap);
    Object::ListType typeList;
    std::set<std::string>::const_iterator I = Player::playableTypes.begin();
    for(;I != Player::playableTypes.end(); ++I) {
        typeList.push_back(Object(*I));
    }
    omap["character_types"] = typeList;
}

OpVector Player::characterError(const Create& op,const Object::MapType& ent) const
{
    Object::MapType::const_iterator I = ent.find("name");
    if ((I == ent.end()) || !I->second.IsString()) {
        return error(op, "Object to be created has no name");
    }

#if defined(__GNUC__) && __GNUC__ < 3
    if (!I->second.AsString().substr(0,5).compare("admin")) {
        return error(op, "Object to be created cannot start with admin");
    }
#else
    if (!I->second.AsString().compare(0,5,"admin")) {
        return error(op, "Object to be created cannot start with admin");
    }
#endif

    const std::string& type= ent.find("parents")->second.AsList().front().AsString(); 
    if (Player::playableTypes.find(type) == Player::playableTypes.end()) {
        return error(op, "You cannot create a character of this type.");
    }
    return OpVector();
}
