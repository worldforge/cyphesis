// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "CommRemoteClient.h"

#include "Connection.h"
#include "CommServer.h"

char CommRemoteClient::ipno[255];

CommRemoteClient::CommRemoteClient(CommServer & svr, int fd) :
   CommClient(svr, fd, *new Connection((inet_ntop(AF_INET,
              &((const sockaddr_in&)clientIos.getOutpeer()).sin_addr, ipno, 255)
                                       ? ipno : "UNKNOWN"), *this, svr.server))
{
}


CommRemoteClient::~CommRemoteClient()
{
}
