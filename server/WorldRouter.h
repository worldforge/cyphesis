#ifndef WORLD_ROUTER_H
#define WORLD_ROUTER_H

#include <fstream.h>

class WorldRouter;
class ServerRouting;
class Thing;

#include "Routing.h"
#include "WorldTime.h"

#include <common/WorldInfo.h>

class WorldRouter : public Routing {
    Thing * illegal_thing;
    double real_time;
    oplist operation_queue;
    list_t objects_list;
    time_t init_time;

    void add_operation_to_queue(RootOperation & op, BaseEntity *);
    RootOperation * get_operation_from_queue();
    string get_id(string & name);
    const list_t & broadcastList(const RootOperation & op);
    oplist operation(const RootOperation * op);
  public:
    ServerRouting * server;
    int next_id;
    list_t perceptives;
    list_t omnipresent_list;

    WorldRouter(ServerRouting * server);
    virtual ~WorldRouter() { }

    int idle();

    Thing * add_object(Thing * obj);
    Thing * add_object(const string &, const Message::Object &);
    void del_object(BaseEntity * obj);
        
    bool is_object_deleted(BaseEntity * obj) {
        return find_object(obj->fullid)->fullid=="illegal";
    }

    void update_time() {
        time_t tmp_time = time(NULL) - init_time;
        world_info::time = double(tmp_time);
        real_time = world_info::time;
    }

    virtual oplist message(RootOperation & msg, BaseEntity * obj);
    virtual oplist message(const RootOperation & msg);
    virtual oplist operation(const RootOperation & op);
    virtual oplist Operation(const Look & op);
};

#endif /* WORLD_ROUTER_H */
