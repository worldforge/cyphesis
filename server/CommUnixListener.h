
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_UNIX_LISTENER_H
#define SERVER_COMM_UNIX_LISTENER_H

#include "CommSocket.h"

#include <skstream/skserver_unix.h>

class CommUnixListener : public CommSocket {
  private:
    unix_socket_server m_unixListener;
    bool m_bound;
    std::string m_path;

    bool accept();

  public:
    explicit CommUnixListener(CommServer & svr);
    virtual ~CommUnixListener();

    const std::string & getPath() const { return m_path; }

    bool setup();

    int getFd() const;
    bool eof();
    bool isOpen() const;
    bool read();
    void dispatch();
};

#endif // SERVER_COMM_UNIX_LISTENER_H
