// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#ifndef SERVER_COMM_TCP_LISTENER_H
#define SERVER_COMM_TCP_LISTENER_H

#include "CommStreamListener.h"

#include <skstream/skserver.h>

/// \brief Handle the internet listen socket used to accept connections from
/// remote hosts.
/// \ingroup ServerSockets
class CommTCPListener : public CommStreamListener<tcp_socket_server> {
  protected:
    virtual int accept();

  public:
    explicit CommTCPListener(CommServer & svr,
                             const boost::shared_ptr<CommClientKit> &);
    virtual ~CommTCPListener();

    int setup(int port);
    int setup(struct addrinfo * address);
};

#endif // SERVER_COMM_TCP_LISTENER_H
