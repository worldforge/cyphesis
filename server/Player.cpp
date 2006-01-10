// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Player.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

#include "common/compose.hpp"

std::set<std::string> Player::playableTypes;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Entity::RootEntity;

Player::Player(Connection * conn, const std::string& username,
               const std::string& passwd, const std::string & id, long intId) :
               Account(conn, username, passwd, id, intId)
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

void Player::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
    Account::addToEntity(ent);
    ListType typeList;
    std::set<std::string>::const_iterator I = Player::playableTypes.begin();
    std::set<std::string>::const_iterator Iend = Player::playableTypes.end();
    for (; I != Iend; ++I) {
        typeList.push_back(Element(*I));
    }
    ent->setAttr("character_types", typeList);
}

int Player::characterError(const Operation & op,
                           const RootEntity & ent, OpVector & res) const
{
    if (!ent->hasAttrFlag(Atlas::Objects::NAME_FLAG)) {
        error(op, "Entity to be created has no name", res, getId());
        return true;
    }
    const std::string & name = ent->getName();
#if defined(__GNUC__) && __GNUC__ < 3
    if (!name.substr(0,5).compare("admin")) {
        error(op, "Entity to be created cannot start with admin", res, getId());
        return true;
    }
#else
    if (!name.compare(0,5,"admin")) {
        error(op, "Entity to be created cannot start with admin", res, getId());
        return true;
    }
#endif

    // Parents must have been checked already before calling this method
    const std::string & type = ent->getParents().front(); 
    if (Player::playableTypes.find(type) == Player::playableTypes.end()) {
        error(op, String::compose("You cannot create a character of type \"%1\".", type).c_str(), res, getId());
        return true;
    }
    return false;
}
