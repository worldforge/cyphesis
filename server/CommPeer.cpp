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
CommPeer::CommPeer(CommServer & svr) : CommClient(svr), 
                                       m_login_required(false)
{
    std::cout << "Outgoing peer connection." << std::endl << std::flush;
}

/// \brief Constructor remote peer socket object.
///
/// @param svr Reference to the object that manages all socket communication.
/// @param username Username to login with on peer
/// @param password Password to login with on peer
CommPeer::CommPeer(CommServer & svr, const std::string & username, const std::string & password)
                   : CommClient(svr),
                     m_login_required(true),
                     m_username(username),
                     m_password(password)
{
    std::cout << "Outgoing peer connection." << std::endl << std::flush;
}

CommPeer::~CommPeer()
{
    std::cout << "Peer disconnected." << std::endl << std::flush;
}

/// \brief Connect to a remote peer
///
/// @param host The hostname of the peer to connect to
/// @return Returns 0 on success and -1 on failure.
int CommPeer::connect(const std::string & host)
{
    m_clientIos.open(host, peer_port_num);
    if (m_clientIos.is_open()) {
        return 0;
    }
    return -1;
}

/// \brief Connect to a remote peer on a specific port
///
/// @param host The hostname of the peer to connect to
/// @param port The port to connect on
/// @return Returns 0 on success and -1 on failure.
int CommPeer::connect(const std::string & host, int port)
{
    m_host = host;
    m_port = port;
    m_clientIos.open(host, port);
    if (m_clientIos.is_open()) {
        return 0;
    }
    return -1;
}

/// \brief Called periodically by the server
///
/// \param t The current time at the time of calling
void CommPeer::idle(time_t t)
{
    // Wait for the negotiation to finish with the peer
    if (m_negotiate != 0) {
        if ((t - m_connectTime) > 10) {
            log(NOTICE, "Client disconnected because of negotiation timeout.");
            m_clientIos.shutdown();
        }
    }
    // As soon as negotiation is complete, authenticate on peer
    if(m_negotiate == 0)
    {
        Peer *peer = dynamic_cast<Peer*>(m_connection);
        if (peer == NULL) {
            log(ERROR, "Casting connection to Peer failed");
            return;
        }
        // If a login is required, send the Login op to the peer
        if(m_login_required && peer->getAuthState() == PEER_INIT) {
            Atlas::Objects::Operation::Login l;
            Atlas::Objects::Entity::Anonymous account;
            account->setAttr("username", m_username);
            account->setAttr("password", m_password);
            l->setArgs1(account);
            l->setSerialno(newSerialNo());
            // Send the login op
            send(l);
            log(INFO, "Sent login op to peer");
            peer->setAuthState(PEER_AUTHENTICATING);
        }
        if (peer->getAuthState() == PEER_FAILED) {
            log(NOTICE, "Peer disconnected because authentication failed.");
            m_clientIos.shutdown();
        }
        // Check if we have been stuck in a state of authentication in-progress
        // for over 20 seconds. If so, disconnect from and remove peer.
        if ((t - m_connectTime) > 20) {
            if (peer->getAuthState() == PEER_AUTHENTICATING) {
                log(NOTICE, "Peer disconnected because authentication timed out.");
                m_clientIos.shutdown();
            }
        }
        if (peer->getAuthState() == PEER_AUTHENTICATED) {
            peer->cleanTeleports();
        }
    }
}
