#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Error.h>

#include "BaseEntity.h"

static int debug_ops = 0;

BaseEntity::BaseEntity() : stamp(0.0), deleted(0), in_game(0),
                           omnipresent(0), world(NULL) {
}

void BaseEntity::destroy()
{
    if (deleted != 0) {
        return;
    }
    list_t::const_iterator I;
    for(I = contains.begin(); I != contains.end(); I++) {
        BaseEntity * obj = *I;
        if (obj->deleted == 0) {
            obj->location.parent=location.parent;
            obj->location.coords = location.coords + obj->location.coords;
        }
    }
    if (location) {
        location.parent->contains.remove(this);
    }
}

const Vector3D & BaseEntity::get_xyz()
{
    //Location l=location;
    if (!location) {
        return Vector3D(0.0,0.0,0.0);
    }
    if (location.parent) {
        return location.coords+location.parent->get_xyz();
    } else {
        return location.coords;
    }
}

Object BaseEntity::asObject()
{
    debug_ops && cout << "BaseEntity::asObject" << endl << flush;
    Object::MapType map;
    Object obj(map);
    addObject(&obj);
    return(obj);
}


void BaseEntity::addObject(Object * obj)
{
    debug_ops && cout << "BaseEntity::addObject" << endl << flush;
    Object::MapType & omap = obj->AsMap();
    if (fullid.size() != 0) {
        omap["id"] = fullid;
    }
    Object::ListType contlist;
    list_t::const_iterator I;
    for(I = contains.begin(); I != contains.end(); I++) {
        contlist.push_back(Object((*I)->fullid));
    }
    if (contlist.size() != 0) {
        omap["contains"] = Object(contlist);
    }
}

oplist BaseEntity::external_message(const RootOperation & op)
{
    return message(op);
}

oplist BaseEntity::message(const RootOperation & op)
{
    debug_ops && cout << "BaseEntity::message" << endl << flush;
    return operation(op);
}

oplist BaseEntity::Operation(const Look & op)
{
    debug_ops && cout << "look op got all the way to here" << endl << flush;
    Sight * s = new Sight();
    *s = Sight::Instantiate();
    Object::ListType args(1,asObject());
    s->SetArgs(args);
    s->SetTo(op.GetFrom());

    // Set refno?
    return(oplist(1,s));
    
}

oplist BaseEntity::operation(const RootOperation & op)
{
    debug_ops && cout << "BaseEntity::operation" << endl << flush;
    return call_operation(op);
}

oplist BaseEntity::external_operation(const RootOperation & op)
{
    return operation(op);
}

#if 0
inline void BaseEntity::set_refno_op(RootOperation * op, const RootOperation &ref_op)
{
    op->SetRefno(ref_op.GetSerialno());
}

void BaseEntity::set_refno(oplist ret, const RootOperation & ref_op)
{
    while (ret.size() != 0) {
        RootOperation * br = ret.front();
        set_refno_op(br, ref_op);
        ret.pop_front();
    }
}

op_no_t BaseEntity::op_enumerate(const RootOperation * op)
{
    const Object & parents = op->GetAttr("parents");
    if (!parents.IsList()) {
        debug_ops && cout << "This isn't an operation." << endl << flush;
    }
    if (parents.AsList().size() != 1) {
        debug_ops && cout << "This is a weird operation." << endl << flush;
    }
    if (!parents.AsList().begin()->IsString()) {
        debug_ops && cout << "This op is screwed.\n" << endl << flush;
    }
    string parent(parents.AsList().begin()->AsString());
    debug_ops && cout << "BaseEntity::op_enumarate [" << parent << "]" << endl << flush;
    if ("login" == parent) {
        return(OP_LOGIN);
    }
    if ("chop" == parent) {
        return(OP_CHOP);
    }
    if ("create" == parent) {
        return(OP_CREATE);
    }
    if ("cut" == parent) {
        return(OP_CUT);
    }
    if ("delete" == parent) {
        return(OP_DELETE);
    }
    if ("eat" == parent) {
        return(OP_EAT);
    }
    if ("fire" == parent) {
        return(OP_FIRE);
    }
    if ("move" == parent) {
        return(OP_MOVE);
    }
    if ("set" == parent) {
        return(OP_SET);
    }
    if ("sight" == parent) {
        return(OP_SIGHT);
    }
    if ("sound" == parent) {
        return(OP_SOUND);
    }
    if ("talk" == parent) {
        return(OP_TALK);
    }
    if ("touch" == parent) {
        return(OP_TOUCH);
    }
    if ("tick" == parent) {
        return(OP_TICK);
    }
    if ("look" == parent) {
        return(OP_LOOK);
    }
    if ("load" == parent) {
        return(OP_LOAD);
    }
    if ("save" == parent) {
        return(OP_SAVE);
    }
    if ("setup" == parent) {
        return(OP_SETUP);
    }
    if ("error" == parent) {
        return(OP_ERROR);
    }
    return (OP_INVALID);
}

oplist BaseEntity::call_operation(const RootOperation & op)
{
    debug_ops && cout << "BaseEntity::call_operation" << endl << flush;
    oplist res;
    op_no_t op_no = op_enumerate(&op);
    OP_SWITCH(op, op_no, res,)
    return(res);
}

oplist BaseEntity::error(const RootOperation & op, const char * string)
{
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
#endif
