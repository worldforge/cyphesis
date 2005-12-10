// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "CommMaster.h"

#include "Master.h"
#include "CommServer.h"

#include "common/globals.h"

/// \brief Constructor remote master socket object.
///
/// @param svr Reference to the object that manages all socket communication.
/// @param addr Address of the remote master server.
CommMaster::CommMaster(CommServer & svr, const std::string & addr,
                       const std::string & id) :
            CommClient(svr, *new Master(*this, svr.m_server, id))
{
    std::cout << "Outgoing master connection." << std::endl << std::flush;
}

CommMaster::~CommMaster()
{
}

int CommMaster::connect(const std::string & host)
{
    m_clientIos.open(host, client_port_num);
    if (m_clientIos.is_open()) {
        return 0;
    }
    return -1;
}
