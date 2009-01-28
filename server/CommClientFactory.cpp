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

#include "CommClientFactory.h"

#include "CommServer.h"
#include "CommRemoteClient.h"
#include "CommSlaveClient.h"
#include "CommPeer.h"

#include "common/id.h"
#include "common/log.h"
#include "common/system.h"

CommClientKit::~CommClientKit()
{
}

template <class ClientT>
int CommClientFactory<ClientT>::newCommClient(CommServer & svr,
                                              int asockfd,
                                              const std::string & address)
{
    std::string connection_id;
    if (newId(connection_id) < 0) {
        log(ERROR, "Unable to accept connection as no ID available");
        closesocket(asockfd);
        return -1;
    }

    CommClient * newcli = new ClientT(svr, asockfd, address, connection_id);

    newcli->setup();

    // Add this new client to the list.
    svr.addSocket(newcli);
    svr.addIdle(newcli);

    return 0;
}

template class CommClientFactory<CommRemoteClient>;
template class CommClientFactory<CommSlaveClient>;
template class CommClientFactory<CommPeer>;
