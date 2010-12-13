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

#ifndef SERVER_COMM_PEER_H
#define SERVER_COMM_PEER_H

#include "CommClient.h"

/// \brief Handle an internet socket connected to a remote peer server.
/// \ingroup ServerSockets
class CommPeer : public CommClient {
  protected:
    /// \brief The hostname that this peer is connected to
    std::string m_host;
    /// \brief The port that this connection is on
    int m_port;
    /// \brief Serial number for referring to the connection attempt
    long m_ref;

  public:
    CommPeer(CommServer & svr);
    virtual ~CommPeer();

    bool eof();
    int read();

    void idle(time_t t);

    int connect(const std::string &, int, long);
    void setup(Router *);

    /// \brief Get the hostname of the connected peer
    ///
    /// @return The hostname of the connected peer
    const std::string & getHost() {
        return m_host;
    }

    /// \brief Get the port the peer is connected on
    ///
    /// @return The port the peer is connected on
    int getPort() {
        return m_port;
    }
};

#endif // SERVER_COMM_PEER_H
