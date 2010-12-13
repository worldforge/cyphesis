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

#include <Atlas/Negotiate.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

/// \brief Constructor remote peer socket object.
///
/// @param svr Reference to the object that manages all socket communication.
/// @param username Username to login with on peer
/// @param password Password to login with on peer
CommPeer::CommPeer(CommServer & svr) : CommClient(svr)
{
}

CommPeer::~CommPeer()
{
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
    m_clientIos.open(host, port, true);
    if (m_clientIos.is_open()) {
        return 0;
    }
    return -1;
}

void CommPeer::setup(Router * connection)
{
    m_connection = connection;

    if (!m_clientIos.connect_pending()) {
        m_negotiate->poll(false);
        m_clientIos << std::flush;
    }
}

bool CommPeer::eof()
{
    if (m_clientIos.connect_pending()) {
        return false;
    } else {
        return CommStreamClient::eof();
    }
}

int CommPeer::read()
{
    if (m_clientIos.connect_pending()) {
        if (m_clientIos.isReady(0)) {
            return 0;
        } else {
            return -1;
        }
    } else {
        Atlas::Negotiate * oneg = m_negotiate;
        int ret = CommClient::read();
        if (oneg != m_negotiate) {
            log(INFO, "Peer notification completed");
        }
        return ret;
    }
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
    } else {
        Peer *peer = dynamic_cast<Peer*>(m_connection);
        if (peer == NULL) {
            log(WARNING, "Casting CommPeer connection to Peer failed");
            return;
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
