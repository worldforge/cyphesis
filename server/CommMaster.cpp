// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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


#include "CommMaster.h"

#include "Master.h"

#include "common/globals.h"

/// \brief Constructor remote master socket object.
///
/// @param svr Reference to the object that manages all socket communication.
/// @param addr Address of the remote master server.
CommMaster::CommMaster() : CommClient()
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
