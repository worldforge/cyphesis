
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_UNIX_LISTENER_H
#define SERVER_COMM_UNIX_LISTENER_H

#include "CommSocket.h"

class CommUnixListener : public CommSocket {
  private:
    int listenFd;
    bool bound;

    bool accept();

  public:
    CommUnixListener(CommServer & svr) : CommSocket(svr),
                                         listenFd(-1), bound(false) { }
    virtual ~CommUnixListener();

    bool setup();

    int getFd() const;
    bool eof();
    bool isOpen() const;
    bool read();
};

#endif // SERVER_COMM_UNIX_LISTENER_H
