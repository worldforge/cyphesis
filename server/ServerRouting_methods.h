// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_SERVER_ROUTING_METHODS_H
#define SERVER_SERVER_ROUTING_METHODS_H

#include "ServerRouting.h"
#include "WorldRouter.h"

inline int ServerRouting::idle() {
    return world.idle();
}

inline BaseEntity * ServerRouting::addObject(BaseEntity * obj) {
    objects[obj->getId()] = obj;
    return obj;
}


#endif // SERVER_SERVER_ROUTING_METHODS_H
