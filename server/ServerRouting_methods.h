// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_SERVER_ROUTING_METHODS_H
#define SERVER_SERVER_ROUTING_METHODS_H

#include "ServerRouting.h"
#include "common/BaseWorld.h"

#if !defined(__GNUC__) || __GNUC__ > 2 || (__GNUC__ > 1 && __GNUC_MINOR__ > 95)

inline void ServerRouting::idle() {
    m_world.idle();
}

#endif // defined(__GNUC__) .....

#endif // SERVER_SERVER_ROUTING_METHODS_H
