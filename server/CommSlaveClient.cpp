// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "CommSlaveClient.h"

#include "SlaveClientConnection.h"
#include "CommServer.h"

/// \brief Constructor for local client socket object.
///
/// @param svr Reference to the object that manages all socket communication.
/// @param fd Socket file descriptor
CommSlaveClient::CommSlaveClient(CommServer & svr, int fd) :
                 CommClient(svr, fd, *new SlaveClientConnection("local", *this,
                                                                svr.m_server))
{
}


CommSlaveClient::~CommSlaveClient()
{
}
