// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#ifndef SERVER_COMM_META_CLIENT_H
#define SERVER_COMM_META_CLIENT_H

#include "CommSocket.h"

#include <string>

extern "C" {
    #include <netinet/in.h>
}

class CommMetaClient : public CommSocket {
  private:
    struct sockaddr_in meta_sa;
    int metaFd;

    static const int metaserverPort = 8453;

  public:
    CommMetaClient(CommServer & svr) : CommSocket(svr) { }

    void metaserverKeepalive();
    void metaserverReply();
    void metaserverTerminate();

    int getFd() const;
    bool eof();
    bool isOpen() const;
    bool read();

    bool setup(const std::string &);
};

#endif // SERVER_COMM_META_CLIENT_H
