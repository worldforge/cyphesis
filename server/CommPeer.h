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


#ifndef SERVER_COMM_PEER_H
#define SERVER_COMM_PEER_H

#include "CommClient.h"

#include <skstream/skstream.h>

#include <sigc++/signal.h>

/// \brief Handle an internet socket connected to a remote peer server.
/// \ingroup ServerSockets
class CommPeer : public CommClient<tcp_socket_stream> {
  public:
    CommPeer(CommServer & svr, const std::string &);
    virtual ~CommPeer();

    bool connect_pending() const {
        return m_clientIos.connect_pending();
    }

    bool eof();

    void idle(time_t t);

    int connect(const std::string &, int);
    int connect(struct addrinfo *);
    void setup(Link *);

    sigc::signal<void> connected;
    sigc::signal<void> failed;
};

#endif // SERVER_COMM_PEER_H
