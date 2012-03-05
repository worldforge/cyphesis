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

// $Id$

#include "TrustedConnection.h"

#include "Player.h"
#include "SystemAccount.h"

#include <common/compose.hpp>
#include <common/log.h>

/// \brief TrustedConnection constructor
///
/// @param client The network connection to the client that is trusted
/// @param svr The core server object
/// @param addr A string representation of the client's network address
/// @param id The identifier of this connection.
TrustedConnection::TrustedConnection(CommClient & client,
                                     ServerRouting & svr,
                                     const std::string & addr,
                                     const std::string & id, long iid) :
                                     Connection(client, svr, addr, id, iid)
{
}

Account * TrustedConnection::newAccount(const std::string & type,
                                        const std::string & username,
                                        const std::string & hash,
                                        const std::string & id, long intId)
{
    if (type == "sys") {
        return new SystemAccount(this, username, hash, id, intId);
    } else if (type == "admin") {
        return new Admin(this, username, hash, id, intId);
    } else {
        if (type != "player") {
            log(WARNING, String::compose("Local client tried to create "
                                         "account of unknown type \"%1\". "
                                         "Creating Player.", type));
        }
        return new Player(this, username, hash, id, intId);
    }
}
