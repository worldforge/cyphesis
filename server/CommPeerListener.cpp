// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommPeerListener.h"

#include "CommPeer.h"
#include "CommServer.h"

static const bool debug_flag = false;

/// \brief Constructor for listener socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommPeerListener::CommPeerListener(CommServer & svr) : CommListener(svr)
{
}

CommPeerListener::~CommPeerListener()
{
}

void CommPeerListener::create(int asockfd, const char * address)
{
    CommPeer * newpeer = new CommPeer(m_commServer, asockfd, address);

    newpeer->setup();

    // Add this new peer to the list.
    m_commServer.addSocket(newpeer);
}
