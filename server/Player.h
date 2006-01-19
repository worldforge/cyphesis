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

#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "Account.h"

/// \brief This is a class for handling users who are regular players
class Player : public Account {
  protected:
    virtual int characterError(const Operation &,
                               const Atlas::Objects::Entity::RootEntity &,
                               OpVector &) const;
  public:
    Player(Connection * conn, const std::string & username,
                              const std::string & passwd,
                              const std::string & id, long intId);
    virtual ~Player();

    virtual const char * getType() const;

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    static std::set<std::string> playableTypes;
};

#endif // SERVER_PLAYER_H
