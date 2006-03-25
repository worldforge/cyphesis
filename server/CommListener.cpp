// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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

#include "CommListener.h"

#include "CommRemoteClient.h"
#include "CommServer.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"

#include <iostream>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <cassert>

static const bool debug_flag = false;

/// \brief Constructor for listener socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommListener::CommListener(CommServer & svr) : CommSocket(svr)
{
}

CommListener::~CommListener()
{
}

int CommListener::getFd() const
{
    return m_listener.getSocket();
}

bool CommListener::eof()
{
    return false;
}
bool CommListener::isOpen() const
{
    return true;
}

int CommListener::read()
{
    accept();
    // FIXME Perhaps we should return the error if accept() doesn't work?
    return 0;
}

void CommListener::dispatch()
{
}

/// \brief Create and bind the listen socket.
int CommListener::setup(int port)
{
    m_listener.open(port);
    if (!m_listener.is_open()) {
        return -1;
    }
    // Set a linger time of 0 seconds, so that the socket is got rid
    // of quickly.
    int socket = m_listener.getSocket();
    struct linger {
        int   l_onoff;
        int   l_linger;
    } listenLinger = { 1, 0 };
    ::setsockopt(socket, SOL_SOCKET, SO_LINGER, &listenLinger,
                                                sizeof(listenLinger));
    // Ensure the address can be reused once we are done with it.
    int flag = 1;
    ::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    return 0;
}

/// \brief Accept a new connect to the listen socket.
int CommListener::accept()
{
    // Low level socket code to accept a new client connection, and create
    // the associated commclient object.
    struct sockaddr_storage sst;
    unsigned int addr_len = sizeof(sst);

    debug(std::cout << "Accepting.." << std::endl << std::flush;);
    int asockfd = ::accept(m_listener.getSocket(),
                           (struct sockaddr *)&sst, &addr_len);

    if (asockfd < 0) {
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
    if (adr != 0) {
        address = ::inet_ntop(sst.ss_family, adr, buf, INET6_ADDRSTRLEN);
    } else {
        log(WARNING, "Unable to determine address type for connection");
    }
    if (address == 0) {
        log(WARNING, "Unable to determine remote address for connection");
        logSysError(WARNING);
        address = "unknown";
    }
    
    create(asockfd, address);
    return 0;
}

void CommListener::create(int asockfd, const char * address)
{
    std::string connection_id;
    newId(connection_id);
    assert(!connection_id.empty());

    CommRemoteClient * newcli = new CommRemoteClient(m_commServer, asockfd,
                                                     address, connection_id);

    newcli->setup();

    // Add this new client to the list.
    m_commServer.addSocket(newcli);
    m_commServer.addIdle(newcli);
}
