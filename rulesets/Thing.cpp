// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

#include <common/Setup.h>
#include <common/Tick.h>
#include <common/Chop.h>
#include <common/Cut.h>
#include <common/Eat.h>
#include <common/Nourish.h>
#include <common/Fire.h>

#include "Thing.h"
#include "Script.h"

#include <server/WorldRouter.h>

#include <common/const.h>
#include <common/debug.h>

static const bool debug_flag = false;

using Atlas::Message::Object;

Thing::Thing() : perceptive(false)
{
    inGame = true;
    name = string("Foo");
    attributes["mode"] = Object("birth");
}

Thing::~Thing() { }

oplist Thing::Operation(const Setup & op)
{
    oplist res;
    if (script->Operation("setup", op, res) != 0) {
        return res;
    }
    RootOperation * tick = new Tick(Tick::Instantiate());
    tick->SetTo(fullid);
    return oplist(1,tick);
}

oplist Thing::Operation(const Action & op)
{
    oplist res;
    if (script->Operation("action", op, res) != 0) {
        return res;
    }
    RootOperation * s = new Sight(Sight::Instantiate());
    Object::ListType args(1,op.AsObject());
    s->SetArgs(args);
    return oplist(1,s);
}

oplist Thing::Operation(const Create & op)
{
    oplist res;
    if (script->Operation("create", op, res) != 0) {
        return res;
    }
    const Object::ListType & args=op.GetArgs();
    if (args.size() == 0) {
       return oplist();
    }
    try {
        const Object::MapType & ent = args.front().AsMap();
        Object::MapType::const_iterator I = ent.find("parents");
        if (I == ent.end()) {
            return error(op, "Object to be created has no type");
        }
        const Object::ListType & parents = I->second.AsList();
        string type;
        if (parents.size() < 1) {
            type = "thing";
        } else {
            type = parents.front().AsString();
        }
        debug( cout << fullid << " creating " << type;);
        Thing * obj = world->addObject(type,ent);
        if (!obj->location) {
            obj->location.ref = location.ref;
            obj->location.coords = location.coords;
        }
        if (obj->location.ref != NULL) {
            obj->location.ref->contains.push_back(obj);
            obj->location.ref->contains.unique();
        }
        Create c(op);
        Object::ListType args2(1,obj->asObject());
        c.SetArgs(args2);
        RootOperation * s = new Sight(Sight::Instantiate());
        Object::ListType args3(1,c.AsObject());
        s->SetArgs(args3);
        return oplist(1,s);
    }
    catch (Atlas::Message::WrongTypeException) {
        cerr << "EXCEPTION: Malformed object to be created\n";
        return error(op, "Malformed object to be created\n");
    }
    return oplist();
}

oplist Thing::Operation(const Delete & op)
{
    oplist res;
    if (script->Operation("delete", op, res) != 0) {
        return res;
    }
    // world->delObject(this);
    RootOperation * s = new Sight(Sight::Instantiate());
    Object::ListType args(1,op.AsObject());
    s->SetArgs(args);
    return oplist(1,s);
}

oplist Thing::Operation(const Fire & op)
{
    oplist res;
    if (script->Operation("fire", op, res) != 0) {
        return res;
    }
    Object::MapType::iterator I = attributes.find("burn_speed");
    if (I == attributes.end()) {
        return res;
    }
    const Object & bs = I->second;
    if (!bs.IsNum()) { return res; }
    const Object::MapType & fire_ent = op.GetArgs().front().AsMap();
    double consumed = bs.AsNum() * fire_ent.find("status")->second.AsNum();
    Object::MapType self_ent;
    self_ent["id"] = fullid;
    self_ent["status"] = status - (consumed / weight);

    const string & to = fire_ent.find("id")->second.AsString();
    Object::MapType nour_ent;
    nour_ent["id"] = to;
    nour_ent["weight"] = consumed;

    Set * s = new Set(Set::Instantiate());
    s->SetTo(fullid);
    s->SetArgs(Object::ListType(1,self_ent));

    Nourish * n = new Nourish(Nourish::Instantiate());
    n->SetTo(to);
    n->SetArgs(Object::ListType(1,nour_ent));

    oplist res2(2);
    res2[0] = s;
    res2[1] = n;
    return res2;
}

oplist Thing::Operation(const Move & op)
{
    debug( cout << "Thing::move_operation" << endl << flush;);
    seq++;
    oplist res;
    if (script->Operation("move", op, res) != 0) {
        return res;
    }
    const Object::ListType & args=op.GetArgs();
    if (args.size() == 0) {
        debug( cout << "ERROR: move op has no argument" << endl << flush;);
        return oplist();
    }
    try {
        Vector3D oldpos = location.coords;
        const Object::MapType & ent = args.front().AsMap();
        const string & oname = ent.find("id")->second.AsString();
        debug( cout << "In " << fullid << " got moveop for " << oname << endl << flush;);
        Object::MapType::const_iterator I = ent.find("loc");
        if (I == ent.end()) {
            debug( cout << "ERROR: move op arg has no ref" << endl << flush;);
            return error(op, "Move location has no ref");
        }
        const string & ref = I->second.AsString();
        edict_t::iterator J = world->eobjects.find(ref);
        if (J == world->eobjects.end()) {
            debug( cout << "ERROR: move op arg ref is invalid" << endl << flush;);
            return error(op, "Move location ref invalid");
        }
        debug(cout << "{" << ref << "}" << endl << flush;);
        Entity * newref = J->second;
        if (newref == this) {
            debug( cout << "ERROR: move op arg ref is same as entity" << endl << flush;);
            debug( cout << "ERROR: attempt by entity to move into itself" << endl << flush;);
            return error(op, "Attempt by entity to move into itself");
        }
        if (location.ref != newref) {
            location.ref->contains.remove(this);
            newref->contains.push_back(this);
            location.ref = newref;
        }
        I = ent.find("pos");
        if (I == ent.end()) {
            return error(op, "Move location has no position");
        }

        location.coords = Vector3D(I->second.AsList());
        I = ent.find("velocity");
        if (I != ent.end()) {
            location.velocity = Vector3D(I->second.AsList());
        }

        RootOperation * s = new Sight(Sight::Instantiate());
        Object::ListType args2(1,op.AsObject());
        s->SetArgs(args2);
        oplist res2(1,s);
        // I think it might be wise to send a set indicating we have changed
        // modes, but this would probably be wasteful
        if (consts::enable_ranges && perceptive) {
            debug(cout << "testing range" << endl;);
            elist_t::const_iterator I = location.ref->contains.begin();
            Object::ListType appear, disappear;
            Object::MapType this_ent;
            this_ent["id"] = fullid;
            this_ent["stamp"] = (double)seq;
            for(;I != location.ref->contains.end(); I++) {
                const bool wasInRange = (*I)->location.inRange(oldpos, consts::sight_range);
                const bool isInRange = (*I)->location.inRange(location.coords, consts::sight_range);
                // Build appear and disappear lists, and send operations
                // Also so operations to (dis)appearing perceptive
                // entities saying that we are (dis)appearing
                if (wasInRange ^ isInRange) {
                    Object::MapType that_ent;
                    that_ent["id"] = (*I)->fullid;
                    that_ent["stamp"] = (double)(*I)->seq;
                    if (wasInRange) {
                        // We are losing sight of that object
                        disappear.push_back(that_ent);
                        debug(cout << fullid << ": losing site of " <<(*I)->fullid << endl;);
                        if (((Thing*)*I)->perceptive) {
                            // Send operation to the entity in question so it
                            // knows it is losing sight of us.
                            Disappearance * d = new Disappearance(Disappearance::Instantiate());
                            d->SetArgs(Object::ListType(1,this_ent));
                            d->SetTo((*I)->fullid);
                            res2.push_back(d);
                        }
                    } else /*if (isInRange)*/ {
                        // We are gaining sight of that object
                        appear.push_back(that_ent);
                        debug(cout << fullid << ": gaining site of " <<(*I)->fullid << endl;);
                        if (((Thing*)*I)->perceptive) {
                            // Send operation to the entity in question so it
                            // knows it is gaining sight of us.
                            Appearance * a = new Appearance(Appearance::Instantiate());
                            a->SetArgs(Object::ListType(1,this_ent));
                            a->SetTo((*I)->fullid);
                            res2.push_back(a);
                        }
                    }
                }
            }
            if (disappear.size() != 0) {
                // Send an operation to ourselves with a list of entities
                // we are losing sight of
                Appearance * a = new Appearance(Appearance::Instantiate());
                a->SetArgs(appear);
                a->SetTo(fullid);
                res2.push_back(a);
            }
            if (appear.size() != 0) {
                // Send an operation to ourselves with a list of entities
                // we are gaining sight of
                Disappearance * d = new Disappearance(Disappearance::Instantiate());
                d->SetArgs(disappear);
                d->SetTo(fullid);
                res2.push_back(d);
            }
        }
        return res2;
    }
    catch (Atlas::Message::WrongTypeException) {
        cerr << "EXCEPTION: Malformed object to be moved\n";
        return error(op, "Malformed object to be moved\n");
    }
    return oplist();
}

oplist Thing::Operation(const Set & op)
{
    seq++;
    oplist res;
    if (script->Operation("set", op, res) != 0) {
        return res;
    }
    const Object::ListType & args=op.GetArgs();
    if (args.size() == 0) {
       return oplist();
    }
    try {
        const Object::MapType & ent = args.front().AsMap();
        Object::MapType::const_iterator I;
        for (I = ent.begin(); I != ent.end(); I++) {
            set(I->first, I->second);
        }
        RootOperation * s = new Sight(Sight::Instantiate());
        Object::ListType args2(1,op.AsObject());
        s->SetArgs(args2);
        oplist res2(1,s);
        if (status < 0) {
            RootOperation * d = new Delete(Delete::Instantiate());
            Object::ListType args3(1,this->asObject());
            d->SetArgs(args3);
            d->SetTo(fullid);
            res2.push_back(d);
        }
        return res2;
    }
    catch (Atlas::Message::WrongTypeException) {
        cerr << "EXCEPTION: Malformed set operation\n";
        return error(op, "Malformed set operation\n");
    }
    return oplist();
}

oplist Thing::Operation(const Look & op)
{
    oplist res;
    if (script->Operation("look", op, res) != 0) {
        return res;
    }
    return BaseEntity::Operation(op);
}
