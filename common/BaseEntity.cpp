// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

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

#include "common/debug.h"

static const bool debug_flag = false;

BaseEntity::BaseEntity() : deleted(false), in_game(false),
                           omnipresent(false), world(NULL) {
}

void BaseEntity::destroy()
{
    if (deleted == true) {
        return;
    }
    list_t::const_iterator I;
    for(I = contains.begin(); I != contains.end(); I++) {
        BaseEntity * obj = *I;
        if (obj->deleted == false) {
            obj->location.ref = location.ref;
            obj->location.coords = location.coords + obj->location.coords;
        }
    }
    if (location) {
        location.ref->contains.remove(this);
    }
}

Vector3D BaseEntity::get_xyz() const
{
    //Location l=location;
    if (!location) {
        static Vector3D ret(0.0,0.0,0.0);
        return ret;
    }
    if (location.ref) {
        return location.coords+location.ref->get_xyz();
    } else {
        return location.coords;
    }
}

Object BaseEntity::asObject() const
{
    debug( cout << "BaseEntity::asObject" << endl << flush;);
    Object::MapType map;
    map["objtype"] = "object";
    Object obj(map);
    addObject(&obj);
    return(obj);
}


void BaseEntity::addObject(Object * obj) const
{
    debug( cout << "BaseEntity::addObject" << endl << flush;);
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
    debug( cout << "BaseEntity::message" << endl << flush;);
    return operation(op);
}

oplist BaseEntity::Operation(const Look & op)
{
    debug( cout << "look op got all the way to here" << endl << flush;);
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
    debug( cout << "BaseEntity::operation" << endl << flush;);
    return call_operation(op);
}

oplist BaseEntity::external_operation(const RootOperation & op)
{
    return operation(op);
}
