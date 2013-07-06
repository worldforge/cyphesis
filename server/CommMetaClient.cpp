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

#include "CommMetaClient.h"

#include "CommServer.h"

#include "common/debug.h"
#include "common/log.h"
#include "common/globals.h"

#include <cstring>

#include <iostream>

/*
 * Imported MetaServer API implementation.
 */
#include "metaserverapi/MetaServerAPI.hpp"

#ifdef HAVE_RES_INIT
#include <resolv.h>
#endif // HAVE_RES_INIT

static const bool debug_flag = false;


/// \brief Constructor for metaserver communication socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommMetaClient::CommMetaClient(CommServer & svr) : Idle(svr), CommSocket(svr),
                                                   m_resolveTime(-1),
                                                   m_lastTime(-1),
                                                   m_heartbeatTime(300),
                                                   m_connected(false),
                                                   m_active(false),
                                                   m_attributes(false)


{
}

CommMetaClient::~CommMetaClient()
{
    metaserverTerminate();
}

int CommMetaClient::getFd() const
{
    return m_clientIos.getSocket();
}

bool CommMetaClient::eof()
{
    return m_clientIos.peek() == std::iostream::traits_type::eof();
}

bool CommMetaClient::isOpen() const
{
    return m_clientIos.getSocket() > -1;
}

int CommMetaClient::read()
{
    metaserverReply();
    return 0;
}

void CommMetaClient::dispatch()
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

    return 0;
}

/// \brief Send a keepalive packet to the metaserver.
///
/// This should be called periodically to send a packet notifying the
/// metaserver that this server is still alive.
void CommMetaClient::metaserverKeepalive()
{

	MetaServerPacket keep;

    keep.setPacketType(NMT_SERVERKEEPALIVE);

    m_clientIos.write(keep.getBuffer().data(), keep.getSize());
    m_clientIos << std::flush;

}

/// \brief Read a reply from the metaserver.
///
/// Read the data sent by the metaserver. If the packet received is
/// a handshake, respond with the required response to verify that
/// we are alive.
void CommMetaClient::metaserverReply()
{
    uint32_t handshake = 0, command = 0;
    std::size_t packet_size;
    std::array<char,MAX_PACKET_BYTES> rawBuf;

    packet_size = m_clientIos.readsome(rawBuf.data(), MAX_PACKET_BYTES);

    if ( packet_size < (std::streamsize)sizeof(command)) {
        log(WARNING, "WARNING: Reply from metaserver too short");
        return;
    }

    MetaServerPacket shake( rawBuf, packet_size );

    if(shake.getPacketType() == NMT_HANDSHAKE )
    {
        handshake = shake.getIntData(4); // we know 4 bytes for type, and 4 for shake
        debug(std::cout << "MetaServer contacted successfully."
                        << std::endl << std::flush;);

        MetaServerPacket servershake;
        servershake.setPacketType(NMT_SERVERSHAKE);
        servershake.addPacketData(handshake);

        m_clientIos.write(servershake.getBuffer().data(), servershake.getSize());
        m_clientIos << std::flush;

        m_active = true;

    }
}

/// \brief Send a terminate packet to the metaserver.
///
/// This should be called to indicate that this server is going down,
/// and should no longer be listed.
void CommMetaClient::metaserverTerminate()
{

    MetaServerPacket term;

    term.setPacketType(NMT_TERMINATE);

    m_clientIos.write(term.getBuffer().data(), term.getSize() );
    m_clientIos << std::flush;

    m_active = false;
}

void CommMetaClient::metaserverAttribute(const std::string& k, const std::string & v )
{
	MetaServerPacket m;
	m.setPacketType(NMT_SERVERATTR);
	m.addPacketData(k.length());
	m.addPacketData(v.length());
	m.addPacketData(k);
	m.addPacketData(v);
	m_clientIos.write(m.getBuffer().data(), m.getSize() );
	m_clientIos << std::flush;
}

void CommMetaClient::disconnect()
{
}

int CommMetaClient::flush()
{
    return 0;
}

void CommMetaClient::idle(time_t t)
{
    if (! m_connected ) {
        // Establish socket for communication with the metaserver
        bool success = m_clientIos.setTarget(m_server, m_metaserverPort);
        if (success) {
            m_connected = true;
            m_commServer.addSocket(this);
        } else {
#ifdef HAVE_RES_INIT
            res_init();
#endif // HAVE_RES_INIT
        }
    } else {

    	/*
    	 * Send keep alive (first or renew, same methodology)
    	 */
    	if ( t>(m_lastTime + m_heartbeatTime))
    	{
    		m_lastTime = t;
    		metaserverKeepalive();

    		if ( m_active )
    		{
        		metaserverAttribute("server_uuid", server_uuid );
        		metaserverAttribute("server_key", server_key );
        		m_attributes=true;
    		}
    	}

    	/*
    	 * This is an edge case to handle when the initial session
    	 * is in the process of being created, but not finalized
    	 * as indicated by m_active and prevent an entire m_heartbeatTime
    	 * interval of missing attributes, since the interval can be
    	 * quite large (300s)
    	 **/
    	if ( !m_attributes && m_active )
    	{
    		metaserverAttribute("server_uuid", server_uuid );
    		metaserverAttribute("server_key", server_key );
    		m_attributes=true;
    	}

    }
}
