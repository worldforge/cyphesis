// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef SERVER_ROUTING_H
#define SERVER_ROUTING_H

#include <map>

class ServerRouting;
class CommServer;

#include "WorldRouter.h"
#include "Routing.h"

class ServerRouting : public Routing {
    CommServer * comm_server;
    const string svr_name;
  public:
    WorldRouter * world;
    fdict_t id_dict;

    ServerRouting(CommServer * server, const string & name);
    ~ServerRouting() { }

    virtual void addObject(Atlas::Message::Object *) const;

    int idle() {
        return world->idle();
    }
    
    BaseEntity * add_object(BaseEntity * obj) {
        obj = Routing::add_object(obj);
        id_dict[obj->fullid] = obj;
        return obj;
    }
};

#endif /* SERVER_ROUTING_H */
