
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_LISTENER_H
#define SERVER_COMM_LISTENER_H

#include "CommSocket.h"

class CommListener : public CommSocket {
  private:
    int listenFd;
    int listenPort;

    bool accept();

  public:
    CommListener(CommServer & svr) : CommSocket(svr) { }

    bool setup(int port);

    int getFd() const;
    bool eof();
    bool isOpen() const;
    bool read();
};

#endif // SERVER_COMM_LISTENER_H
