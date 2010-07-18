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

#include "CommPeer.h"

#include "Peer.h"
#include "CommServer.h"

#include "common/globals.h"
#include "common/serialno.h"
#include "common/log.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

INT_OPTION(peer_port_num, 6769, CYPHESIS, "peerport",
           "Network listen port for peer server connections");

/// \brief Constructor remote peer socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommPeer::CommPeer(CommServer & svr) : CommClient(svr), m_login_required(false), m_loggedin(false)
{
    std::cout << "Outgoing peer connection." << std::endl << std::flush;
}

/// \brief Constructor remote peer socket object.
///
/// @param svr Reference to the object that manages all socket communication.
/// @param username Username to login with on peer
/// @param password Password to login with on peer
CommPeer::CommPeer(CommServer & svr, std::string & username, std::string & password)
                   : CommClient(svr),
                     m_username(username),
                     m_password(password), 
                     m_login_required(true),
                     m_loggedin(false)
{
    std::cout << "Outgoing peer connection." << std::endl << std::flush;
}

CommPeer::~CommPeer()
{
    std::cout << "Peer disconnected." << std::endl << std::flush;
}

int CommPeer::connect(const std::string & host)
{
    m_clientIos.open(host, peer_port_num);
    if (m_clientIos.is_open()) {
        return 0;
    }
    return -1;
}

int CommPeer::connect(const std::string & host, int port)
{
    m_clientIos.open(host, port);
    if (m_clientIos.is_open()) {
        return 0;
    }
    return -1;
}

void CommPeer::idle(time_t t)
{
    if (m_negotiate != 0) {
        if ((t - m_connectTime) > 10) {
            log(NOTICE, "Client disconnected because of negotiation timeout.");
            m_clientIos.shutdown();
        }
    }
    if(m_negotiate == 0)
    {
        if(m_login_required && !m_loggedin) {
            Atlas::Objects::Operation::Login l;
            Atlas::Objects::Entity::Anonymous account;
            account->setAttr("username", m_username);
            account->setAttr("password", m_password);
            l->setArgs1(account);
            l->setSerialno(newSerialNo());
            // Send the login op
            send(l);
            m_loggedin = true;
            log(INFO, "Sent login op to peer");
        }
    }
}
