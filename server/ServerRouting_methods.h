// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef SERVER_ROUTING_METHODS_H
#define SERVER_ROUTING_METHODS_H

#include "ServerRouting.h"
#include "WorldRouter.h"

inline int ServerRouting::idle() {
    return world->idle();
}

inline BaseEntity * ServerRouting::add_object(BaseEntity * obj) {
    obj = Routing::add_object(obj);
    id_dict[obj->fullid] = obj;
    return obj;
}


#endif // SERVER_ROUTING_METHODS_H
