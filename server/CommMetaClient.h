// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#ifndef SERVER_COMM_META_CLIENT_H
#define SERVER_COMM_META_CLIENT_H

#include "CommIdleSocket.h"

#include <string>

extern "C" {
    #include <sys/socket.h>
}

class CommMetaClient : public CommIdleSocket {
  private:
    struct sockaddr_storage meta_sa;
    socklen_t meta_sa_len;
    int metaFd;
    time_t lastTime;

    static const int metaserverPort = 8453;

  public:
    CommMetaClient(CommServer & svr) : CommIdleSocket(svr), lastTime(-1) { }

    virtual ~CommMetaClient();

    void metaserverKeepalive();
    void metaserverReply();
    void metaserverTerminate();

    int getFd() const;
    bool eof();
    bool isOpen() const;
    bool read();

    bool setup(const std::string &);
    void idle(time_t t);
};

#endif // SERVER_COMM_META_CLIENT_H
