// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2002-2004 Alistair Riddoch
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


#ifndef SERVER_COMM_META_CLIENT_H
#define SERVER_COMM_META_CLIENT_H

#include "metaserverapi/MetaServerPacket.hpp"

#include "common/asio.h"
#include "common/asio.h"
#include "common/asio.h"
#include <boost/asio/steady_timer.hpp>

#include <string>
#include <map>

/// \brief Handle a socket used to communicate with the metaserver.
/// \ingroup ServerSockets
class CommMetaClient {
  private:

    boost::asio::ip::udp::socket mSocket;
    boost::asio::steady_timer mKeepaliveTimer;
    boost::asio::ip::udp::resolver mResolver;
    boost::asio::ip::udp::endpoint mDestination;
    /// \brief True if an endpoint has been resolved.
    bool mHasEndpoint;

    std::array<char, MAX_PACKET_BYTES> mReadBuffer;

    /// The domain of the metaserver to use.
    std::string m_server;

    /// The interval between refreshing handshaking with ms
    int m_heartbeatTime;

    /// Port number used to talk to the metaserver.
    static const int m_metaserverPort = 8453;

    /// List of attributes to register with the metaserver
    std::map<std::string,std::string> m_serverAttributes;

    void keepalive();
    void do_receive();

  public:
    explicit CommMetaClient(boost::asio::io_context& ioService);

    virtual ~CommMetaClient();

    void metaserverKeepalive();
    void metaserverReply(size_t packet_size);
    void metaserverTerminate();
    void metaserverAttribute(const std::string & k, const std::string & v );
    void updateAttributes();
    void sendAllAttributes();

    int setup(const std::string &);
};

#endif // SERVER_COMM_META_CLIENT_H
