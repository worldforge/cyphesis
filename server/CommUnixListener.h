
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_UNIX_LISTENER_H
#define SERVER_COMM_UNIX_LISTENER_H

#include "CommSocket.h"

#include <skstream/skserver_unix.h>

/// \brief Handle the listen socket used to listen for unix socket connections
/// on the local machine.
class CommUnixListener : public CommSocket {
  private:
    /// skstream object which manages the low level unix listen socket.
    unix_socket_server m_unixListener;
    /// Flag to indicate if the socket is bound.
    bool m_bound;
    /// Filesystem path of the unix socket.
    std::string m_path;

    int accept();

  public:
    explicit CommUnixListener(CommServer & svr);
    virtual ~CommUnixListener();

    /// Accessor for the filesystem path of the socket.
    const std::string & getPath() const { return m_path; }

    int setup();

    int getFd() const;
    bool isOpen() const;
    bool eof();
    int read();
    void dispatch();
};

#endif // SERVER_COMM_UNIX_LISTENER_H
