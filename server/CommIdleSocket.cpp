// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "CommIdleSocket.h"

#include "CommServer.h"

/// \brief Constructor for socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommIdleSocket::CommIdleSocket(CommServer & svr) : CommSocket(svr)
{
}

CommIdleSocket::~CommIdleSocket()
{
    m_commServer.removeIdle(this);
}
