#ifndef BASE_ENTITY_H
#define BASE_ENTITY_H

#include <list>

#include <modules/Location.h>

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

typedef int cid_t;

#define NO_ID 0

//class NoCode : public Exception {
//
    //public:
//};

typedef int Entity;

class BaseEntity;
class WorldRouter;

typedef std::list<BaseEntity *> list_t;

typedef bad_type(WorldRouter::*method_t)(bad_type, BaseEntity *);

bad_type get_dict_func(bad_type dict, bad_type func_str, bad_type func_undefined);

class BaseEntity {
  public:
    cid_t id;
    char * name;
    Location location;
    list_t contains;
    double stamp;
    int deleted;
    bool omnipresent;
    WorldRouter * world;

    BaseEntity::BaseEntity();
    bad_type super();
    bad_type base_init(bad_type kw);
    bad_type destroy();
    bad_type __repr__();
    bad_type get_type();
    Vector3D get_xyz();
    bad_type as_entity();
    bad_type message(bad_type msg, bad_type op_method);
    bad_type external_message(bad_type msg);
    bad_type find_operation(bad_type op_id, char * prefix,bad_type undefined_operation);
    bad_type setup_operation(bad_type op);
    bad_type look_operation(bad_type op);
    bad_type undefined_operation(bad_type op);
    bad_type call_operation(bad_type op);
    bad_type operation(bad_type op);
    bad_type external_operation(bad_type op);
    bad_type apply_to_operation(method_t method , bad_type msg, BaseEntity * obj);
    bad_type set_refno_op(bad_type op, bad_type ref_op);
    bad_type set_refno(bad_type msg, bad_type ref_msg);
    bad_type set_debug_op(bad_type op);
    bad_type set_debug(const char * msg);
    bad_type debug_op(bad_type op, const char * string_message);
    bad_type debug(bad_type msg, const char * string_message);
    bad_type error(bad_type op, const char * string);
};

#endif /* BASE_ENTITY_H */
