#ifndef SERVER_ROUTING_H
#define SERVER_ROUTING_H

#include <map>

class ServerRouting;
class CommServer;

#include "WorldRouter.h"
//#include "server.h"
#include "Routing.h"

typedef int bad_type; // Remove this to get unset type reporting

class ServerRouting : public Routing {
    CommServer * comm_server;
    char * svr_name;
  public:
    WorldRouter * world;
    dict_t id_dict;

    ServerRouting(CommServer * server, char * name);

    virtual BaseEntity * add_object(BaseEntity * obj);
    void save(char * filename);
    void load(char * filename);
    int idle();
    
};

#endif /* SERVER_ROUTING_H */
