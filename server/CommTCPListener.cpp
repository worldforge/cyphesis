// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "CommTCPListener.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"
#include "common/system.h"

#include <iostream>

#include <cstring>

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif // HAVE_ARPA_INET_H
#include <unistd.h>
#include <errno.h>

static const bool debug_flag = false;

/// \brief Constructor for stream listener socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommTCPListener::CommTCPListener(CommServer & svr,
                                 const boost::shared_ptr<CommClientKit> & kit) :
                 CommStreamListener<tcp_socket_server>(svr, kit)
{
}

CommTCPListener::~CommTCPListener()
{
}

/// \brief Accept a new connect to the listen socket.
int CommTCPListener::accept()
{
    // Low level socket code to accept a new client connection, and create
    // the associated commclient object.
    struct sockaddr_storage sst;
    SOCKLEN addr_len = sizeof(sst);

    debug(std::cout << "Accepting.." << std::endl << std::flush;);
    int asockfd = ::accept(m_listener.getSocket(),
                           (struct sockaddr *)&sst, &addr_len);

    if (asockfd < 0) {
        log(ERROR,
            String::compose("System error accepting network connection: %1",
                            strerror(errno)));
        return -1;
    }
    debug(std::cout << "Accepted" << std::endl << std::flush;);
    
    void * adr = 0;
    if (sst.ss_family == AF_INET) {
        adr = &((sockaddr_in&)sst).sin_addr;
    } else if (sst.ss_family == AF_INET6) {
        adr = &((sockaddr_in6&)sst).sin6_addr;
    }
    char buf[INET6_ADDRSTRLEN];
    const char * address = 0;
#ifdef HAVE_INET_NTOP
    if (adr != 0) {
        address = ::inet_ntop(sst.ss_family, adr, buf, INET6_ADDRSTRLEN);
    } else {
        log(WARNING, "Unable to determine address type for connection");
    }
#elif HAVE_GETNAMEINFO
    // FIXME Get things using getnameinfo with NI_NUMERICHOST
#endif // HAVE_INET_NTOP
    if (address == 0) {
        log(WARNING, "Unable to determine remote address for connection");
        logSysError(WARNING);
        address = "unknown";
    }
    
    return create(asockfd, address);
}

/// \brief Create and bind the listen socket.
int CommTCPListener::setup(int port)
{
    if (m_listener.open(port) != 0) {
        return -1;
    }
    return 0;
}

int CommTCPListener::setup(struct addrinfo * i)
{
    if (m_listener.open(i) != 0) {
        return -1;
    }
    return 0;
}
