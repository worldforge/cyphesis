// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

#include <common/Setup.h>
#include <common/Tick.h>
#include <common/Nourish.h>
#include <common/Fire.h>

#include "Thing.h"
#include "Script.h"

#include <common/BaseWorld.h>

#include <common/const.h>
#include <common/debug.h>

static const bool debug_flag = false;

using Atlas::Message::Object;

Thing::Thing() : perceptive(false)
{
}

Thing::~Thing() { }

OpVector Thing::SetupOperation(const Setup & op)
{

    // This is a bit of a hack that I am not entirely happy with.
    // We broadcast a sight of create of ourselves so that everything
    // nearby can see us. This is to get round the fact that the sight
    // of create broadcast by the entity that created us may have
    // been elsewhere on the map.
    RootOperation * sight = new Sight(Sight::Instantiate());
    Create c(Create::Instantiate());
    c.SetArgs(Object::ListType(1,asObject()));
    c.SetTo(getId());
    c.SetFrom(getId());
    sight->SetArgs(Object::ListType(1, c.AsObject()));

    OpVector sres;
    if (script->Operation("setup", op, sres) != 0) {
        sres.push_back(sight);
        return sres;
    }

    OpVector res(2);

    res[0] = sight;

    RootOperation * tick = new Tick(Tick::Instantiate());
    tick->SetTo(getId());

    res[1] = tick;

    return res;
}

OpVector Thing::ActionOperation(const Action & op)
{
    OpVector res;
    if (script->Operation("action", op, res) != 0) {
        return res;
    }
    RootOperation * s = new Sight(Sight::Instantiate());
    s->SetArgs(Object::ListType(1,op.AsObject()));
    return OpVector(1,s);
}

OpVector Thing::CreateOperation(const Create & op)
{
    OpVector res;
    if (script->Operation("create", op, res) != 0) {
        return res;
    }
    const Object::ListType & args=op.GetArgs();
    if (args.empty()) {
       return OpVector();
    }
    try {
        Object::MapType ent = args.front().AsMap();
        Object::MapType::const_iterator I = ent.find("parents");
        if ((I == ent.end()) || !I->second.IsList()) {
            return error(op, "Object to be created has no type");
        }
        const Object::ListType & parents = I->second.AsList();
        I = ent.find("loc");
        if ((I == ent.end()) && (location.ref != NULL)) {
            ent["loc"] = location.ref->getId();
        }
        std::string type;
        if (parents.empty()) {
            type = "thing";
        } else {
            type = parents.front().AsString();
        }
        debug( std::cout << getId() << " creating " << type;);
        Entity * obj = world->addObject(type,ent);
        if (!obj->location) {
            obj->location.ref = location.ref;
            obj->location.coords = location.coords;
        }
        if (obj->location.ref != NULL) {
            obj->location.ref->contains.insert(obj);
        }
        Create c(op);
        c.SetArgs(Object::ListType(1,obj->asObject()));
        RootOperation * s = new Sight(Sight::Instantiate());
        s->SetArgs(Object::ListType(1,c.AsObject()));
        s->SetRefno(op.GetSerialno());
        return OpVector(1,s);
    }
    catch (Atlas::Message::WrongTypeException) {
        std::cerr << "EXCEPTION: Malformed object to be created\n";
        return error(op, "Malformed object to be created\n");
    }
    return OpVector();
}

OpVector Thing::DeleteOperation(const Delete & op)
{
    OpVector res;
    if (script->Operation("delete", op, res) != 0) {
        return res;
    }
    // The actual destruction and removal of this entity will be handled
    // by the WorldRouter
    RootOperation * s = new Sight(Sight::Instantiate());
    s->SetArgs(Object::ListType(1,op.AsObject()));
    return OpVector(1,s);
}

OpVector Thing::FireOperation(const Fire & op)
{
    OpVector res;
    if (script->Operation("fire", op, res) != 0) {
        return res;
    }
    Object::MapType::iterator I = attributes.find("burn_speed");
    if ((I == attributes.end()) || !I->second.IsNum() || op.GetArgs().empty()) {
        return res;
    }
    double bspeed = I->second.AsNum();
    const Object::MapType & fire_ent = op.GetArgs().front().AsMap();
    double consumed = bspeed * fire_ent.find("status")->second.AsNum();
    Object::MapType self_ent;
    self_ent["id"] = getId();
    self_ent["status"] = status - (consumed / weight);

    const std::string & to = fire_ent.find("id")->second.AsString();
    Object::MapType nour_ent;
    nour_ent["id"] = to;
    nour_ent["weight"] = consumed;

    Set * s = new Set(Set::Instantiate());
    s->SetTo(getId());
    s->SetArgs(Object::ListType(1,self_ent));

    Nourish * n = new Nourish(Nourish::Instantiate());
    n->SetTo(to);
    n->SetArgs(Object::ListType(1,nour_ent));

    OpVector res2(2);
    res2[0] = s;
    res2[1] = n;
    return res2;
}

OpVector Thing::MoveOperation(const Move & op)
{
    debug( std::cout << "Thing::move_operation" << std::endl << std::flush;);
    seq++;
    OpVector res;
    if (script->Operation("move", op, res) != 0) {
        return res;
    }
    const Object::ListType & args=op.GetArgs();
    if (args.empty()) {
        debug( std::cout << "ERROR: move op has no argument" << std::endl << std::flush;);
        return OpVector();
    }
    try {
        Vector3D oldpos = location.coords;
        const Object::MapType & ent = args.front().AsMap();
        Object::MapType::const_iterator I = ent.find("loc");
        if ((I == ent.end()) || !I->second.IsString()) {
            return error(op, "Move location has no ref");
        }
        const std::string & ref = I->second.AsString();
        EntityDict::const_iterator J = world->getObjects().find(ref);
        if (J == world->getObjects().end()) {
            return error(op, "Move location ref invalid");
        }
        debug(std::cout << "{" << ref << "}" << std::endl << std::flush;);
        Entity * newref = J->second;
        if (newref == this) {
            return error(op, "Attempt by entity to move into itself");
        }
        if (location.ref != newref) {
            location.ref->contains.erase(this);
            newref->contains.insert(this);
            location.ref = newref;
        }
        I = ent.find("pos");
        if ((I == ent.end()) || !I->second.IsList()) {
            return error(op, "Move location has no position");
        }

        location.coords = Vector3D(I->second.AsList());
        I = ent.find("velocity");
        if (I != ent.end()) {
            location.velocity = Vector3D(I->second.AsList());
        }
        I = ent.find("orientation");
        if (I != ent.end()) {
            location.orientation = Quaternion(I->second.AsList());
        }

        RootOperation * s = new Sight(Sight::Instantiate());
        s->SetArgs(Object::ListType(1,op.AsObject()));
        OpVector res2(1,s);
        // I think it might be wise to send a set indicating we have changed
        // modes, but this would probably be wasteful

        // This code handles sending Appearance and Disappearance operations
        // to this entity and others to indicate if one has gained or lost
        // sight of the other because of this movement
        if (consts::enable_ranges && perceptive) {
            debug(std::cout << "testing range" << std::endl;);
            EntitySet::const_iterator I = location.ref->contains.begin();
            Object::ListType appear, disappear;
            Object::MapType this_ent;
            this_ent["id"] = getId();
            this_ent["stamp"] = (double)seq;
            Object::ListType this_as_args(1,this_ent);
            for(;I != location.ref->contains.end(); I++) {
                const bool wasInRange = (*I)->location.inRange(oldpos, consts::sight_range);
                const bool isInRange = (*I)->location.inRange(location.coords, consts::sight_range);
                // Build appear and disappear lists, and send operations
                // Also so operations to (dis)appearing perceptive
                // entities saying that we are (dis)appearing
                if (wasInRange ^ isInRange) {
                    Object::MapType that_ent;
                    that_ent["id"] = (*I)->getId();
                    that_ent["stamp"] = (double)(*I)->getSeq();
                    if (wasInRange) {
                        // We are losing sight of that object
                        disappear.push_back(that_ent);
                        debug(std::cout << getId() << ": losing site of " <<(*I)->getId() << std::endl;);
                        if (((Thing*)*I)->perceptive) {
                            // Send operation to the entity in question so it
                            // knows it is losing sight of us.
                            Disappearance * d = new Disappearance(Disappearance::Instantiate());
                            d->SetArgs(this_as_args);
                            d->SetTo((*I)->getId());
                            res2.push_back(d);
                        }
                    } else /*if (isInRange)*/ {
                        // We are gaining sight of that object
                        appear.push_back(that_ent);
                        debug(std::cout << getId() << ": gaining site of " <<(*I)->getId() << std::endl;);
                        if (((Thing*)*I)->perceptive) {
                            // Send operation to the entity in question so it
                            // knows it is gaining sight of us.
                            Appearance * a = new Appearance(Appearance::Instantiate());
                            a->SetArgs(this_as_args);
                            a->SetTo((*I)->getId());
                            res2.push_back(a);
                        }
                    }
                }
            }
            if (!appear.empty()) {
                // Send an operation to ourselves with a list of entities
                // we are losing sight of
                Appearance * a = new Appearance(Appearance::Instantiate());
                a->SetArgs(appear);
                a->SetTo(getId());
                res2.push_back(a);
            }
            if (!disappear.empty()) {
                // Send an operation to ourselves with a list of entities
                // we are gaining sight of
                Disappearance * d = new Disappearance(Disappearance::Instantiate());
                d->SetArgs(disappear);
                d->SetTo(getId());
                res2.push_back(d);
            }
        }
        return res2;
    }
    catch (Atlas::Message::WrongTypeException) {
        std::cerr << "EXCEPTION: Malformed object to be moved\n";
        return error(op, "Malformed object to be moved\n");
    }
    return OpVector();
}

OpVector Thing::SetOperation(const Set & op)
{
    seq++;
    OpVector res;
    if (script->Operation("set", op, res) != 0) {
        return res;
    }
    const Object::ListType & args=op.GetArgs();
    if (args.empty()) {
       return OpVector();
    }
    try {
        const Object::MapType & ent = args.front().AsMap();
        Object::MapType::const_iterator I;
        for (I = ent.begin(); I != ent.end(); I++) {
            set(I->first, I->second);
        }
        RootOperation * s = new Sight(Sight::Instantiate());
        s->SetArgs(Object::ListType(1,op.AsObject()));
        OpVector res2(1,s);
        if (status < 0) {
            RootOperation * d = new Delete(Delete::Instantiate());
            d->SetArgs(Object::ListType(1,this->asObject()));
            d->SetTo(getId());
            res2.push_back(d);
        }
        return res2;
    }
    catch (Atlas::Message::WrongTypeException) {
        std::cerr << "EXCEPTION: Malformed set operation\n";
        return error(op, "Malformed set operation\n");
    }
    return OpVector();
}

OpVector Thing::LookOperation(const Look & op)
{
    OpVector res;
    if (script->Operation("look", op, res) != 0) {
        return res;
    }
    return BaseEntity::LookOperation(op);
}
