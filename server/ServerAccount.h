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

// $Id$

#ifndef SERVER_SERVERACCOUNT_H
#define SERVER_SERVERACCOUNT_H

#include "Account.h"

#include "common/Router.h"

#include <sigc++/connection.h>

/// \brief This is a class for handling users with administrative priveleges
class ServerAccount : public Account {
  protected:
    virtual int characterError(const Operation & op,
                               const Atlas::Objects::Entity::RootEntity & ent,
                               OpVector & res) const;

    Entity * addNewEntity(const std::string &,
                          const Atlas::Objects::Entity::RootEntity &,
                          const Atlas::Objects::Entity::RootEntity &);

  public:
    ServerAccount(Connection * conn, const std::string & username,
                  const std::string & passwd,
                  const std::string & id, long intId);
    virtual ~ServerAccount();

    virtual const char * getType() const;

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    virtual void CreateOperation(const Operation &, OpVector &);
};

#endif // SERVER_ADMIN_H
