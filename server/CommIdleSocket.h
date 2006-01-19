// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

#error This file has been removed from the build

#ifndef SERVER_COMM_IDLE_SOCKET_H
#define SERVER_COMM_IDLE_SOCKET_H

#include "CommSocket.h"

#include <time.h>

/// \brief Base class for any socket which needs to be polled regularly.
///
/// This could probably be re-implemented as a pure virtual interface which
/// does not inherit from CommSocket.
class CommIdleSocket : public CommSocket {
  protected:
    explicit CommIdleSocket(CommServer & svr);
  public:
    virtual ~CommIdleSocket();

    /// \brief Perform idle tasks.
    ///
    /// Called from the server's core idle function whenever it is called.
    virtual void idle(time_t t) = 0;
};

#endif // SERVER_COMM_IDLE_SOCKET_H
