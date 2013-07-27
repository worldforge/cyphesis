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

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"
#include "common/system.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <cstdlib>

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

BaseClient::BaseClient()
{
}

BaseClient::~BaseClient()
{
}

/// \brief Send an operation to the server
void BaseClient::send(const Operation & op)
{
    m_connection.send(op);
}


/// \brief Create a new account on the server
///
/// @param name User name of the new account
/// @param password Password of the new account
Root BaseClient::createSystemAccount()
{
    Anonymous player_ent;
    player_ent->setAttr("username", create_session_username());
    player_ent->setAttr("password", compose("%1%2", ::rand(), ::rand()));
    player_ent->setParents(std::list<std::string>(1, "sys"));
    
    Create createAccountOp;
    createAccountOp->setArgs1(player_ent);
    createAccountOp->setSerialno(m_connection.newSerialNo());
    send(createAccountOp);
    if (m_connection.wait() != 0) {
        std::cerr << "ERROR: Failed to log into server: \""
                  << m_connection.errorMessage() << "\""
                  << std::endl << std::flush;
        return Root(0);
    }

    const Root & ent = m_connection.getInfoReply();

    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "ERROR: Logged in, but account has no id" << std::endl
                  << std::flush;
    } else {
        m_playerId = ent->getId();
    }

    return ent;
}

/// \brief Create a new account on the server
///
/// @param name User name of the new account
/// @param password Password of the new account
Root BaseClient::createAccount(const std::string & name,
                               const std::string & password)
{
    m_playerName = name;

    Anonymous player_ent;
    player_ent->setAttr("username", name);
    player_ent->setAttr("password", password);
    player_ent->setParents(std::list<std::string>(1, "player"));
    
    debug(std::cout << "Loggin " << name << " in with " << password << " as password"
               << std::endl << std::flush;);
    
    Login loginAccountOp;
    loginAccountOp->setArgs1(player_ent);
    loginAccountOp->setSerialno(m_connection.newSerialNo());
    send(loginAccountOp);

    if (m_connection.wait() != 0) {
        Create createAccountOp;
        createAccountOp->setArgs1(player_ent);
        createAccountOp->setSerialno(m_connection.newSerialNo());
        send(createAccountOp);
        if (m_connection.wait() != 0) {
            std::cerr << "ERROR: Failed to log into server" << std::endl
                      << std::flush;
            return Root(0);
        }
    }

    const Root & ent = m_connection.getInfoReply();

    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "ERROR: Logged in, but account has no id" << std::endl
                  << std::flush;
    } else {
        m_playerId = ent->getId();
    }

    return ent;
}



void BaseClient::logout()
{
    Logout logout;
    send(logout);

    if (m_connection.wait() != 0) {
        std::cerr << "ERROR: Failed to logout" << std::endl << std::flush;
    }
}

// I'm making this pure virtual, to see if that is desired.
//void BaseClient::idle() {
//    time.sleep(0.1);
//}

/// \brief Handle any operations that have arrived from the server
void BaseClient::handleNet()
{
    Operation input;
    while ((input = m_connection.pop()).isValid()) {
        OpVector res;
        operation(input, res);
//        m_character->operation(input, res);
        OpVector::const_iterator Iend = res.end();
        for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
            send(*I);
        }
    }
}
