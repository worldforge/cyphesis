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

#error This file has been removed from the build.
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef SERVER_COMM_PEER_LISTENER_H
#define SERVER_COMM_PEER_LISTENER_H

#include "CommListener.h"

/// \brief Handle the internet listen socket used to accept connections from
/// remote peer servers.
/// \ingroup ServerSockets
class CommPeerListener : public CommListener {
  private:
    virtual int create(int fd, const char * address);

  public:
    explicit CommPeerListener(CommServer & svr);
    virtual ~CommPeerListener();
};

#endif // SERVER_COMM_PEER_LISTENER_H
