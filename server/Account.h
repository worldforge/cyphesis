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

#ifndef SERVER_ACCOUNT_H
#define SERVER_ACCOUNT_H

#include "common/Router.h"

#include <vector>

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::vector<Element> ListType;
    }
}

class Connection;

typedef std::map<long, Entity *> EntityDict;

/// \brief This is the base class for storing information about uses who
/// can use this server.
///
/// The majority of functionality relating to user accounts is encapsulated
/// here. Sub-classes control privilege levels by implementing
/// characterError().
class Account : public Router {
  protected:
    /// \brief A store of Character entities belonging to this account
    EntityDict m_charactersDict;
    /// \brief The username of this account
    std::string m_username;
    /// \brief The password used to authenticate this account
    std::string m_password;

    Entity * addNewCharacter(const std::string &,
                             const Atlas::Objects::Entity::RootEntity &,
                             const Atlas::Objects::Root &);
    void characterDestroyed(long);

    /// \brief Check a character creation op is within the privelege levels
    /// of this account.
    ///
    /// @param op The full operation used for error reporting
    /// @param ent A RootEntity representing the character to be created
    /// @param res Any resulting error is returned here
    virtual int characterError(const Operation & op,
                               const Atlas::Objects::Root & ent,
                               OpVector & res) const = 0;

    int filterTasks(const Atlas::Message::ListType & tasks,
                    const Atlas::Objects::Entity::RootEntity &) const;

  public:
    /// \brief The network connection currently subscribed to this account
    Connection * m_connection;
    
    /// \brief Connect and add a character to this account
    int connectCharacter(Entity *chr);

    Account(Connection * conn, const std::string & username,
                               const std::string & passwd,
                               const std::string & id, long intId);
    virtual ~Account();

    const std::string & username() const {
        return m_username;
    }

    const std::string & password() const {
        return m_password;
    }

    /// \brief Get a string representation of the type of account
    virtual const char * getType() const;

    /// \brief Store this account in the database
    virtual void store() const;

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    virtual void operation(const Operation &, OpVector &);

    virtual void LogoutOperation(const Operation &, OpVector &);
    virtual void CreateOperation(const Operation &, OpVector &);
    virtual void SetOperation(const Operation &, OpVector &);
    virtual void ImaginaryOperation(const Operation &, OpVector &);
    virtual void TalkOperation(const Operation &, OpVector &);
    virtual void LookOperation(const Operation &, OpVector &);
    virtual void GetOperation(const Operation &, OpVector &);
    virtual void OtherOperation(const Operation &, OpVector &);

    void addCharacter(Entity *);

    /// \brief Read only accessor for the Character dictionary
    const EntityDict & getCharacters() const {
        return m_charactersDict;
    }
};

#endif // SERVER_ACCOUNT_H
