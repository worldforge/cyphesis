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


#ifndef SERVER_COMM_UNIX_LISTENER_H
#define SERVER_COMM_UNIX_LISTENER_H

#include "CommStreamListener.h"

#include <skstream/skserver_unix.h>

class CommClientKit;

/// \brief Handle the listen socket used to listen for unix socket connections
/// on the local machine.
/// \ingroup ServerSockets
class CommUnixListener : public CommStreamListener<unix_socket_server> {
  private:
    /// Filesystem path of the unix socket.
    std::string m_path;

    virtual int accept();

  public:
    explicit CommUnixListener(CommServer & svr,
                              const boost::shared_ptr<CommClientKit> & kit);
    virtual ~CommUnixListener();

    /// Accessor for the filesystem path of the socket.
    const std::string & getPath() const { return m_path; }

    int setup(const std::string & name);
};

#endif // SERVER_COMM_UNIX_LISTENER_H
