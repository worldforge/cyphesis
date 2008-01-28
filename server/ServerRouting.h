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

// $Id: ServerRouting.h,v 1.55 2008-01-28 23:48:33 alriddoch Exp $

#ifndef SERVER_SERVER_ROUTING_H
#define SERVER_SERVER_ROUTING_H

#include "Account.h"

#include "common/id.h"
#include "common/Identified.h"

#include <cassert>

class BaseWorld;
class Lobby;

typedef std::map<long, IdentifiedRouter *> RouterMap;

/// \brief ServerRouting represents the core of the server.
///
/// This class has one instance which is the core object in the server.
/// It maintains list of all out-of-game (OOG) objects in the server.
class ServerRouting : public IdentifiedRouter {
  private:
    /// A mapping of ID to object of all the OOG objects in the server.
    RouterMap m_objects;
    /// A mapping of ID to object of all the accounts in the server.
    AccountDict m_accounts;
    /// The text name of the ruleset this server is running.
    const std::string m_svrRuleset;
    /// The name of this server.
    const std::string m_svrName;
    /// The number of clients currently connected.
    int m_numClients;
  public:
    /// A reference to the World management object.
    BaseWorld & m_world;
    /// A reference to the Lobby management object.
    Lobby & m_lobby;

    ServerRouting(BaseWorld & wrld,
                  const std::string & ruleset,
                  const std::string & name,
                  const std::string & id, long intId,
                  const std::string & lId, long lIntId);
    ~ServerRouting();

    /// Add an OOG object to the server.
    void addObject(IdentifiedRouter * obj) {
        assert(!obj->getId().empty());
        assert(integerId(obj->getId()) == obj->getIntId());
        assert(obj->getIntId() > 0);
        m_objects[obj->getIntId()] = obj;
    }

    /// Add an Account object to the server.
    void addAccount(Account * a) {
        m_accounts[a->m_username] = a;
        addObject(a);
    }

    /// Remove an OOG object from the server.
    void delObject(IdentifiedRouter * obj) {
        m_objects.erase(obj->getIntId());
    }

    /// Accessor for OOG objects map.
    const RouterMap & getObjects() const {
        return m_objects;
    }

    /// \brief Find an object with the given id.
    ///
    /// @return a pointer to the object with the given id, or
    /// zero if no object with this id is present.
    IdentifiedRouter * getObject(const std::string & id) const {
        RouterMap::const_iterator I = m_objects.find(integerId(id));
        if (I == m_objects.end()) {
            return 0;
        } else {
            return I->second;
        }
    }

    /// \brief Find an account with a given username.
    ///
    /// @return a pointer to the Account object with the given
    /// username, or zero if the Account is not present. Does
    /// not check any external authentication sources, or the
    /// database.
    Account * getAccountByName(const std::string & username) const {
        AccountDict::const_iterator I = m_accounts.find(username);
        if (I == m_accounts.end()) {
            return 0;
        } else {
            return I->second;
        }
    }

    /// Increment the number of clients connected to this server.
    void incClients() { ++m_numClients; }
    /// Decrement the number of clients connected to this server.
    void decClients() { --m_numClients; }

    /// Accessor for the number of clients connected to this server.
    int getClients() { return m_numClients; }

    /// Accessor for world manager object.
    BaseWorld & getWorld() { return m_world; }

    /// Accessor for server ruleset.
    const std::string & getRuleset() const { return m_svrRuleset; }

    /// Accessor for server name.
    const std::string & getName() const { return m_svrName; }

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    virtual void operation(const Operation &, OpVector &) { /* REMOVE */ }
};

#endif // SERVER_SERVER_ROUTING_H
