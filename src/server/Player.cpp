// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "Player.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

#include "common/compose.hpp"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

int Player::s_numberOfPlayers = 0;

/// \brief Constructor
///
/// @param conn the Connection this account is being created by.
/// @param username a string giving the username.
/// @param passwd a string giving the password.
/// @param id a string giving the identifier of the account.
/// @param intId an integer giving the identifier of the account.
Player::Player(Connection * conn,
               const std::string & username,
               const std::string & passwd,
               RouterId id) :
        Account(conn, username, passwd, std::move(id))
{
    s_numberOfPlayers++;
}

const char * Player::getType() const
{
    return "player";
}

Player::~Player() {
    s_numberOfPlayers--;
}
