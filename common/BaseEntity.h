// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef BASE_ENTITY_H
#define BASE_ENTITY_H

#include <Atlas/Objects/Operation/Error.h>

#include <list>
#include <vector>

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
	OP_GET,
	OP_SIGHT,
	OP_SOUND,
	OP_TOUCH,
	OP_TALK,
	OP_TICK,
	OP_LOOK,
	OP_LOAD,
	OP_SAVE,
	OP_SETUP,
	OP_APPEARANCE,
	OP_DISAPPEARANCE,
	OP_INVALID
} op_no_t;

#define OP_SWITCH(_op, _op_no, _prefix) \
    switch (_op_no) { \
        case OP_LOGIN: \
            return _prefix ## Operation((const Login &)_op); \
            break; \
        case OP_CHOP: \
            return _prefix ## Operation((const Chop &)_op); \
            break; \
        case OP_CREATE: \
            return _prefix ## Operation((const Create &)_op); \
            break; \
        case OP_CUT: \
            return _prefix ## Operation((const Cut &)_op); \
            break; \
        case OP_DELETE: \
            return _prefix ## Operation((const Delete &)_op); \
            break; \
        case OP_EAT: \
            return _prefix ## Operation((const Eat &)_op); \
            break; \
        case OP_FIRE: \
            return _prefix ## Operation((const Fire &)_op); \
            break; \
        case OP_MOVE: \
            return _prefix ## Operation((const Move &)_op); \
            break; \
        case OP_NOURISH: \
            return _prefix ## Operation((const Nourish &)_op); \
            break; \
        case OP_SET: \
            return _prefix ## Operation((const Set &)_op); \
            break; \
        case OP_GET: \
            return _prefix ## Operation((const Get &)_op); \
            break; \
        case OP_SIGHT: \
            return _prefix ## Operation((const Sight &)_op); \
            break; \
        case OP_SOUND: \
            return _prefix ## Operation((const Sound &)_op); \
            break; \
        case OP_TALK: \
            return _prefix ## Operation((const Talk &)_op); \
            break; \
        case OP_TOUCH: \
            return _prefix ## Operation((const Touch &)_op); \
            break; \
        case OP_TICK: \
            return _prefix ## Operation((const Tick &)_op); \
            break; \
        case OP_LOOK: \
            return _prefix ## Operation((const Look &)_op); \
            break; \
        case OP_LOAD: \
            return _prefix ## Operation((const Load &)_op); \
            break; \
        case OP_SAVE: \
            return _prefix ## Operation((const Save &)_op); \
            break; \
        case OP_SETUP: \
            return _prefix ## Operation((const Setup &)_op); \
            break; \
        case OP_APPEARANCE: \
            return _prefix ## Operation((const Appearance &)_op); \
            break; \
        case OP_DISAPPEARANCE: \
            return _prefix ## Operation((const Disappearance &)_op); \
            break; \
        case OP_ERROR: \
            return _prefix ## Operation((const Error &)_op); \
            break; \
        default: \
            cout << "nothing doing here {" << _op.GetParents().front().AsString() << "}" << endl; \
            return _prefix ## Operation(_op); \
            break; \
    }

#define SUB_OP_SWITCH(_op, _sub_op_no, _prefix, _sub_op) \
    switch (_sub_op_no) { \
        case OP_LOGIN: \
            return _prefix ## Operation(_op, (Login &)_sub_op); \
            break; \
        case OP_CHOP: \
            return _prefix ## Operation(_op, (Chop &)_sub_op); \
            break; \
        case OP_CREATE: \
            return _prefix ## Operation(_op, (Create &)_sub_op); \
            break; \
        case OP_CUT: \
            return _prefix ## Operation(_op, (Cut &)_sub_op); \
            break; \
        case OP_DELETE: \
            return _prefix ## Operation(_op, (Delete &)_sub_op); \
            break; \
        case OP_EAT: \
            return _prefix ## Operation(_op, (Eat &)_sub_op); \
            break; \
        case OP_FIRE: \
            return _prefix ## Operation(_op, (Fire &)_sub_op); \
            break; \
        case OP_MOVE: \
            return _prefix ## Operation(_op, (Move &)_sub_op); \
            break; \
        case OP_SET: \
            return _prefix ## Operation(_op, (Set &)_sub_op); \
            break; \
        case OP_TALK: \
            return _prefix ## Operation(_op, (Talk &)_sub_op); \
            break; \
        case OP_TOUCH: \
            return _prefix ## Operation(_op, (Touch &)_sub_op); \
            break; \
        default: \
            cout << "nothing doing here {" << _op.GetParents().front().AsString() << "_" << _sub_op.GetParents().front().AsString() << "}" << endl; \
            return _prefix ## Operation(_op, _sub_op); \
            break; \
    }

typedef std::map<string, BaseEntity *> fdict_t;
typedef std::list<BaseEntity *> flist_t;
typedef std::pair<string, BaseEntity *> fidpair_t;

typedef int Entity;

class BaseEntity;
class WorldRouter;

namespace Atlas {
    namespace Objects {
        namespace Operation {
            class Logout;
            class Create;
            class Delete;
            class Move;
            class Set;
            class Get;
            class Sight;
            class Sound;
            class Touch;
            class Talk;
            class Look;
            class Appearance;
            class Disappearance;
            class Error;
            class Load;
            class Save;
            class Setup;
            class Tick;
            class Eat;
            class Nourish;
            class Cut;
            class Chop;
            class Fire;
        }
    }
}

using namespace Atlas;
using namespace Objects;
using namespace Operation;

typedef std::vector<RootOperation *> oplist;

typedef std::list<BaseEntity *> list_t;

class BaseEntity {
  public:
    string fullid;
    string name;
    Location location;
    list_t contains;
    bool deleted;
    bool in_game;
    bool omnipresent;
    WorldRouter * world;

    BaseEntity();
    virtual ~BaseEntity() { }

    Vector3D get_xyz() const;
    virtual void destroy();

    Message::Object asObject() const;
    virtual void addObject(Message::Object *) const;

    virtual oplist external_message(const RootOperation & op);
    virtual oplist message(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);
    virtual oplist external_operation(const RootOperation & op);

    virtual oplist Operation(const Login & op) { oplist res; return(res); }
    virtual oplist Operation(const Logout & op) { oplist res; return(res); }
    virtual oplist Operation(const Chop & op) { oplist res; return(res); }
    virtual oplist Operation(const Create & op) { oplist res; return(res); }
    virtual oplist Operation(const Cut & op) { oplist res; return(res); }
    virtual oplist Operation(const Delete & op) { oplist res; return(res); }
    virtual oplist Operation(const Eat & op) { oplist res; return(res); }
    virtual oplist Operation(const Fire & op) { oplist res; return(res); }
    virtual oplist Operation(const Get & op) { oplist res; return(res); }
    virtual oplist Operation(const Move & op) { oplist res; return(res); }
    virtual oplist Operation(const Nourish & op) { oplist res; return(res); }
    virtual oplist Operation(const Set & op) { oplist res; return(res); }
    virtual oplist Operation(const Sight & op) { oplist res; return(res); }
    virtual oplist Operation(const Sound & op) { oplist res; return(res); }
    virtual oplist Operation(const Talk & op) { oplist res; return(res); }
    virtual oplist Operation(const Touch & op) { oplist res; return(res); }
    virtual oplist Operation(const Tick & op) { oplist res; return(res); }
    virtual oplist Operation(const Look & op);
    virtual oplist Operation(const Load & op) { oplist res; return(res); }
    virtual oplist Operation(const Save & op) { oplist res; return(res); }
    virtual oplist Operation(const Setup & op) { oplist res; return(res); }
    virtual oplist Operation(const Appearance & op) { oplist res; return(res); }
    virtual oplist Operation(const Disappearance & op) { oplist res; return(res); }
    virtual oplist Operation(const RootOperation & op) { oplist res; return(res); }

    void set_refno_op(RootOperation * op, const RootOperation & ref_op) const {
        op->SetRefno(ref_op.GetSerialno());
    }

    void set_refno(oplist & ret, const RootOperation & ref_op) const {
        for(oplist::const_iterator I = ret.begin(); I != ret.end(); I++) {
            set_refno_op(*I, ref_op);
        }
    }

    op_no_t op_enumerate(const RootOperation * op) const {
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
        if ("get" == parent)  { return(OP_GET); }
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
        if ("appearance" == parent)  { return(OP_APPEARANCE); }
        if ("disappearance" == parent)  { return(OP_DISAPPEARANCE); }
        if ("error" == parent)  { return(OP_ERROR); }
        return (OP_INVALID);
    }

    oplist call_operation(const RootOperation & op) {
        const op_no_t op_no = op_enumerate(&op);
        OP_SWITCH(op, op_no,)
    }

    oplist error(const RootOperation & op, const char * errstring) const {
        Error * e = new Error();
        *e = Error::Instantiate();

        cerr << "ERROR generated by " << fullid << " with message:" << endl;
        cerr << " [" << errstring << "]" << endl << flush;
        Object::ListType args;
        Object::MapType errmsg;
        errmsg["message"] = Object(errstring);
        args.push_back(Object(errmsg));
        args.push_back(op.AsObject());

        e->SetArgs(args);
        e->SetRefno(op.GetSerialno());

        return oplist(1,e);
    }
};

inline ostream & operator<<(ostream& s, Location& v)
{
    return s << "{" << v.ref->fullid << "," << v.coords << "," << v.velocity << "}";
}

#endif /* BASE_ENTITY_H */
