// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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


#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include "common/Link.h"

#include <sigc++/trackable.h>

#include <list>
#include <deque>
#include <memory>

class Account;

class CommSocket;

class LocatedEntity;

class ServerRouting;

struct ConnectableRouter;


struct RouterWithQueue
{
    Router* router;
    /**
     * Operations send by the router, to be processed by calls to "dispatch".
     */
    std::deque<Operation> opsQueue;
};

typedef std::map<long, RouterWithQueue> RouterMap;

/// \brief Class representing connections from a client at the Atlas level.
///
/// This is the first point of dispatch for any operation from the client.
/// It maintains a dictionary of entities which are associated with this
/// connection, such as Account objects that the client has logged into,
/// and any other entities that that are associated with those accounts,
/// like in-game characters. Clients specify which entity should handle
/// an operation using the from attribute.
class Connection : public Link, virtual public sigc::trackable
{
    protected:

        /**
         * A queue of incoming ops.
         * Entries are put here by "externalOperation" and processed by calls to "dispatch".
         */
        std::deque<Operation> m_operationsQueue;
        RouterMap m_objects;

        std::map<long, ConnectableRouter*> m_connectableRouters;

        std::list<std::string> m_possessionRouters;

        Account* addNewAccount(const std::string& account,
                               const std::string& username,
                               const std::string& password);

        void disconnectObject(ConnectableRouter* router,
                              const std::string& event);

        virtual std::unique_ptr<Account> newAccount(const std::string& type,
                                    const std::string& username,
                                    const std::string& passwd,
                                    const std::string& id, long intId);

        virtual int verifyCredentials(const Account&,
                                      const Atlas::Objects::Root&) const;

    public:
        ServerRouting& m_server;

        Connection(CommSocket& commSocket, ServerRouting& svr,
                   const std::string& addr, const std::string& id, long iid);

        ~Connection() override;

        RouterMap& objects()
        { return m_objects; }

        /**
         * Turns on and off possession ability for the specified router id.
         *
         * Typically a router id is an Account instance.
         *
         * @param enabled
         */
        void setPossessionEnabled(bool enabled, const std::string& routerId);

        void addObject(Router* obj);

        void addConnectableRouter(ConnectableRouter* obj);

        void removeObject(long id);

        void externalOperation(const Operation& op, Link&) override;

        void operation(const Operation&, OpVector&) override;

        virtual void LoginOperation(const Operation&, OpVector&);

        virtual void LogoutOperation(const Operation&, OpVector&);

        virtual void CreateOperation(const Operation&, OpVector&);

        virtual void GetOperation(const Operation&, OpVector&);

        /**
         * Dispatches incoming ops.
         * @param numberOfOps
         * @return
         */
        size_t dispatch(size_t numberOfOps);

        friend class Connectiontest;

        friend class ConnectionShakerintegration;

        friend class AccountConnectionCharacterintegration;
};

#endif // SERVER_CONNECTION_H
