// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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

#include "TCPListenFactory.h"

#include "server/CommServer.h"
#include "server/CommTCPListener.h"

#include "common/compose.hpp"
#include "common/log.h"

#include <skstream/skaddress.h>

using String::compose;

using boost::shared_ptr;

int TCPListenFactory::listen(CommServer & commServer, int port,
                             const shared_ptr<CommClientKit> & client_kit)
{
    tcp_address address;
    int count = 0;

    if (address.resolveListener(compose("%1", port)) == 0) {
        tcp_address::const_iterator I = address.begin();
        for (; I != address.end(); ++I) {
            CommTCPListener * listener = new CommTCPListener(commServer,
                  client_kit);
            if (listener->setup(*I) != 0) {
                delete listener;
            } else {
                ++count;
                commServer.addSocket(listener);
            }
        }
    }

    return count > 0 ? 0 : -1;
}
