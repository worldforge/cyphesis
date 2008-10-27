// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2002-2004 Alistair Riddoch
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

#ifndef SERVER_COMM_META_CLIENT_H
#define SERVER_COMM_META_CLIENT_H

#include "CommSocket.h"
#include "Idle.h"

#include <string>

#include <skstream/skstream.h>

/// \brief Handle a socket used to communicate with the metaserver.
/// \ingroup ServerSockets
class CommMetaClient : public CommSocket, virtual public Idle {
  private:
    /// C++ iostream compatible socket object handling the socket IO.
    udp_socket_stream m_clientIos;

    /// The domain of the metaserver to use.
    std::string m_server;

    /// The last time the address of the metaserver was successfully resolved
    time_t m_resolveTime;

    /// The last time a packet was sent to the metaserver.
    time_t m_lastTime;

    /// Port number used to talk to the metaserver.
    static const int m_metaserverPort = 8453;

    /// State of the client code.
    enum { META_INIT, META_RESOLVED } m_state;

  public:
    explicit CommMetaClient(CommServer & svr);

    virtual ~CommMetaClient();

    void metaserverKeepalive();
    void metaserverReply();
    void metaserverTerminate();

    int setup(const std::string &);

    int getFd() const;
    bool isOpen() const;
    bool eof();
    int read();
    void dispatch();

    void idle(time_t t);
};

#endif // SERVER_COMM_META_CLIENT_H
