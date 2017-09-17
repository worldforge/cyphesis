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


#ifndef SERVER_SERVER_ROUTING_H
#define SERVER_SERVER_ROUTING_H

#include "common/Router.h"
#include "common/Shaker.h"

class Account;
class BaseWorld;
class Lobby;

typedef std::map<long, Router *> RouterMap;
typedef std::map<std::string, Account *> AccountDict;

extern bool restricted_flag;

/// \brief ServerRouting represents the core of the server.
///
/// This class has one instance which is the core object in the server.
/// It maintains list of all out-of-game (OOG) objects in the server.
class ServerRouting : public Router {
  protected:
    /// A shaker to generate a salt.
    Shaker m_shaker;
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
    /// Static self object for external access
    static ServerRouting * m_instance;
  public:
    /// A reference to the World management object.
    BaseWorld & m_world;
    /// A reference to the Lobby management object.
    Lobby & m_lobby;

    static ServerRouting * instance() {
        return m_instance;
    }

    ServerRouting(BaseWorld & wrld,
                  const std::string & ruleset,
                  const std::string & name,
                  const std::string & id, long intId,
                  const std::string & lId, long lIntId);
    ~ServerRouting();

    /// Increment the number of clients connected to this server.
    void incClients() { ++m_numClients; }
    /// Decrement the number of clients connected to this server.
    void decClients() { --m_numClients; }

    /// Accessor for the number of clients connected to this server.
    int getClients() { return m_numClients; }

    /// Accessor for world manager object.
    BaseWorld & getWorld() { return m_world; }
    
    /// Accesor for Shaker object.
    Shaker & getShaker() { return m_shaker; }
    
    /// Accessor for OOG objects map.
    const RouterMap & getObjects() const {
        return m_objects;
    }

    /// Accessor for server ruleset.
    const std::string & getRuleset() const { return m_svrRuleset; }

    /// Accessor for server name.
    const std::string & getName() const { return m_svrName; }

    void addObject(Router * obj);
    void addAccount(Account * a);
    void delObject(Router * obj);
    Router * getObject(const std::string & id) const;
    Account * getAccountByName(const std::string & username);

    void addToMessage(Atlas::Message::MapType &) const override;
    void addToEntity(const Atlas::Objects::Entity::RootEntity &) const override;

    void externalOperation(const Operation & op, Link &) override;
    void operation(const Operation &, OpVector &) override;
};

#endif // SERVER_SERVER_ROUTING_H
