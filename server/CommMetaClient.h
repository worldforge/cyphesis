// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#ifndef SERVER_COMM_META_CLIENT_H
#define SERVER_COMM_META_CLIENT_H

#include "CommIdleSocket.h"

#include <string>

#include <skstream/skstream.h>

/// \brief Handle a socket used to communicate with the metaserver.
class CommMetaClient : public CommIdleSocket {
  private:
    /// C++ iostream compatible socket object handling the socket IO.
    udp_socket_stream m_clientIos;
    /// The last time a packet was sent to the metaserver.
    time_t m_lastTime;

    /// Port number used to talk to the metaserver.
    static const int m_metaserverPort = 8453;

  public:
    explicit CommMetaClient(CommServer & svr);

    virtual ~CommMetaClient();

    void metaserverKeepalive();
    void metaserverReply();
    void metaserverTerminate();

    bool setup(const std::string &);

    int getFd() const;
    bool isOpen() const;
    bool eof();
    bool read();
    void dispatch();

    void idle(time_t t);
};

#endif // SERVER_COMM_META_CLIENT_H
