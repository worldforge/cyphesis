#ifndef BASE_ENTITY_H
#define BASE_ENTITY_H

#include <Atlas/Objects/Operation/Error.h>

#include <list>

#include <modules/Location.h>

typedef enum op_no {
	OP_LOGIN,
	OP_LOGOUT,
	OP_CHOP,
	OP_CREATE,
	OP_CUT,
	OP_DELETE,
	OP_EAT,
	OP_ERROR,
	OP_FIRE,
	OP_MOVE,
	OP_NOURISH,
	OP_SET,
	OP_SIGHT,
	OP_SOUND,
	OP_TOUCH,
	OP_TALK,
	OP_TICK,
	OP_LOOK,
	OP_LOAD,
	OP_SAVE,
	OP_SETUP,
	OP_INVALID
} op_no_t;

#define OP_SWITCH(_op, _op_no, _result, _prefix) \
    switch (_op_no) { \
        case OP_LOGIN: \
            _result = _prefix ## Operation((const Login &)_op); \
            break; \
        case OP_CHOP: \
            _result = _prefix ## Operation((const Chop &)_op); \
            break; \
        case OP_CREATE: \
            _result = _prefix ## Operation((const Create &)_op); \
            break; \
        case OP_CUT: \
            _result = _prefix ## Operation((const Cut &)_op); \
            break; \
        case OP_DELETE: \
            _result = _prefix ## Operation((const Delete &)_op); \
            break; \
        case OP_EAT: \
            _result = _prefix ## Operation((const Eat &)_op); \
            break; \
        case OP_FIRE: \
            _result = _prefix ## Operation((const Fire &)_op); \
            break; \
        case OP_MOVE: \
            _result = _prefix ## Operation((const Move &)_op); \
            break; \
        case OP_NOURISH: \
            _result = _prefix ## Operation((const Nourish &)_op); \
            break; \
        case OP_SET: \
            _result = _prefix ## Operation((const Set &)_op); \
            break; \
        case OP_SIGHT: \
            _result = _prefix ## Operation((const Sight &)_op); \
            break; \
        case OP_SOUND: \
            _result = _prefix ## Operation((const Sound &)_op); \
            break; \
        case OP_TALK: \
            _result = _prefix ## Operation((const Talk &)_op); \
            break; \
        case OP_TOUCH: \
            _result = _prefix ## Operation((const Touch &)_op); \
            break; \
        case OP_TICK: \
            _result = _prefix ## Operation((const Tick &)_op); \
            break; \
        case OP_LOOK: \
            _result = _prefix ## Operation((const Look &)_op); \
            break; \
        case OP_LOAD: \
            _result = _prefix ## Operation((const Load &)_op); \
            break; \
        case OP_SAVE: \
            _result = _prefix ## Operation((const Save &)_op); \
            break; \
        case OP_SETUP: \
            _result = _prefix ## Operation((const Setup &)_op); \
            break; \
        case OP_ERROR: \
            _result = _prefix ## Operation((const Error &)_op); \
            break; \
        default: \
            cout << "nothing doing here {" << _op.GetParents().front().AsString() << "}" << endl; \
            _result = _prefix ## Operation(_op); \
            break; \
    }

#define SUB_OP_SWITCH(_op, _sub_op_no, _result, _prefix, _sub_op) \
    switch (_sub_op_no) { \
        case OP_LOGIN: \
            _result = _prefix ## Operation(_op, (Login &)_sub_op); \
            break; \
        case OP_CHOP: \
            _result = _prefix ## Operation(_op, (Chop &)_sub_op); \
            break; \
        case OP_CREATE: \
            _result = _prefix ## Operation(_op, (Create &)_sub_op); \
            break; \
        case OP_CUT: \
            _result = _prefix ## Operation(_op, (Cut &)_sub_op); \
            break; \
        case OP_DELETE: \
            _result = _prefix ## Operation(_op, (Delete &)_sub_op); \
            break; \
        case OP_EAT: \
            _result = _prefix ## Operation(_op, (Eat &)_sub_op); \
            break; \
        case OP_FIRE: \
            _result = _prefix ## Operation(_op, (Fire &)_sub_op); \
            break; \
        case OP_MOVE: \
            _result = _prefix ## Operation(_op, (Move &)_sub_op); \
            break; \
        case OP_SET: \
            _result = _prefix ## Operation(_op, (Set &)_sub_op); \
            break; \
        case OP_TALK: \
            _result = _prefix ## Operation(_op, (Talk &)_sub_op); \
            break; \
        case OP_TOUCH: \
            _result = _prefix ## Operation(_op, (Touch &)_sub_op); \
            break; \
        default: \
            cout << "nothing doing here {" << _op.GetParents().front().AsString() << "_" << _sub_op.GetParents().front().AsString() << "}" << endl; \
            _result = _prefix ## Operation(_op, _sub_op); \
            break; \
    }

typedef std::map<string, BaseEntity *> fdict_t;
typedef std::list<BaseEntity *> flist_t;
typedef std::pair<string, BaseEntity *> fidpair_t;

typedef int Entity;

class BaseEntity;
class WorldRouter;

namespace Atlas { namespace Objects { namespace Operation {

class Logout;
class Create;
class Delete;
class Move;
class Set;
class Sight;
class Sound;
class Touch;
class Talk;
class Look;
class Error;

class Load : public RootOperation {
  public:
    Load() : RootOperation() {
        SetId(string("load"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Load() { }
    static Load Instantiate() {
        Load value;
        Message::Object::ListType parents;
        parents.push_back(string("load"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

class Save : public RootOperation {
  public:
    Save() : RootOperation() {
        SetId(string("save"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Save() { }
    static Save Instantiate() {
        Save value;
        Message::Object::ListType parents;
        parents.push_back(string("save"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

class Setup : public RootOperation {
  public:
    Setup() : RootOperation() {
        SetId(string("setup"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Setup() { }
    static Setup Instantiate() {
        Setup value;
        Message::Object::ListType parents;
        parents.push_back(string("setup"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

class Tick : public RootOperation {
  public:
    Tick() : RootOperation() {
        SetId(string("tick"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Tick() { }
    static Tick Instantiate() {
        Tick value;
        Message::Object::ListType parents;
        parents.push_back(string("tick"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

class Eat : public RootOperation {
  public:
    Eat() : RootOperation() {
        SetId(string("eat"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Eat() { }
    static Eat Instantiate() {
        Eat value;
        Message::Object::ListType parents;
        parents.push_back(string("eat"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

class Nourish : public RootOperation {
  public:
    Nourish() : RootOperation() {
        SetId(string("nourish"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Nourish() { }
    static Nourish Instantiate() {
        Nourish value;
        Message::Object::ListType parents;
        parents.push_back(string("nourish"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

class Cut : public RootOperation {
  public:
    Cut() : RootOperation() {
        SetId(string("cut"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Cut() { }
    static Cut Instantiate() {
        Cut value;
        Message::Object::ListType parents;
        parents.push_back(string("cut"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

class Chop : public RootOperation {
  public:
    Chop() : RootOperation() {
        SetId(string("chop"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Chop() { }
    static Chop Instantiate() {
        Chop value;
        Message::Object::ListType parents;
        parents.push_back(string("chop"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

class Fire : public RootOperation {
  public:
    Fire() : RootOperation() {
        SetId(string("fire"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Fire() { }
    static Fire Instantiate() {
        Fire value;
        Message::Object::ListType parents;
        parents.push_back(string("fire"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

using namespace Atlas;
using namespace Objects;
using namespace Operation;

typedef std::list<RootOperation *> oplist;

typedef std::list<BaseEntity *> list_t;

class BaseEntity {
  public:
    string fullid;
    string name;
    Location location;
    list_t contains;
    double stamp;
    int deleted;
    int in_game;
    bool omnipresent;
    WorldRouter * world;

    BaseEntity();
    virtual ~BaseEntity() { }

    const Vector3D & get_xyz();
    virtual void destroy();

    Message::Object asObject();
    virtual void addObject(Message::Object *);

    virtual oplist external_message(const RootOperation & msg);
    virtual oplist message(const RootOperation & msg);
    virtual oplist operation(const RootOperation & op);
    virtual oplist external_operation(const RootOperation & op);

    virtual oplist Operation(const Login & obj) { oplist res; return(res); }
    virtual oplist Operation(const Logout & obj) { oplist res; return(res); }
    virtual oplist Operation(const Chop & obj) { oplist res; return(res); }
    virtual oplist Operation(const Create & obj) { oplist res; return(res); }
    virtual oplist Operation(const Cut & obj) { oplist res; return(res); }
    virtual oplist Operation(const Delete & obj) { oplist res; return(res); }
    virtual oplist Operation(const Eat & obj) { oplist res; return(res); }
    virtual oplist Operation(const Fire & obj) { oplist res; return(res); }
    virtual oplist Operation(const Move & obj) { oplist res; return(res); }
    virtual oplist Operation(const Nourish & obj) { oplist res; return(res); }
    virtual oplist Operation(const Set & obj) { oplist res; return(res); }
    virtual oplist Operation(const Sight & obj) { oplist res; return(res); }
    virtual oplist Operation(const Sound & obj) { oplist res; return(res); }
    virtual oplist Operation(const Talk & obj) { oplist res; return(res); }
    virtual oplist Operation(const Touch & obj) { oplist res; return(res); }
    virtual oplist Operation(const Tick & obj) { oplist res; return(res); }
    virtual oplist Operation(const Look & obj);
    virtual oplist Operation(const Load & obj) { oplist res; return(res); }
    virtual oplist Operation(const Save & obj) { oplist res; return(res); }
    virtual oplist Operation(const Setup & obj) { oplist res; return(res); }
    virtual oplist Operation(const RootOperation & obj) { oplist res; return(res); }

    void set_refno_op(RootOperation * op, const RootOperation & ref_op) {
        op->SetRefno(ref_op.GetSerialno());
    }

    void set_refno(oplist ret, const RootOperation & ref_op) {
        while (ret.size() != 0) {
            RootOperation * br = ret.front();
            set_refno_op(br, ref_op);
            ret.pop_front();
        }
    }

    op_no_t op_enumerate(const RootOperation * op) {
        const Object::ListType & parents = op->GetParents();
        if (parents.size() != 1) {
            cerr << "This is a weird operation." << endl << flush;
        }
        if (!parents.begin()->IsString()) {
            cerr << "This op has invalid parent.\n" << endl << flush;
        }
        const string & parent = parents.begin()->AsString();
        if ("login" == parent)  { return(OP_LOGIN); }
        if ("chop" == parent)  { return(OP_CHOP); }
        if ("create" == parent)  { return(OP_CREATE); }
        if ("cut" == parent)  { return(OP_CUT); }
        if ("delete" == parent)  { return(OP_DELETE); }
        if ("eat" == parent)  { return(OP_EAT); }
        if ("fire" == parent)  { return(OP_FIRE); }
        if ("move" == parent)  { return(OP_MOVE); }
        if ("nourish" == parent)  { return(OP_NOURISH); }
        if ("set" == parent)  { return(OP_SET); }
        if ("sight" == parent)  { return(OP_SIGHT); }
        if ("sound" == parent)  { return(OP_SOUND); }
        if ("talk" == parent)  { return(OP_TALK); }
        if ("touch" == parent)  { return(OP_TOUCH); }
        if ("tick" == parent)  { return(OP_TICK); }
        if ("look" == parent)  { return(OP_LOOK); }
        if ("load" == parent)  { return(OP_LOAD); }
        if ("save" == parent)  { return(OP_SAVE); }
        if ("setup" == parent)  { return(OP_SETUP); }
        if ("error" == parent)  { return(OP_ERROR); }
        return (OP_INVALID);
    }

    oplist call_operation(const RootOperation & op) {
        oplist res;
        const op_no_t op_no = op_enumerate(&op);
        OP_SWITCH(op, op_no, res,)
        return(res);
    }

    oplist error(const RootOperation & op, const char * string) {
        Error * e = new Error();
        *e = Error::Instantiate();

        list<Object> args;
        Object::MapType errmsg;
        errmsg["message"] = Object(string);
        args.push_back(Object(errmsg));
        args.push_back(op.AsObject());

        e->SetArgs(args);
        e->SetRefno(op.GetSerialno());

        return(oplist(1,e));
    }
};

inline ostream & operator<<(ostream& s, Location& v)
{
    return s << "{" << v.parent->fullid << "," << v.coords << "," << v.velocity << "}";
}

#endif /* BASE_ENTITY_H */
