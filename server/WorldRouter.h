#ifndef WORLD_ROUTER_H
#define WORLD_ROUTER_H

#include <fstream.h>

class WorldRouter;

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
    ServerRouting * server;
    double real_time;

  public:
    //dict_t objects;
    //fdict_t fobjects;
    list_t perceptives;
    list_t omnipresent_list;
    ofstream queue_fp;

    WorldRouter(ServerRouting * server);
    bad_type get_id(string & name, string & full_id);
    virtual BaseEntity * add_object(BaseEntity * obj);
    virtual BaseEntity * add_object(const string & type, const Message::Object & ent);
    void del_object(BaseEntity * obj);
    bad_type is_object_deleted(BaseEntity *);
    bad_type message(bad_type msg, BaseEntity *);
    BaseEntity * get_operation_place(bad_type op);
    bad_type operation(bad_type op);
    bad_type look_operation(bad_type op);
    bad_type print_queue(bad_type msg);
    bad_type add_operation_to_queue(bad_type op, BaseEntity *);
    bad_type get_operation_from_queue();
    bad_type find_range(BaseEntity *, bad_type attribute, bad_type range, bad_type generate_messages=0);
    bad_type update_all_ranges(BaseEntity *);
    bad_type update_range(BaseEntity *, bad_type attribute, bad_type range, bad_type generate_messages=0);
    bad_type collision(BaseEntity *);
    bad_type execute_code(bad_type code);
    bad_type eval_code(bad_type code);
    bad_type save(bad_type filename);
    bad_type load(bad_type filename);
    bad_type update_time();
    bad_type get_time();
    bad_type idle();
};

#endif /* WORLD_ROUTER_H */
