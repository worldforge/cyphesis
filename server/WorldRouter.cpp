// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Look.h>

#include "WorldRouter.h"
#include "ServerRouting.h"

#include <rulesets/Thing.h>
#include <rulesets/ThingFactory.h>
#include <common/debug.h>

extern "C" {
    #include <stdio.h>
}

using Atlas::Message::Object;

static const bool debug_flag = false;
static int halt_time;

WorldRouter::WorldRouter(ServerRouting * srvr) : server(srvr), next_id(0)
{
    fullid = "world_0";
    init_time = time(NULL);
    update_time();
    server->id_dict[fullid]=this;
    fobjects[fullid]=this;
    perceptives.push_back(this);
    objects_list.push_back(this);
    halt_time = 60 * 60 * 8;
    //WorldTime tmp_date("612-1-1 08:57:00");
    //This structure is used to tell libatlas about stuff
    //world_info.time.s=tmp_date.seconds();
    //world_info.coordinates_conversion_class=Vector3D;
    //world_info.seconds2string=seconds2string;
    //world_info.string2DateTime=WorldTime;
}

inline void WorldRouter::add_operation_to_queue(RootOperation & op,
                         const BaseEntity * obj)
{
    if (op.GetFrom() == "cheat") {
        op.SetFrom(op.GetTo());
    } else {
        op.SetFrom(obj->fullid);
    }
    update_time();
    double t = world_info::time;
    //if (t > halt_time) {
        //cout << "Exiting for memory leak report" << endl << flush;
        //exit(0);
    //}
    t = t + op.GetFutureSeconds();
    op.SetSeconds(t);
    op.SetFutureSeconds(0.0);
    std::list<RootOperation *>::iterator I;
    int i = 0;
    for(I = operation_queue.begin();
        (I != operation_queue.end()) && ((*I)->GetSeconds() <= t) ; I++,i++);
    operation_queue.insert(I, &op);
    debug(cout << i << " operation added to queue" << endl << flush;);
}

inline RootOperation * WorldRouter::get_operation_from_queue()
{
    std::list<RootOperation *>::iterator I = operation_queue.begin();
    if (I == operation_queue.end()) {
        return(NULL);
    }
    if ((*I)->GetSeconds() > real_time) {
        return(NULL);
    }
    debug(cout << "pulled op off queue" << endl << flush;);
    RootOperation * op = (*I);
    operation_queue.pop_front();
    return(op);
}

inline string WorldRouter::get_id(string & name)
{
    string full_id;

    char buf[32];
    sprintf(buf, "%d", ++next_id);
    full_id = name + string("_") + string(buf);
    size_t index;
    while ((index = full_id.find(' ', 0)) != string::npos) {
        full_id[index] = '_';
    }
    return(full_id);
}

Thing * WorldRouter::add_object(Thing * obj)
{
    debug(cout << "WorldRouter::add_object(Thing *)" << endl << flush;);
    obj->fullid=get_id(obj->name);
    server->id_dict[obj->fullid]=fobjects[obj->fullid]=obj;
    objects_list.push_back(obj);
    if (!obj->location) {
        debug(cout << "set loc " << this  << endl << flush;);
        obj->location=Location(this, Vector3D(0,0,0));
        debug(cout << "loc set with parent " << obj->location.parent->fullid << endl << flush;);
    }
    if (NULL == obj->location.parent) {
        debug(cout << "set parent" << endl << flush;);
        obj->location.parent=this;
    }
    if (obj->location.parent==this) {
        debug(cout << "loc is world" << endl << flush;);
        contains.push_back(obj);
        contains.unique();
    }
    obj->world=this;
    if (obj->omnipresent) {
        omnipresent_list.push_back(obj);
    }
    Setup * s = new Setup();
    *s = Setup::Instantiate();
    s->SetTo(obj->fullid);
    s->SetFutureSeconds(-0.1);
    add_operation_to_queue(*s, this);
    return (obj);
}

Thing * WorldRouter::add_object(const string & typestr, const Object & ent)
{
    debug(cout << "WorldRouter::add_object(string, ent)" << endl << flush;);
    Thing * obj;
    obj = thing_factory.newThing(typestr, ent, this);
    return add_object(obj);
}

void WorldRouter::del_object(BaseEntity * obj)
{
    // Remove object from contains of its real parent?
    if (obj->location.parent != NULL) {
        obj->location.parent->contains.remove(obj);
    }
    // Remove object from world just to make sure
    contains.remove(obj);
    omnipresent_list.remove(obj);
    perceptives.remove(obj);
    objects_list.remove(obj);
    fobjects.erase(obj->fullid);
}

oplist WorldRouter::message(const RootOperation & op)
{
    debug(cout << "FATAL: Wrong type of WorldRouter message function called" << endl << flush;);
    // You may eventually want to remove this as it causes a deliberate segfault
    //return(*(RootOperation **)NULL);
    oplist res;
    return(res);
}

oplist WorldRouter::message(RootOperation & op, const BaseEntity * obj)
{
    add_operation_to_queue(op, obj);
    oplist res;
    return(res);
}

inline const list_t & WorldRouter::broadcastList(const RootOperation & op) const
{
    const Object::ListType & parents = op.GetParents();
    if ((parents.size() > 0) && (parents.front().IsString())) {
        const string & parent = parents.front().AsString();
        if ((parent == "sight") || (parent == "sound")) {
            return perceptives;
        }
    }
    return objects_list;
}

oplist WorldRouter::operation(const RootOperation * op)
{
    oplist res;
    const RootOperation & op_ref = *op;
    string to = op_ref.GetTo();
    debug(cout << "WorldRouter::operation {" << to << "}" << endl << flush;);
    op_no_t op_type = op_enumerate(op);

    debug(cout << 0 << flush;);
    if ((to.size() != 0) && (to!="all")) {
        if (fobjects.find(to) == fobjects.end()) {
            cerr << "CRITICAL: Op to=\"" << to << "\"" << " does not exist"
                 << endl << flush;
            return res;
        }
        BaseEntity * toEntity = fobjects[to];
        if (toEntity == NULL) {
            cerr << "CRITICAL: Op to=\"" << to << "\"" << " is NULL"
                 << endl << flush;
            return res;
        }
        if ((to != fullid) || (op_type == OP_LOOK)) {
            if (to == fullid) {
                res = ((BaseEntity *)this)->Operation((Look &)op_ref);
            } else {
                res = toEntity->operation(op_ref);
            }
            while (res.size() != 0) {
                RootOperation * ro = res.front();
                message(*ro, toEntity);
                res.pop_front();
            }
            if (op_type == OP_DELETE) {
                toEntity->destroy();
                toEntity->deleted = true;
                delete toEntity;
                toEntity = NULL;
            }
        }
    } else {
        RootOperation newop = op_ref;
        const list_t & broadcast = broadcastList(op_ref);
        std::list<BaseEntity *>::const_iterator I;
        for(I = broadcast.begin(); I != broadcast.end(); I++) {
            newop.SetTo((*I)->fullid);
            operation(&newop);
        }
    }
                
    oplist res2;
    return(res2);
}

oplist WorldRouter::operation(const RootOperation & op)
{
    return(operation(&op));
}

oplist WorldRouter::Operation(const Look & op)
{
    debug(cout << "WorldRouter::Operation(Look)" << endl << flush;);
    string from = op.GetFrom();
    if (fobjects.find(from) == fobjects.end()) {
        debug(cout << "FATAL: Op has invalid from" << endl << flush;);
        //return(*(RootOperation **)NULL);
    } else {
        debug(cout << "Adding [" << from << "] to perceptives" << endl << flush;);
        perceptives.push_back(fobjects[from]);
        perceptives.unique();
    }
    return(BaseEntity::Operation(op));
}

int WorldRouter::idle()
{
    update_time();
    RootOperation * op;
    while ((op = get_operation_from_queue()) != NULL) {
        try {
            operation(op);
        }
        catch (...) {
            cerr << "EXCEPTION: Caught in world.idle()" << endl;
            cerr << "         : Thrown while processing ";
            cerr << op->GetParents().front().AsString();
            cerr << " operation sent to " << op->GetTo();
            cerr << " from " << op->GetFrom() << "." << endl << flush;
        }
        delete op;
    }
    if (op==NULL) {
        return(0);
    }
    return(1);
}
