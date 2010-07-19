// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#include "Peer.h"

#include "ServerRouting.h"
#include "Lobby.h"

#include "common/id.h"
#include "common/log.h"
#include "common/system.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Entity::Anonymous;

/// \brief Constructor
///
/// @param client the client socket used to connect to the peer.
/// @param svr the server routing object of this server.
/// @param addr a string representation of the address of the peer.
/// @param id a string giving the indentifier of the peer connection.
Peer::Peer(CommClient & client,
           ServerRouting & svr,
           const std::string & addr,
           const std::string & id) :
      Router(id, forceIntegerId(id)), m_commClient(client), m_server(svr)
{
}

Peer::~Peer()
{
}

void Peer::operation(const Operation &op, OpVector &res)
{
    log(INFO, "GOT AN OP!");
    const OpNo op_no = op->getClassNo();
    switch(op_no) {
        case Atlas::Objects::Operation::INFO_NO:
            log(INFO, "Got info op!");
            break;
        case Atlas::Objects::Operation::LOGIN_NO:
            LoginOperation(op, res);
            break;
    }
}

int Peer::verifyCredentials(const Account & account,
                                  const Root & creds) const
{
    Element passwd_attr;
    if (creds->copyAttr("password", passwd_attr) != 0 || !passwd_attr.isString()) {
        return -1;
    }
    const std::string & passwd = passwd_attr.String();

    return check_password(passwd, account.password());
}

void Peer::LoginOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Login has no argument", res);
        return;
    }
    // Account should be the first argument of the op
    const Root & arg = args.front();
    // Check for username, and if its not there, then check for
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
    Account * account = m_server.getAccountByName(username);
    // or if not, from the database
    if (account == 0 || verifyCredentials(*account, arg) != 0) {
        clientError(op, "Login is invalid", res);
        return;
    }
    // Account appears to be who they say they are
    if (account->m_connection) {
        // Internals don't allow player to log in more than once.
        clientError(op, "This account is already logged in", res);
        return;
    }
    // Connect everything up
//    addObject(account);
//    EntityDict::const_iterator J = account->getCharacters().begin();
//    EntityDict::const_iterator Jend = account->getCharacters().end();
//    for (; J != Jend; ++J) {
//        addEntity(J->second);
//    }
    account->m_connection = (Connection *)this;
    m_server.m_lobby.addAccount(account);
    // Let the client know they have logged in
    Info info;
    Anonymous info_arg;
    account->addToEntity(info_arg);
    info->setArgs1(info_arg);
    log(INFO, "Successful login");
    res.push_back(info);

    logEvent(LOGIN, String::compose("%1 %2 - Login account %3",
                                    getId(), account->getId(), username));
}
