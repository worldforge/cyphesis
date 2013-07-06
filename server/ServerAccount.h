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


#ifndef SERVER_SERVER_ACCOUNT_H
#define SERVER_SERVER_ACCOUNT_H

#include "Account.h"

#include "common/Router.h"

#include <sigc++/connection.h>

/// \brief This is a class for handling users with administrative priveleges
class ServerAccount : public Account {
  protected:
    virtual int characterError(const Operation & op,
                               const Atlas::Objects::Root & ent,
                               OpVector & res) const;

    LocatedEntity * addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &,
                                 const Atlas::Objects::Root &);

    virtual void createObject(const std::string &,
                              const Atlas::Objects::Root &,
                              const Operation &,
                              OpVector &);
  public:
    ServerAccount(Connection * conn, const std::string & username,
                  const std::string & passwd,
                  const std::string & id, long intId);
    virtual ~ServerAccount();

    virtual const char * getType() const;

    friend class ServerAccounttest;
};

#endif // SERVER_SERVER_ACCOUNT_H
