// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Player.h"

#include "common/types.h"

#include <Atlas/Objects/Operation/Create.h>

std::set<std::string> Player::playableTypes;

Player::Player(Connection * conn, const std::string& username,
               const std::string& passwd, const std::string & id) :
               Account(conn, username, passwd, id)
{
}

Player::~Player() { }

const char * Player::getType() const
{
    return "player";
}

void Player::addToMessage(MapType & omap) const
{
    Account::addToMessage(omap);
    ListType & typeList = (omap["character_types"] = ListType()).asList();
    std::set<std::string>::const_iterator I = Player::playableTypes.begin();
    std::set<std::string>::const_iterator Iend = Player::playableTypes.end();
    for (; I != Iend; ++I) {
        typeList.push_back(Element(*I));
    }
}

int Player::characterError(const Create & op,
                            const MapType & ent, OpVector & res) const
{
    MapType::const_iterator I = ent.find("name");
    if ((I == ent.end()) || !I->second.isString()) {
        error(op, "Entity to be created has no name", res, getId());
        return true;
    }

#if defined(__GNUC__) && __GNUC__ < 3
    if (!I->second.asString().substr(0,5).compare("admin")) {
        error(op, "Entity to be created cannot start with admin", res, getId());
        return true;
    }
#else
    if (!I->second.asString().compare(0,5,"admin")) {
        error(op, "Entity to be created cannot start with admin", res, getId());
        return true;
    }
#endif

    I = ent.find("parents");
    if ((I == ent.end()) || !I->second.isList()) {
        error(op, "You cannot create a character with no type.", res, getId());
        return true;
    }
    const ListType & parents = I->second.asList();
    if (parents.empty() || !parents.front().isString()) {
        error(op, "You cannot create a character with non-string type.", res, getId());
        return true;
    }
    const std::string& type = parents.front().asString(); 
    if (Player::playableTypes.find(type) == Player::playableTypes.end()) {
        error(op, "You cannot create a character of this type.", res, getId());
        return true;
    }
    return false;
}
