#ifndef BASE_ENTITY_H
#define BASE_ENTITY_H

#include <list>

#include <modules/Location.h>

typedef int bad_type; // Remove this to get unset type reporting

typedef enum op_no {
	OP_LOGIN,
	OP_LOGOUT,
	OP_CREATE,
	OP_MOVE,
	OP_SET,
	OP_SIGHT,
	OP_SOUND,
	OP_TOUCH,
	OP_LOOK,
	OP_LOAD,
	OP_SAVE,
	OP_SETUP,
	OP_INVALID
} op_no_t;

#define None 0 // Remove this to deal with un-initialied vars

typedef int cid_t;

typedef std::map<cid_t, BaseEntity *> dict_t;
typedef std::pair<cid_t, BaseEntity *> idpair_t;

typedef std::map<string, BaseEntity *> fdict_t;
typedef std::pair<string, BaseEntity *> fidpair_t;

#define NO_ID 0

//class NoCode : public Exception {
//
    //public:
//};

typedef int Entity;

class BaseEntity;
class WorldRouter;

namespace Atlas { namespace Objects { namespace Operation {

class Logout;
class Create;
class Move;
class Set;
class Sight;
class Sound;
class Touch;
class Look;

class Load : public RootOperation { };
class Save : public RootOperation { };
class Setup : public RootOperation { };

} } }

using namespace Atlas;
using namespace Objects;
using namespace Operation;

//typedef Atlas::Objects::Operation::RootOperation Atlas::Objects::Operation::RootOperation;
//typedef Atlas::Objects::Operation::Login Atlas::Objects::Operation::Login;
//typedef Atlas::Objects::Operation::Create Create_op_t;

typedef std::list<BaseEntity *> list_t;

typedef bad_type(WorldRouter::*method_t)(bad_type, BaseEntity *);

bad_type get_dict_func(bad_type dict, bad_type func_str, bad_type func_undefined);

class BaseEntity {
  public:
    cid_t id;
    string fullid;
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

    virtual Message::Object & asObject();
    virtual void addObject(Message::Object *);
    virtual RootOperation * message(const RootOperation & msg);
    virtual RootOperation * Operation(const Login & obj) { return NULL; }
    virtual RootOperation * Operation(const Logout & obj) { return NULL; }
    virtual RootOperation * Operation(const Create & obj) { return NULL; }
    virtual RootOperation * Operation(const Move & obj) { return NULL; }
    virtual RootOperation * Operation(const Set & obj) { return NULL; }
    virtual RootOperation * Operation(const Sight & obj) { return NULL; }
    virtual RootOperation * Operation(const Sound & obj) { return NULL; }
    virtual RootOperation * Operation(const Touch & obj) { return NULL; }
    virtual RootOperation * Operation(const Look & obj);
    virtual RootOperation * Operation(const Load & obj) { return NULL; }
    virtual RootOperation * Operation(const Save & obj) { return NULL; }
    virtual RootOperation * Operation(const Setup & obj) { return NULL; }
    virtual RootOperation * Operation(const RootOperation & obj) { return NULL; }
    bad_type find_operation(bad_type op_id, char * prefix,bad_type undefined_operation);
    bad_type setup_operation(bad_type op);
    bad_type look_operation(bad_type op);
    bad_type undefined_operation(bad_type op);
    bad_type call_operation(bad_type op);
    op_no_t op_enumerate(const RootOperation & op);
    virtual RootOperation * operation(const RootOperation & op);
    virtual RootOperation * external_operation(const RootOperation & op);
    //bad_type external_operation(bad_type op);
    bad_type apply_to_operation(method_t method , bad_type msg, BaseEntity * obj);
    bad_type set_refno_op(bad_type op, bad_type ref_op);
    bad_type set_refno(bad_type msg, bad_type ref_msg);
    bad_type set_debug_op(bad_type op);
    bad_type set_debug(const char * msg);
    bad_type debug_op(bad_type op, const char * string_message);
    bad_type debug(bad_type msg, const char * string_message);
    RootOperation * error(const RootOperation & op, const char * string);
};

#endif /* BASE_ENTITY_H */
