// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_SERVER_ROUTING_METHODS_H
#define SERVER_SERVER_ROUTING_METHODS_H

#error This file has been removed from the build.

#include "ServerRouting.h"
#include "common/BaseWorld.h"

/// \brief Called whenever the comms system is idle.
///
/// @return True if it did not have time to do everything, and would like to
/// be called again as soon as possible.
inline bool ServerRouting::idle() {
    return m_world.idle();
}

#endif // SERVER_SERVER_ROUTING_METHODS_H
