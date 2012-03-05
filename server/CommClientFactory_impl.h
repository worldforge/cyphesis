// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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
#include "CommClient.h"
#include "ServerRouting.h"

#include "common/id.h"
#include "common/log.h"
#include "common/system.h"

#ifndef SERVER_COMM_CLIENT_FACTORY_IMPL_H
#define SERVER_COMM_CLIENT_FACTORY_IMPL_H

template <class ConnectionT>
int CommClientFactory<ConnectionT>::newCommClient(CommServer & svr,
                                                  int asockfd,
                                                  const std::string & address)
{
    std::string connection_id;
    long c_iid = newId(connection_id);
    if (c_iid < 0) {
        log(ERROR, "Unable to accept connection as no ID available");
        closesocket(asockfd);
        return -1;
    }

    CommClient * newcli = new CommClient(svr,
                                         m_server.getName(),
                                         asockfd);

    newcli->setup(new ConnectionT(*newcli,
                                  m_server,
                                  address,
                                  connection_id, c_iid));

    // Add this new client to the list.
    svr.addSocket(newcli);
    svr.addIdle(newcli);

    return 0;
}

#endif // SERVER_COMM_CLIENT_FACTORY_IMPL_H
