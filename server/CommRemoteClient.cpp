// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "CommRemoteClient.h"

#include "Connection.h"
#include "CommServer.h"

CommRemoteClient::CommRemoteClient(CommServer & svr, int fd, const std::string & addr) :
   CommClient(svr, fd, *new Connection(addr, *this, svr.m_server))
{
}


CommRemoteClient::~CommRemoteClient()
{
}
