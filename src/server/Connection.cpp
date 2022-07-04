// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2006 Alistair Riddoch
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


#include "Connection.h"

#include "ServerRouting.h"
#include "Lobby.h"
#include "Player.h"
#include "ExternalMindsConnection.h"
#include "ExternalMindsManager.h"
#include "common/id.h"
#include "common/debug.h"
#include "common/Inheritance.h"
#include "common/system.h"
#include "common/TypeNode.h"
#include "common/log.h"
#include "common/compose.hpp"
#include "common/CommSocket.h"


#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <sigc++/adaptors/bind.h>

#include <algorithm>
#include "Remotery.h"

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Entity::Anonymous;

using String::compose;

static const bool debug_flag = false;

Connection::Connection(CommSocket& socket,
                       ServerRouting& svr,
                       const std::string& addr,
                       RouterId id) :
        Link(socket, std::move(id)), m_server(svr)
{
    m_server.registerConnection(this);
    logEvent(CONNECT, String::compose("%1 - - Connect from %2", id.m_id, addr));
}

Connection::~Connection()
{
    debug_print("destroy called")
    m_server.deregisterConnection(this);

    logEvent(DISCONNECT, String::compose("%1 - - Disconnect", getId()));

    //It's important that we disconnect ourselves as a possession router before we disconnect our objects,
    //else there's a risk that the external minds manager will just issue new possession request to this connection.
    for (auto& routerId : m_possessionRouters) {
        ExternalMindsManager::instance().removeConnection(routerId);
    }

    //Make a copy to iterate over, since disconnectObject will alter m_connectableRouters
    auto routers(m_connectableRouters);
    for (auto& entry : routers) {
        disconnectObject(entry.second, "Disconnect");
    }
}

std::unique_ptr<Account> Connection::newAccount(const std::string& type,
                                                const std::string& username,
                                                const std::string& hash,
                                                RouterId id)
{
    return std::make_unique<Player>(this, username, hash, id);
}

static const int hash_salt_size = 8;

Account* Connection::addNewAccount(const std::string& type,
                                   const std::string& username,
                                   const std::string& password)
{
    std::string hash;
    std::string salt = m_server.getShaker().generateSalt(hash_salt_size);
    hash_password(password, salt, hash);

    auto id = newId();
    if (id.m_intId < 0) {
        log(ERROR, "Account creation failed as no ID available");
        return nullptr;
    }

    auto account = newAccount(type, username, hash, id);
    if (!account) {
        return nullptr;
    }
    addConnectableRouter(account.get());
    assert(account->getConnection() == this);
    m_server.getLobby().addAccount(account.get());
    auto accountPtr = account.get();
    m_server.addAccount(std::move(account));
    return accountPtr;
}

/// \brief Remove an object from this connection.
///
/// The object being removed may be a player, or another type of object such
/// as an avatar.
void Connection::disconnectObject(ConnectableRouter* router,
                                  const std::string& event)
{
    m_server.getLobby().removeAccount(router);
    router->setConnection(nullptr);
    m_connectableRouters.erase(router->getIntId());
    removeRouter(router->getIntId());
}

void Connection::setPossessionEnabled(bool enabled, const std::string& routerId)
{
    if (enabled) {
        ExternalMindsConnection connection(this, routerId);
        auto result = ExternalMindsManager::instance().addConnection(connection);
        if (result == 0) {
            m_possessionRouters.push_back(routerId);
        }
    } else {
        auto result = std::find(m_possessionRouters.begin(), m_possessionRouters.end(), routerId);
        if (result != m_possessionRouters.end()) {
            ExternalMindsManager::instance().removeConnection(routerId);
        }
    }
}


void Connection::addRouter(Router* obj)
{
    m_routers[obj->getIntId()].router = obj;
}

void Connection::addConnectableRouter(ConnectableRouter* obj)
{
    obj->setConnection(this);
    addRouter(obj);
    m_connectableRouters[obj->getIntId()] = obj;
}


void Connection::removeRouter(long id)
{
    m_routers.erase(id);
}

int Connection::verifyCredentials(const Account& account,
                                  const Root& creds) const
{
    Element passwd_attr;
    if (creds->copyAttr("password", passwd_attr) != 0 || !passwd_attr.isString()) {
        return -1;
    }
    const std::string& passwd = passwd_attr.String();

    return check_password(passwd, account.password());
}

size_t Connection::dispatch(size_t numberOfOps)
{
    rmt_ScopedCPUSample(Connection_dispatch, 0)
    size_t processed = 0;

    while (!m_operationsQueue.empty() && processed < numberOfOps) {
        rmt_ScopedCPUSample(dispatch_operation, 0)
        auto op = std::move(m_operationsQueue.front());
        m_operationsQueue.pop_front();
        debug_print("deliver locally")
        OpVector reply;
        long serialno = op->getSerialno();
//        if (debug_flag) {
//            auto timeDiff = BaseWorld::instance().getTimeAsSeconds() - op->getSeconds();
//            if (timeDiff > 0.02) {
//                log(WARNING, String::compose("Time diff for connection %1: %2", getId(), timeDiff));
//            }
//        }
        operation(op, reply);

        if (!reply.empty()) {
            for (auto& replyOp : reply) {
                if (!op->isDefaultSerialno()) {
                    // Should we respect existing refnos?
                    if (replyOp->isDefaultRefno()) {
                        replyOp->setRefno(serialno);
                    }
                }
            }
            // FIXME detect socket failure here
            send(reply);
        }
        processed++;
    }

    for (auto& entry: m_routers) {
        if (!entry.second.opsQueue.empty()) {
            rmt_ScopedCPUSample(dispatch_externalOperations, 0)
            size_t processedOps = 0;
            while (!entry.second.opsQueue.empty() && processedOps < numberOfOps) {
                rmt_ScopedCPUSample(dispatch_externalOperation, 0)
                auto op = std::move(entry.second.opsQueue.front());
                entry.second.opsQueue.pop_front();
//            if (debug_flag) {
//                auto timeDiff = BaseWorld::instance().getTimeAsSeconds() - op->getSeconds();
//                if (timeDiff > 0.02) {
//                    log(WARNING, String::compose("Time diff for router %1:  %2", entry.second.router->getId(), timeDiff));
//                }
//            }

                entry.second.router->externalOperation(op, *this);
                processedOps++;
            }
            processed += processedOps;
        }
    }

    m_commSocket.flush();


    return processed;
}

size_t Connection::queuedOps() const
{
    size_t counter = m_operationsQueue.size();
    for (auto& entry : m_routers) {
        counter += entry.second.opsQueue.size();
    }
    return counter;
}


void Connection::externalOperation(const Operation& op, Link& link)
{
    debug_print("Connection::externalOperation")
    //log(INFO, String::compose("externalOperation in %1", getId()));

    if (op->isDefaultFrom()) {
        m_operationsQueue.emplace_back(op);
        if (m_operationsQueue.size() > 1000) {
            log(WARNING, String::compose("Operations queue for connection %1 is alarmingly high, currently at %2. New op of type '%3'.",
                                         getId(), m_operationsQueue.size(), op->getParent()));
        }
    } else {
        auto& from = op->getFrom();
        debug_print("send on to " << from)
        auto I = m_routers.find(integerId(from));
        if (I == m_routers.end()) {
            sendError(op, String::compose("Client \"%1\" op from \"%2\" is from non-existent object.",
                                          op->getParent(), from), from);
            return;
        } else {
            I->second.opsQueue.emplace_back(op);
            if (I->second.opsQueue.size() > 1000) {
                log(WARNING, String::compose("Operations queue for router %1 is alarmingly high, currently at %2. New op of type '%3'.",
                                             I->second.router->getId(), I->second.opsQueue.size(), op->getParent()));
            }
        }
    }
}

void Connection::operation(const Operation& op, OpVector& res)
{
    debug_print("Connection::operation")
    auto op_no = op->getClassNo();
    switch (op_no) {
        case Atlas::Objects::Operation::CREATE_NO:
            CreateOperation(op, res);
            break;
        case Atlas::Objects::Operation::GET_NO:
            GetOperation(op, res);
            break;
        case Atlas::Objects::Operation::LOGIN_NO:
            LoginOperation(op, res);
            break;
        case Atlas::Objects::Operation::LOGOUT_NO:
            LogoutOperation(op, res);
            break;
        case OP_INVALID:
            break;
        default:
            std::string parent = op->getParent().empty() ? "-" : op->getParent();
            error(op, String::compose("Unknown operation %1 in Connection", parent), res);
            break;
    }
}

void Connection::LoginOperation(const Operation& op, OpVector& res)
{
    debug_print("Got login op")
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        error(op, "Login has no argument", res);
        return;
    }
    // Account should be the first argument of the op
    const Root& arg = args.front();
    // Check for username, and if it's not there, then check for
    // id in case we are dealing with an old client.
    Element user_attr;
    std::string username;
    if (arg->copyAttr("username", user_attr) != 0 || !user_attr.isString()) {
        log(WARNING, "Got Login for account with no username. Checking for old style Login.");
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "Got account Login with no username.", res);
            return;
        }
        username = arg->getId();
    } else {
        username = user_attr.String();
    }
    if (username.empty()) {
        error(op, "Empty username provided for Login", res);
        return;
    }

    // We now have username, so can check whether we know this
    // account, either from existing account ....
    Account* account = m_server.getAccountByName(username);
    // or if not, from the database
    if (account == nullptr || verifyCredentials(*account, arg) != 0) {
        clientError(op, "Login is invalid", res);
        return;
    }
    // Account appears to be who they say they are
    if (account->getConnection()) {
        // Internals don't allow player to log in more than once.
        clientError(op, "This account is already logged in", res);
        return;
    }
    // Connect everything up
    addConnectableRouter(account);
    m_server.getLobby().addAccount(account);
    // Let the client know they have logged in
    Info info;
    Anonymous info_arg;
    account->addToEntity(info_arg);
    info->setArgs1(info_arg);
    debug_print("Good login")
    res.emplace_back(std::move(info));

    logEvent(LOGIN, String::compose("%1 %2 - Login account %3 (%4)",
                                    getId(), account->getId(), username,
                                    account->getType()));
}

void Connection::CreateOperation(const Operation& op, OpVector& res)
{
    debug_print("Got create op")
    if (!m_routers.empty()) {
        clientError(op, "This connection is already logged in", res);
        return;
    }
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        error(op, "Create has no argument", res);
        return;
    }
    const Root& arg = args.front();

    if (restricted_flag) {
        error(op, "Account creation on this server is restricted", res);
        return;
    }

    Element user_attr;
    std::string username;
    if (arg->copyAttr("username", user_attr) != 0 || !user_attr.isString()) {
        error(op, "Got account Create with no username.", res);
        return;
    } else {
        username = user_attr.String();
    }

    Element passwd_attr;
    if (arg->copyAttr("password", passwd_attr) != 0 || !passwd_attr.isString()) {
        error(op, "No account password given", res);
        return;
    }
    const std::string& password = passwd_attr.String();

    if (username.empty() || password.empty() ||
        (nullptr != m_server.getAccountByName(username))) {
        // Account exists, or creation data is duff
        clientError(op, "Account creation is invalid", res);
        return;
    }
    std::string type("player");
    if (!arg->isDefaultParent()) {
        type = arg->getParent();
    }
    Account* account = addNewAccount(type, username, password);
    if (account == nullptr) {
        clientError(op, "Account creation failed", res);
        return;
    }
    Info info;
    Anonymous info_arg;
    account->addToEntity(info_arg);
    info->setArgs1(info_arg);
    debug_print("Good create")
    res.push_back(info);

    logEvent(LOGIN, String::compose("%1 %2 - Create account %3 (%4)",
                                    getId(),
                                    account->getId(),
                                    username,
                                    account->getType()));
}

void Connection::LogoutOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        // Logging self out
        Info info;
        info->setArgs1(op);
        if (!op->isDefaultSerialno()) {
            info->setRefno(op->getSerialno());
        }
        send(info);
        disconnect();
        return;
    }
    const Root& arg = args.front();

    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Got logout for entity with no ID", res);
        return;
    }
    const long obj_id = integerId(arg->getId());
    if (obj_id == -1) {
        error(op, "Got logout for non numeric entity ID", res);
        return;
    }
    auto I = m_connectableRouters.find(obj_id);
    if (I == m_connectableRouters.end()) {
        error(op, String::compose("Got logout for unknown entity ID(%1)",
                                  obj_id),
              res);
        return;
    }
    disconnectObject(I->second, "Logout");

    Info info;
    info->setArgs1(op);
    if (!op->isDefaultSerialno()) {
        info->setRefno(op->getSerialno());
    }
    res.push_back(info);
}

void Connection::GetOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();

    Info info;
    if (args.empty()) {
        Anonymous info_arg;
        m_server.addToEntity(info_arg);
        info->setArgs1(std::move(info_arg));
        debug_print("Replying to empty get")
    } else {
        const Root& arg = args.front();
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "Type definition requested with no id", res);
            return;
        }
        const std::string& id = arg->getId();
        debug_print("Get got for " << id)
        Atlas::Objects::Root o = Inheritance::instance().getClass(id, Visibility::PUBLIC);
        if (!o.isValid()) {
            error(op, String::compose("Unknown type definition for \"%1\" requested", id), res);
            return;
        }
        info->setArgs1(o);
    }

    res.emplace_back(std::move(info));
}
