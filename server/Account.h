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

#ifndef SERVER_ACCOUNT_H
#define SERVER_ACCOUNT_H

#include "common/OOGThing.h"

#include <sigc++/connection.h>

class Connection;

/// \brief This is the base class for storing information about uses who
/// can use this server.
///
/// The majority of functionality relating to user accounts is encapsulated
/// here. Sub-classes control privilege levels by implementing
/// characterError().
class Account : public OOGThing {
  protected:
    typedef std::map<long, SigC::Connection *> ConMap;

    EntityDict m_charactersDict;
    ConMap m_destroyedConnections;

    Entity * addNewCharacter(const std::string &,
                             const Atlas::Objects::Entity::RootEntity &);
    void characterDestroyed(long);

    virtual int characterError(const Operation &,
                               const Atlas::Objects::Entity::RootEntity &,
                               OpVector &) const = 0;

  public:
    Connection * m_connection;
    std::string m_username;
    std::string m_password;

    Account(Connection * conn, const std::string & username,
                               const std::string & passwd,
                               const std::string & id, long intId);
    virtual ~Account();

    virtual const char * getType() const;

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    virtual void LogoutOperation(const Operation &, OpVector &);
    virtual void CreateOperation(const Operation &, OpVector &);
    virtual void SetOperation(const Operation &, OpVector &);
    virtual void ImaginaryOperation(const Operation &, OpVector &);
    virtual void TalkOperation(const Operation &, OpVector &);
    virtual void LookOperation(const Operation &, OpVector &);

    void addCharacter(Entity *);

    const EntityDict & getCharacters() const {
        return m_charactersDict;
    }
};

typedef std::map<std::string, Account *> AccountDict;

#endif // SERVER_ACCOUNT_H
