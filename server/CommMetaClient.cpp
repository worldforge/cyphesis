// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommMetaClient.h"

#include "protocol_instructions.h"

#include "common/debug.h"
#include "common/log.h"
#include "common/globals.h"

#include <cstring>
#include <cstdio>

#include <iostream>

extern "C" {
    #include <sys/socket.h>
    #include <netdb.h>
}

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

CommMetaClient::~CommMetaClient()
{
    metaserverTerminate();
}

int CommMetaClient::getFd() const
{
    return metaFd;
}

bool CommMetaClient::eof()
{
    return false;
}
bool CommMetaClient::isOpen() const
{
    return true;
}

bool CommMetaClient::read()
{
    metaserverReply();
    return false;
}

bool CommMetaClient::setup(const std::string & mserver)
{
    // Establish socket for communication with the metaserver
    memset(&meta_sa, 0, sizeof(meta_sa));

    debug(std::cout << "Connecting to metaserver..."
                    << std::endl << std::flush;);
    struct addrinfo req, *ans;

    req.ai_flags = 0;
    req.ai_family = PF_UNSPEC;
    req.ai_socktype = SOCK_DGRAM;
    req.ai_protocol = 0;

    int ret = ::getaddrinfo(mserver.c_str(), NULL, &req, &ans);
    if (ret != 0) {
        log(WARNING, "metaserver lookup failed. Disabling metaserver");
        return false;
    }

    metaFd = ::socket(ans->ai_family, ans->ai_socktype, ans->ai_protocol);
    if (metaFd < 0) {
        ::freeaddrinfo(ans);
        log(WARNING, "Could not get metaserver socket. Disabling metaserver");
        perror("socket");
        return false;
    }
    memcpy(&meta_sa, ans->ai_addr, ans->ai_addrlen);
    meta_sa_len = ans->ai_addrlen;
    ((sockaddr_in *)&meta_sa)->sin_port = htons(metaserverPort);
    ::freeaddrinfo(ans);

    return true;

}

static const int MAXLINE = 4096;

void CommMetaClient::metaserverKeepalive()
{
    char         mesg[MAXLINE];
    unsigned int packet_size = 0;

    pack_uint32(SKEEP_ALIVE, mesg, &packet_size);
    sendto(metaFd,mesg,packet_size,0, (sockaddr *)&meta_sa,  meta_sa_len);
}

void CommMetaClient::metaserverReply()
{
    char mesg[MAXLINE];
    char *mesg_ptr;
    uint32_t handshake = 0, command = 0;
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);
    unsigned int packet_size;

    if (recvfrom(metaFd, mesg, MAXLINE, 0, (sockaddr*)&addr, &addrlen) < 0) {
        log(WARNING, "WARNING: No reply from metaserver");
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

        sendto(metaFd,mesg,packet_size,0,(sockaddr*)&meta_sa, meta_sa_len);
    }

}

void CommMetaClient::metaserverTerminate()
{
    char         mesg[MAXLINE];
    unsigned int packet_size = 0;

    pack_uint32(TERMINATE, mesg, &packet_size);
    sendto(metaFd,mesg,packet_size, 0, (sockaddr *)&meta_sa,  meta_sa_len);
}

void CommMetaClient::idle(time_t t)
{
    if (t > (lastTime + 5 * 60)) {
        lastTime = t;
        metaserverKeepalive();
    }
}
