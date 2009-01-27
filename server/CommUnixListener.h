// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003-2004 Alistair Riddoch
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

#ifndef SERVER_COMM_UNIX_LISTENER_H
#define SERVER_COMM_UNIX_LISTENER_H

#include "CommStreamListener.h"

#include <skstream/skserver_unix.h>

/// \brief Handle the listen socket used to listen for unix socket connections
/// on the local machine.
/// \ingroup ServerSockets
class CommUnixListener : public CommStreamListener {
  private:
    /// skstream object which manages the low level unix listen socket.
    unix_socket_server m_unixListener;
    /// Flag to indicate if the socket is bound.
    bool m_bound;
    /// Filesystem path of the unix socket.
    std::string m_path;

    virtual int accept();

    virtual int create(int fd);

  public:
    explicit CommUnixListener(CommServer & svr);
    virtual ~CommUnixListener();

    /// Accessor for the filesystem path of the socket.
    const std::string & getPath() const { return m_path; }

    int setup(const std::string & name);
};

#endif // SERVER_COMM_UNIX_LISTENER_H
