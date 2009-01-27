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

// $Id$

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CommListener.h"

#include "CommRemoteClient.h"
#include "CommServer.h"

#include "common/id.h"
#include "common/log.h"
#include "common/system.h"

/// \brief Constructor for listener socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommListener::CommListener(CommServer & svr) : CommTCPListener(svr)
{
}

CommListener::~CommListener()
{
}

int CommListener::create(int asockfd, const char * address)
{
    std::string connection_id;
    if (newId(connection_id) < 0) {
        log(ERROR, "Unable to accept connection as no ID available");
        closesocket(asockfd);
        return -1;
    }

    CommRemoteClient * newcli = new CommRemoteClient(m_commServer, asockfd,
                                                     address, connection_id);

    newcli->setup();

    // Add this new client to the list.
    m_commServer.addSocket(newcli);
    m_commServer.addIdle(newcli);

    return 0;
}
