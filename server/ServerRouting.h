#ifndef SERVER_ROUTING_H
#define SERVER_ROUTING_H

#include <map>

class ServerRouting;
class CommServer;

#include "WorldRouter.h"
#include "Routing.h"

class ServerRouting : public Routing {
    CommServer * comm_server;
    char * svr_name;
  public:
    WorldRouter * world;
    fdict_t id_dict;

    ServerRouting(CommServer * server, char * name);
    ~ServerRouting() { }

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
