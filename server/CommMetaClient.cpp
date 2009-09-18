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

// $Id$

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

#ifdef HAVE_RES_INIT
#include <resolv.h>
#endif // HAVE_RES_INIT

static const bool debug_flag = false;

static const uint32_t SKEEP_ALIVE = 1;
static const uint32_t CKEEP_ALIVE = 2;
static const uint32_t HANDSHAKE = 3;
static const uint32_t SERVERSHAKE = 4;
static const uint32_t CLIENTSHAKE = 5;
static const uint32_t TERMINATE = 6;
static const uint32_t LIST_REQ = 7;
static const uint32_t LIST_RESP = 8;
static const uint32_t PROTO_ERANGE = 9;

static inline char *pack_uint32(uint32_t data, char *buffer, unsigned int *size)
{
    uint32_t netorder;

    netorder = htonl(data);
    memcpy(buffer, &netorder, sizeof(uint32_t));
    *size += sizeof(uint32_t);
    return buffer+sizeof(uint32_t);
}

static inline char *unpack_uint32(uint32_t *dest, char *buffer)
{
    uint32_t netorder;

    memcpy(&netorder, buffer, sizeof(uint32_t));
    *dest = ntohl(netorder);
    return buffer+sizeof(uint32_t);
}

/// \brief Constructor for metaserver communication socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommMetaClient::CommMetaClient(CommServer & svr) : Idle(svr), CommSocket(svr),
                                                   m_resolveTime(-1),
                                                   m_lastTime(-1),
                                                   m_state(META_INIT)
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
    return 0;
}

static const int MAXLINE = 4096;

/// \brief Send a keepalive packet to the metaserver.
///
/// This should be called periodically to send a packet notifying the
/// metaserver that this server is still alive.
void CommMetaClient::metaserverKeepalive()
{
    char         mesg[MAXLINE];
    unsigned int packet_size = 0;

    pack_uint32(SKEEP_ALIVE, mesg, &packet_size);
    m_clientIos.write(mesg, packet_size);
    m_clientIos << std::flush;
}

/// \brief Read a reply from the metaserver.
///
/// Read the data sent by the metaserver. If the packet received is
/// a handshake, respond with the required response to verify that
/// we are alive.
void CommMetaClient::metaserverReply()
{
    char mesg[MAXLINE];
    char *mesg_ptr;
    uint32_t handshake = 0, command = 0;
    unsigned int packet_size;

    if (m_clientIos.readsome(mesg, MAXLINE) < (std::streamsize)sizeof(command)) {
        log(WARNING, "WARNING: Reply from metaserver too short");
        return;
    }
    mesg_ptr = unpack_uint32(&command, mesg);

    if(command == HANDSHAKE)
    {
        mesg_ptr = unpack_uint32(&handshake, mesg_ptr);
        debug(std::cout << "MetaServer contacted successfully."
                        << std::endl << std::flush;);

        packet_size = 0;
        mesg_ptr = pack_uint32(SERVERSHAKE, mesg, &packet_size);
        mesg_ptr = pack_uint32(handshake, mesg_ptr, &packet_size);

        m_clientIos.write(mesg, packet_size);
        m_clientIos << std::flush;
    }

}

/// \brief Send a terminate packet to the metaserver.
///
/// This should be called to indicate that this server is going down,
/// and should no longer be listed.
void CommMetaClient::metaserverTerminate()
{
    char         mesg[MAXLINE];
    unsigned int packet_size = 0;

    pack_uint32(TERMINATE, mesg, &packet_size);
    m_clientIos.write(mesg, packet_size);
    m_clientIos << std::flush;
}

void CommMetaClient::idle(time_t t)
{
    if (m_state == META_INIT) {
        // Establish socket for communication with the metaserver
        bool success = m_clientIos.setTarget(m_server, m_metaserverPort);
        if (success) {
            m_state = META_RESOLVED;
            m_commServer.addSocket(this);
        } else {
#ifdef HAVE_RES_INIT
            res_init();
#endif // HAVE_RES_INIT
        }
    } else if (m_state == META_RESOLVED) {
        if (t > (m_lastTime + 5 * 60)) {
            m_lastTime = t;
            metaserverKeepalive();
        }
    } else {
        log(ERROR, "Unknown state in metaserver client.");
    }
}
