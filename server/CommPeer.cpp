// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "CommPeer.h"

#include "Peer.h"
#include "CommServer.h"

#include "common/globals.h"

/// \brief Constructor remote peer socket object.
///
/// @param svr Reference to the object that manages all socket communication.
/// @param addr Address of the remote peer.
CommPeer::CommPeer(CommServer & svr, const std::string & addr) :
   CommClient(svr, *new Peer(addr, *this, svr.m_server))
{
    std::cout << "Outgoing peer connection." << std::endl << std::flush;
}

/// \brief Constructor remote peer socket object.
///
/// @param svr Reference to the object that manages all socket communication.
/// @param addr Address of the remote peer.
CommPeer::CommPeer(CommServer & svr, int fd, const std::string & addr) :
   CommClient(svr, fd, *new Peer(addr, *this, svr.m_server))
{
    std::cout << "Incoming peer connection." << std::endl << std::flush;
}

CommPeer::~CommPeer()
{
}

int CommPeer::connect(const std::string & host)
{
    m_clientIos.open(host, peer_port_num);
    if (m_clientIos.is_open()) {
        return 0;
    }
    return -1;
}
