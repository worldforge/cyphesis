// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "CommRemoteClient.h"

#include "Connection.h"
#include "CommServer.h"

/// \brief Constructor remote client socket object.
///
/// @param svr Reference to the object that manages all socket communication.
/// @param fd Socket file descriptor
/// @param addr Address of the remote client.
CommRemoteClient::CommRemoteClient(CommServer & svr, int fd,
                                   const std::string & address,
                                   const std::string & id) :
   CommClient(svr, fd, *new Connection(*this, svr.m_server, address, id))
{
}


CommRemoteClient::~CommRemoteClient()
{
}
