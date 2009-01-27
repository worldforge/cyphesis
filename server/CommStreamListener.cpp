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

// $Id$

#include "CommStreamListener.h"

/// \brief Constructor for stream listener socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommStreamListener::CommStreamListener(CommServer & svr) : CommSocket(svr)
{
}

CommStreamListener::~CommStreamListener()
{
}

int CommStreamListener::getFd() const
{
    return m_listener.getSocket();
}

bool CommStreamListener::eof()
{
    return false;
}

bool CommStreamListener::isOpen() const
{
    return m_listener.is_open();
}

int CommStreamListener::read()
{
    accept();
    // Accept errors are not returned, as the listen socket should not
    // be removed.
    return 0;
}

void CommStreamListener::dispatch()
{
}

/// \brief Create and bind the listen socket.
int CommStreamListener::setup(int port)
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
    ::setsockopt(socket, SOL_SOCKET, SO_LINGER, (char *)&listenLinger,
                                                sizeof(listenLinger));
    // Ensure the address can be reused once we are done with it.
    int flag = 1;
    ::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
    return 0;
}
