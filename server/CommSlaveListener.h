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

// $Id: CommSlaveListener.h,v 1.6 2006-12-22 02:14:45 alriddoch Exp $

#ifndef SERVER_COMM_SLAVE_LISTENER_H
#define SERVER_COMM_SLAVE_LISTENER_H

#include "CommUnixListener.h"

/// \brief Handle the unix listen socket used to accept connections from
/// local clients to admin a slave server.
/// \ingroup ServerSockets
class CommSlaveListener : public CommUnixListener {
  private:
    virtual int create(int fd);

  public:
    explicit CommSlaveListener(CommServer & svr);
    virtual ~CommSlaveListener();
};

#endif // SERVER_COMM_SLAVE_LISTENER_H
