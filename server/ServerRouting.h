// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef SERVER_ROUTING_H
#define SERVER_ROUTING_H

#include <map>

#include "Routing.h"

class WorldRouter;
class CommServer;

class ServerRouting : public Routing {
    CommServer * comm_server;
    const string svr_name;
  public:
    WorldRouter * world;
    fdict_t id_dict;

    ServerRouting(CommServer * server, const string & name);
    ~ServerRouting() { }

    int idle();
    BaseEntity * add_object(BaseEntity * obj);

    virtual void addObject(Atlas::Message::Object *) const;
};

#endif /* SERVER_ROUTING_H */
