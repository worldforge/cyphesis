#ifndef WORLD_ROUTER_H
#define WORLD_ROUTER_H

#include <fstream.h>

class WorldRouter;
class Thing;

#include "Routing.h"
#include "ServerRouting.h"
#include "WorldTime.h"

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

//class WorldException : public Exception {
//
    //public:
//};

//typedef bad_type(WorldRouter::*method_t)(bad_type, BaseEntity *);

class WorldRouter : public Routing {
    Thing * illegal_thing;
    double real_time;
    std::list<RootOperation *> operation_queue;

  public:
    //dict_t objects;
    //fdict_t fobjects;
    ServerRouting * server;
    list_t perceptives;
    list_t omnipresent_list;
    ofstream queue_fp;

    WorldRouter(ServerRouting * server);
    virtual ~WorldRouter() { }

    string get_id(string & name);
    virtual BaseEntity * add_object(BaseEntity * obj);
    virtual BaseEntity * add_object(const string & type, const Message::Object & ent);
    virtual void del_object(BaseEntity * obj);
    bad_type is_object_deleted(BaseEntity *);
    virtual RootOperation * message(RootOperation & msg, BaseEntity * obj);
    virtual RootOperation * message(const RootOperation & msg);
    BaseEntity * get_operation_place(const RootOperation & op);
    virtual RootOperation * operation(const RootOperation * op);
    virtual RootOperation * operation(const RootOperation & op);
    virtual RootOperation * Operation(const Look & op);
    bad_type print_queue(bad_type msg);
    void add_operation_to_queue(RootOperation & op, BaseEntity *);
    RootOperation * get_operation_from_queue();
    bad_type find_range(BaseEntity *, bad_type attribute, bad_type range, bad_type generate_messages=0);
    bad_type update_all_ranges(BaseEntity *);
    bad_type update_range(BaseEntity *, bad_type attribute, bad_type range, bad_type generate_messages=0);
    bad_type collision(BaseEntity *);
    bad_type save(bad_type filename);
    bad_type load(bad_type filename);
    void update_time();
    bad_type get_time();
    int idle();
};

#endif /* WORLD_ROUTER_H */
