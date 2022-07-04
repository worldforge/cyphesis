// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
// Copyright (C) 2013 Erik Ogenvik
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

#include "BaseClient.h"

#include "../common/log.h"
#include "../common/debug.h"
#include "../common/compose.hpp"
#include "../common/system.h"
#include "../common/CommSocket.h"
#include "../common/ClientTask.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

using String::compose;

static const bool debug_flag = false;

BaseClient::BaseClient(CommSocket& commSocket)
    : Link(commSocket, 0), m_serialNo(0)
{

}


void BaseClient::externalOperation(const Operation& op, Link& link)
{

    if (debug_flag) {
        std::cout << "BaseClient::externalOperation received {" << std::endl;
        debug_dump(op, std::cout);
        std::cout << "}" << std::endl << std::flush;
    }

    OpVector res;

    if (m_task) {
        m_task->operation(op, res);

        if (m_task->isComplete()) {
            m_task.reset();
        }
    } else {


        if (op->isDefaultTo() && !op->isDefaultRefno()) {
            auto I = m_callbacks.find(op->getRefno());
            if (I != m_callbacks.end()) {
                I->second.timeout->cancel();
                I->second.callback(op, res);
                m_callbacks.erase(I);
            }
        } else {
            operation(op, res);
        }
    }

    link.send(res);

    if (debug_flag) {
        for (auto resOp : res) {
            std::cout << "BaseClient::externalOperation sent {" << std::endl;
            debug_dump(resOp, std::cout);
            std::cout << "}" << std::endl << std::flush;
        }
    }

}

/// \brief Create a new account on the server
///
/// @param name User name of the new account
/// @param password Password of the new account
void BaseClient::createSystemAccount(const std::string& usernameSuffix)
{

    Anonymous player_ent;
    m_username = create_session_username() + usernameSuffix;
    player_ent->setAttr("username", m_username);
    m_password = compose("%1%2", ::rand(), ::rand());
    player_ent->setAttr("password", m_password);
    player_ent->setParent("sys");

    Create createAccountOp;
    createAccountOp->setArgs1(player_ent);
    sendWithCallback(createAccountOp, [&](const Operation& op, OpVector& res) {
                         if (!op->getArgs().empty()) {
                             auto ent = op->getArgs().front();
                             if (!ent->isDefaultId()) {
                                 notifyAccountCreated(RouterId(ent->getId()));
                             } else {
                                 log(ERROR, "ERROR: Logged in, but account has no id.");
                             }
                         }
                     },
                     []() {
                         std::cerr << "ERROR: Failed to log into server: \""
                                   //<< m_connection.errorMessage() << "\""
                                   << std::endl << std::flush;
                     });
}

void BaseClient::logout()
{
    Logout logout;
    send(logout);
}


std::string BaseClient::getErrorMessage(const Operation& err)
{
    const std::vector<Root>& args = err->getArgs();
    if (args.empty()) {
        return "Unknown error.";
    } else {
        const Root& arg = args.front();
        Atlas::Message::Element message;
        if (arg->copyAttr("message", message) != 0) {
            return "Unknown error.";
        } else {
            if (!message.isString()) {
                return "Unknown error.";
            } else {
                return message.String();
            }
        }
    }
}

int BaseClient::runTask(std::shared_ptr<ClientTask> task, const std::string& arg)
{

    if (m_task != nullptr) {
        std::cout << "Busy" << std::endl << std::flush;
        return -1;
    }

    m_task = std::move(task);

    OpVector res;

    m_task->setup(arg, res);

    if (m_task->isComplete()) {
        m_task.reset();
        return -1;
    }

    send(res);

    return 0;
}

int BaseClient::runTask(std::function<bool(const Operation&, OpVector&)> function)
{
    return runTask(std::make_shared<FunctionClientTask>(function), "");
}


int BaseClient::endTask()
{
    m_task.reset();
    return 0;
}

/**
 * Checks if there's an active task.
 * @return True if there's a task set.
 */
bool BaseClient::hasTask() const
{
    return (bool)m_task;
}


void BaseClient::notifyConnectionComplete()
{
    createSystemAccount();
}

void BaseClient::sendWithCallback(Operation op, std::function<void(const Operation&, OpVector&)> callback, std::function<void()> timeoutCallback, std::chrono::milliseconds duration)
{
    auto serialno = m_serialNo++;
    op->setSerialno(serialno);

    auto timer = std::make_unique<boost::asio::steady_timer>(m_commSocket.m_io_context);
#if BOOST_VERSION >= 106600
    timer->expires_after(duration);
#else
    timer->expires_from_now(duration);
#endif
    timer->async_wait([&, serialno](boost::system::error_code ec) {
        if (!ec) {
            log(WARNING, String::compose("Timeout on operation with serial no %1.", serialno));
            auto I = m_callbacks.find(serialno);
            if (I != m_callbacks.end()) {
                if (I->second.timeoutCallback) {
                    I->second.timeoutCallback();
                }
                m_callbacks.erase(I);
            }
        }
    });

    CallbackEntry entry{
        std::move(callback),
        std::move(timer),
        std::move(timeoutCallback)
    };

    m_callbacks.emplace(serialno, std::move(entry));
    send(op);

}

