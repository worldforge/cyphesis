// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef WORLD_ROUTER_H
#define WORLD_ROUTER_H

#include <fstream.h>
#include <sys/time.h>
#include <unistd.h>

class WorldRouter;
class ServerRouting;
class Thing;

#include "Routing.h"
#include "WorldTime.h"

#include <common/WorldInfo.h>

typedef std::list<RootOperation *> opqueue;

class WorldRouter : public Routing {
    double real_time;
    opqueue operation_queue;
    list_t objects_list;
    time_t init_time;

    void add_operation_to_queue(RootOperation & op, const BaseEntity *);
    RootOperation * get_operation_from_queue();
    string get_id(string & name);
    const list_t & broadcastList(const RootOperation & op) const;
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
        
    bool is_object_deleted(BaseEntity * obj) const {
        return find_object(obj->fullid)->deleted;
    }

    void update_time() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        double tmp_time = (double)(tv.tv_sec - init_time) + (double)tv.tv_usec/1000000;
        world_info::time = double(tmp_time);
        real_time = world_info::time;
    }

    virtual oplist message(RootOperation & op, const BaseEntity * obj);
    virtual oplist message(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);
    virtual oplist Operation(const Look & op);
};

#endif /* WORLD_ROUTER_H */
