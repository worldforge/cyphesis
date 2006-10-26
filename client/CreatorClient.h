// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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

// $Id: CreatorClient.h,v 1.17 2006-10-26 00:48:01 alriddoch Exp $

#ifndef CLIENT_CREATOR_CLIENT_H
#define CLIENT_CREATOR_CLIENT_H

#include "CharacterClient.h"

class Entity;

/// \brief Class to implement a creator entity in an admin client
class CreatorClient : public CharacterClient {
  private:
    Entity * sendLook(const Operation & op);
  public:
    CreatorClient(const std::string &, long, const std::string&, ClientConnection&);

    Entity * make(const Atlas::Objects::Entity::RootEntity &);
    void sendSet(const std::string &, const Atlas::Objects::Entity::RootEntity &);
    Entity * look(const std::string &);
    Entity * lookFor(const Atlas::Objects::Entity::RootEntity &);
    int runScript(const std::string & package, const std::string & function);
};

#endif // CLIENT_CREATOR_CLIENT_H
