// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*
 * Imported MetaServer API implementation.
 */
#include "metaserverapi/MetaServerAPI.hpp"

#include "CommMetaClient.h"

#include "common/debug.h"
#include "common/log.h"
#include "common/globals.h"

#include <cstring>
#include <iostream>
#include <memory>



using namespace boost::asio;
static const bool debug_flag = false;


/// \brief Constructor for metaserver communication socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommMetaClient::CommMetaClient(io_service& ioService) :
        mSocket(ioService, ip::udp::endpoint(ip::udp::v4(), 0)), mKeepaliveTimer(
                ioService), mResolver(ioService), m_heartbeatTime(300)
{
}

CommMetaClient::~CommMetaClient()
{
}

/// \brief Set the target address if the communication socket.
///
/// @param mserver String address of the metaserver,
int CommMetaClient::setup(const std::string & mserver)
{
    m_server = mserver;

    /*
     * Iterate over "attributes" section of configuration,
     * and if so, place in m_serverAttributes
     * NOTE: get rid of varconf ... this config is horribly complicated
     * ( and i thought program_options was bad ).
     */
    readConfigItem("attributes","server_uuid", server_uuid );
    readConfigItem("attributes","server_key", server_key );

    /*
     * Get the heartbeat time
     */
    readConfigItem("cyphesis","msheartbeat", m_heartbeatTime);

    /*
     * I know this is a dup storage, but eventually we want it
     * to iterate over all section entries and send them all
     *
     */
    m_serverAttributes["server_uuid"] = server_uuid;
    m_serverAttributes["server_key"] = server_key;

    ip::udp::resolver::query query(ip::udp::v4(), m_server, "8453");
    mResolver.async_resolve(query,
            [this](boost::system::error_code ec, ip::udp::resolver::iterator iterator ) {
                mDestination = *iterator;
                this->metaserverKeepalive();
                this->do_receive();
                this->keepalive();
            });

    return 0;
}

void CommMetaClient::do_receive()
{
    mSocket.async_receive_from(buffer(mReadBuffer, MAX_PACKET_BYTES), mDestination,
            [this](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    this->metaserverReply(length);
                }
                this->do_receive();
            });
}

void CommMetaClient::keepalive()
{

    mKeepaliveTimer.expires_from_now(
            boost::posix_time::seconds(m_heartbeatTime));

    mKeepaliveTimer.async_wait([this](boost::system::error_code ec)
    {
        if (!ec)
        {
            ip::udp::resolver::query query(ip::udp::v4(), m_server, "8453");
            mResolver.async_resolve(query,
                    [this](boost::system::error_code ec, ip::udp::resolver::iterator iterator ) {
                        if (!ec) {
                            mDestination = *iterator;
                            this->metaserverKeepalive();
                        }
                    });
        }
        this->keepalive();
    });
}

/// \brief Send a keepalive packet to the metaserver.
///
/// This should be called periodically to send a packet notifying the
/// metaserver that this server is still alive.
void CommMetaClient::metaserverKeepalive()
{

	auto keep = std::make_shared<MetaServerPacket>();

    keep->setPacketType(NMT_SERVERKEEPALIVE);

    mSocket.async_send_to(buffer(keep->getBuffer(), keep->getSize()),
            mDestination,
            [this, keep](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    metaserverAttribute("server_uuid", server_uuid );
                    metaserverAttribute("server_key", server_key );
                }
            });
}

/// \brief Read a reply from the metaserver.
///
/// Read the data sent by the metaserver. If the packet received is
/// a handshake, respond with the required response to verify that
/// we are alive.
void CommMetaClient::metaserverReply(size_t packet_size)
{
    uint32_t handshake = 0, command = 0;

    if ( packet_size < (std::streamsize)sizeof(command)) {
        log(WARNING, "WARNING: Reply from metaserver too short");
        return;
    }

    MetaServerPacket shake(mReadBuffer, packet_size);

    if(shake.getPacketType() == NMT_HANDSHAKE )
    {
        handshake = shake.getIntData(4); // we know 4 bytes for type, and 4 for shake
        debug(std::cout << "MetaServer contacted successfully."
                        << std::endl << std::flush;);

        auto servershake = std::make_shared<MetaServerPacket>();
        servershake->setPacketType(NMT_SERVERSHAKE);
        servershake->addPacketData(handshake);


        mSocket.async_send_to(buffer(servershake->getBuffer(), servershake->getSize()),mDestination,
                [this, servershake](boost::system::error_code ec, std::size_t length){});


    }
}

/// \brief Send a terminate packet to the metaserver.
///
/// This should be called to indicate that this server is going down,
/// and should no longer be listed.
void CommMetaClient::metaserverTerminate()
{

    mKeepaliveTimer.cancel();
    auto term = std::make_shared<MetaServerPacket>();

    term->setPacketType(NMT_TERMINATE);

    //Do a blocking send as we're calling this when we're shutting down.
    mSocket.send_to(buffer(term->getBuffer(), term->getSize()), mDestination);

}

void CommMetaClient::metaserverAttribute(const std::string& k, const std::string & v )
{
    auto m = std::make_shared<MetaServerPacket>();

    m->setPacketType(NMT_SERVERATTR);
    m->addPacketData(k.length());
    m->addPacketData(v.length());
    m->addPacketData(k);
    m->addPacketData(v);


    mSocket.async_send_to(buffer(m->getBuffer(), m->getSize()), mDestination,
            [this, m](boost::system::error_code ec, std::size_t length){});
}

