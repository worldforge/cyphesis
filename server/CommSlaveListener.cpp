// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "CommSlaveListener.h"

#include "CommSlaveClient.h"
#include "CommServer.h"

static const bool debug_flag = false;

/// \brief Constructor for listener socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommSlaveListener::CommSlaveListener(CommServer & svr) : CommUnixListener(svr)
{
}

CommSlaveListener::~CommSlaveListener()
{
}

void CommSlaveListener::create(int asockfd)
{
    CommSlaveClient * newpeer = new CommSlaveClient(m_commServer, asockfd);

    newpeer->setup();

    // Add this new peer to the list.
    m_commServer.addSocket(newpeer);
}
