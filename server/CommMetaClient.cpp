// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommMetaClient.h"

#include "protocol_instructions.h"

#include "common/debug.h"
#include "common/log.h"
#include "common/globals.h"

#include <cstring>

#include <iostream>

static const bool debug_flag = false;

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
CommMetaClient::CommMetaClient(CommServer & svr) : CommIdleSocket(svr),
                                                   m_lastTime(-1)
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
    return false;
}

bool CommMetaClient::isOpen() const
{
    return true;
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
    // Establish socket for communication with the metaserver
    bool success = m_clientIos.setTarget(mserver, m_metaserverPort);
    return success ? 0 : -1;
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

    m_clientIos.peek();
    if (m_clientIos.readsome(mesg, MAXLINE) < (std::streamsize)sizeof(command)) {
        log(WARNING, "WARNING: Reply from metaserver too short");
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
    if (t > (m_lastTime + 5 * 60)) {
        m_lastTime = t;
        metaserverKeepalive();
    }
}
