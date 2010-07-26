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

// $Id$

#ifndef CLIENT_CHARACTER_CLIENT_H
#define CLIENT_CHARACTER_CLIENT_H

#include "rulesets/BaseMind.h"

class ClientConnection;

/// \brief Class to implement a character entity in an admin client
class CharacterClient : public BaseMind {
  protected:
    ClientConnection & m_connection;

    int sendAndWaitReply(const Operation &, OpVector &);
    LocatedEntity * sendLook(const Operation & op);
  public:
    CharacterClient(const std::string &, long, ClientConnection&);

    void send(const Operation & op);
    LocatedEntity * look(const std::string &);
    LocatedEntity * lookFor(const Atlas::Objects::Entity::RootEntity &);
};

#endif // CLIENT_CHARACTER_CLIENT_H
