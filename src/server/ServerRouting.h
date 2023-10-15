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

#include <common/Singleton.h>
#include "common/Router.h"
#include "common/Shaker.h"
#include "ConnectableRouter.h"
#include "common/AssetsHandler.h"
#include <memory>
#include <set>

class Account;

class BaseWorld;

class Persistence;

class Lobby;

extern bool restricted_flag;

/// \brief ServerRouting represents the core of the server.
///
/// This class has one instance which is the core object in the server.
/// It maintains list of all out-of-game (OOG) objects in the server.
class ServerRouting
{
    private:
        /// A shaker to generate a salt.
        Shaker m_shaker;
        /// A mapping of ID to object of all the OOG objects in the server.
        /// These are all owned by this instance.
        std::map<long, std::unique_ptr<ConnectableRouter>> m_routers;
        /// A mapping of ID to object of all the accounts in the server.
        /// All of the accounts are aliases for ConnectableRouter base instances that exists in m_objects.
        std::map<std::string, Account*> m_accounts;
        /// The text name of the ruleset this server is running.
        const std::string m_svrRuleset;
        /// The name of this server.
        const std::string m_svrName;
        /// The Lobby management object.
        std::unique_ptr<Lobby> m_lobby;

        AssetsHandler m_assetsHandler;
        /// The number of clients currently connected.
        int m_numClients;
        //All client connections in the system. These aren't owned by this instance.
        std::set<Connection*> m_connections;

        long m_processOpsTotal;
    public:
        /// A reference to the World management object.
        BaseWorld& m_world;
        Persistence& m_persistence;

        ServerRouting(BaseWorld& wrld,
                      Persistence& persistence,
                      std::string ruleset,
                      std::string name,
                      RouterId lobbyId,
                      AssetsHandler assetsHandler);

        ~ServerRouting();

        void disconnectAllConnections();

        void registerConnection(Connection* connection)
        {
            m_connections.insert(connection);
            m_numClients++;
        }

        void deregisterConnection(Connection* connection)
        {
            m_connections.erase(connection);
            m_numClients--;
        }

        /// Accessor for the number of clients connected to this server.
        size_t getClients()
        { return m_connections.size(); }

        Lobby& getLobby()
        {
            return *m_lobby;
        }

        const Lobby& getLobby() const
        {
            return *m_lobby;
        }

        /// Accessor for world manager object.
        BaseWorld& getWorld()
        { return m_world; }

        /// Accesor for Shaker object.
        Shaker& getShaker()
        { return m_shaker; }

        /// Accessor for OOG objects map.
        const std::map<long, std::unique_ptr<ConnectableRouter>>& getObjects() const
        {
            return m_routers;
        }

        const std::map<std::string, Account*>& getAccounts() const
        {
            return m_accounts;
        }

        /// Accessor for server ruleset.
        const std::string& getRuleset() const
        { return m_svrRuleset; }

        /// Accessor for server name.
        const std::string& getName() const
        { return m_svrName; }

        void addRouter(std::unique_ptr<ConnectableRouter> obj);

        void addAccount(std::unique_ptr<Account> a);

        ConnectableRouter* getObject(const std::string& id) const;

        Account* getAccountByName(const std::string& username);

        void addToMessage(Atlas::Message::MapType&) const;

        void addToEntity(const Atlas::Objects::Entity::RootEntity&) const;

        /**
         * Performs dispatch of any queued operations.
         * @param numberOfOps The max number of operations to dispatch per "actor".
         * @return The total number of dispatched operations.
         */
        size_t dispatch(size_t numberOfOps);
};

#endif // SERVER_SERVER_ROUTING_H
