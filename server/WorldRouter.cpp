// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>

#include <common/Setup.h>

#include "WorldRouter.h"
#include "ServerRouting.h"

#include <rulesets/Thing.h>
#include <rulesets/EntityFactory.h>
#include <common/debug.h>
#include <common/const.h>

#include <strstream>

extern "C" {
    #include <stdio.h>
}

using Atlas::Message::Object;

static const bool debug_flag = false;

WorldRouter::WorldRouter(ServerRouting & srvr) : server(srvr), nextId(0)
{
    fullid = "world_0";
    initTime = time(NULL);
    updateTime();
    server.idDict[fullid]=this;
    objects[fullid]=this;
    perceptives.push_back(this);
    objectList.push_back(this);
    //WorldTime tmp_date("612-1-1 08:57:00");
    //This structure is used to tell libatlas about stuff
    //world_info.time.s=tmp_date.seconds();
    //world_info.coordinates_conversion_class=Vector3D;
    //world_info.seconds2string=seconds2string;
    //world_info.string2DateTime=WorldTime;
}

inline void WorldRouter::addOperationToQueue(RootOperation & op,
                         const BaseEntity * obj)
{
    if (op.GetFrom() == "cheat") {
        op.SetFrom(op.GetTo());
    } else {
        op.SetFrom(obj->fullid);
    }
    updateTime();
    double t = world_info::time;
    t = t + op.GetFutureSeconds();
    op.SetSeconds(t);
    op.SetFutureSeconds(0.0);
    opqueue::iterator I;
    int i = 0;
    for(I = operationQueue.begin();
        (I != operationQueue.end()) && ((*I)->GetSeconds() <= t) ; I++,i++);
    operationQueue.insert(I, &op);
    debug(cout << i << " operation added to queue" << endl << flush;);
}

inline RootOperation * WorldRouter::getOperationFromQueue()
{
    std::list<RootOperation *>::iterator I = operationQueue.begin();
    if (I == operationQueue.end()) {
        return(NULL);
    }
    if ((*I)->GetSeconds() > realTime) {
        return(NULL);
    }
    debug(cout << "pulled op off queue" << endl << flush;);
    RootOperation * op = (*I);
    operationQueue.pop_front();
    return(op);
}

inline std::string WorldRouter::getId(std::string & name)
{
    std::string full_id;

    std::strstream buf;
    buf << name << "_" << ++nextId;
    full_id = std::string(buf.str(), buf.pcount());
    size_t index;
    while ((index = full_id.find(' ', 0)) != std::string::npos) {
        full_id[index] = '_';
    }
    return(full_id);
}

Thing * WorldRouter::addObject(Thing * obj)
{
    debug(cout << "WorldRouter::addObject(Thing *)" << endl << flush;);
    if (obj->fullid.size() == 0) {
        obj->fullid=getId(obj->name);
    } else {
        cout << "Adding object with known id" << endl << flush;
    }
    server.idDict[obj->fullid]=objects[obj->fullid]=obj;
    objectList.push_back(obj);
    if (!obj->location) {
        debug(cout << "set loc " << this  << endl << flush;);
        obj->location.ref=this;
        obj->location.coords=Vector3D(0,0,0);
        debug(cout << "loc set with ref " << obj->location.ref->fullid << endl << flush;);
    }
    if (obj->location.ref==this) {
        debug(cout << "loc is world" << endl << flush;);
        contains.push_back(obj);
        contains.unique();
    }
    obj->world=this;
    if (obj->omnipresent) {
        omnipresentList.push_back(obj);
    }
    Setup * s = new Setup();
    *s = Setup::Instantiate();
    s->SetTo(obj->fullid);
    s->SetFutureSeconds(-0.1);
    addOperationToQueue(*s, this);
    return (obj);
}

Thing * WorldRouter::addObject(const string & typestr, const Object & ent,
                                const string & id)
{
    debug(cout << "WorldRouter::addObject(string, ent)" << endl << flush;);
    Thing * obj;
    obj = EntityFactory::instance()->newThing(typestr, ent, this);
    obj->fullid = id;
    return addObject(obj);
}

void WorldRouter::delObject(BaseEntity * obj)
{
    // Remove object from contains of its real ref?
    if (obj->location.ref != NULL) {
        obj->location.ref->contains.remove(obj);
    }
    // Remove object from world just to make sure
    contains.remove(obj);
    omnipresentList.remove(obj);
    perceptives.remove(obj);
    objectList.remove(obj);
    objects.erase(obj->fullid);
}

oplist WorldRouter::message(const RootOperation & op)
{
    debug(cout << "FATAL: Wrong type of WorldRouter message function called" << endl << flush;);
    // You may eventually want to remove this as it causes a deliberate segfault
    //return(*(RootOperation **)NULL);
    return oplist();
}

oplist WorldRouter::message(RootOperation & op, const BaseEntity * obj)
{
    addOperationToQueue(op, obj);
    return oplist();
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
    return objectList;
}

oplist WorldRouter::operation(const RootOperation * op)
{
    const RootOperation & op_ref = *op;
    string to = op_ref.GetTo();
    debug(cout << "WorldRouter::operation {" << to << "}" << endl << flush;);
    op_no_t op_type = opEnumerate(op);

    debug(cout << 0 << flush;);
    if ((to.size() != 0) && (to!="all")) {
        if (objects.find(to) == objects.end()) {
            cerr << "CRITICAL: Op to=\"" << to << "\"" << " does not exist"
                 << endl << flush;
            return oplist();
        }
        BaseEntity * toEntity = objects[to];
        if (toEntity == NULL) {
            cerr << "CRITICAL: Op to=\"" << to << "\"" << " is NULL"
                 << endl << flush;
            return oplist();
        }
        if ((to != fullid) || (op_type == OP_LOOK)) {
            oplist res;
            if (to == fullid) {
                res = ((BaseEntity *)this)->Operation((Look &)op_ref);
            } else {
                res = toEntity->operation(op_ref);
            }
            for(oplist::const_iterator I = res.begin(); I != res.end(); I++) {
                message(**I, toEntity);
            }
            if (op_type == OP_DELETE) {
                delObject(toEntity);
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
            if (consts::enable_ranges) {
                const string & from = newop.GetFrom();
                if ((from.size() != 0) &&
                    (objects.find(from) != objects.end()) &&
                    (!objects[from]->location.inRange((*I)->location,
                                                       consts::sight_range))) {
                        debug(cout << "Op from " <<from<< " cannot be seen by "
                                   << (*I)->fullid << endl << flush;);
                             
                        continue;
                }
            }
            newop.SetTo((*I)->fullid);
            operation(&newop);
        }
    }
                
    return oplist();
}

oplist WorldRouter::operation(const RootOperation & op)
{
    return(operation(&op));
}

oplist WorldRouter::Operation(const Look & op)
{
    debug(cout << "WorldRouter::Operation(Look)" << endl << flush;);
    string from = op.GetFrom();
    if (objects.find(from) == objects.end()) {
        debug(cout << "FATAL: Op has invalid from" << endl << flush;);
        //return(*(RootOperation **)NULL);
    } else {
        debug(cout << "Adding [" << from << "] to perceptives" << endl << flush;);
        perceptives.push_back(objects[from]);
        perceptives.unique();
        if (consts::enable_ranges) {
            Sight * s = new Sight();
            *s = Sight::Instantiate();

            Object::MapType omap;
            omap["id"] = fullid;
            BaseEntity * opFrom = objects[from];
            const Vector3D & fromLoc = opFrom->getXyz();
            Object::ListType contlist;
            list_t::const_iterator I;
            for(I = contains.begin(); I != contains.end(); I++) {
                if ((*I)->location.inRange(fromLoc, consts::sight_range)) {
                    contlist.push_back(Object((*I)->fullid));
                }
            }
            if (contlist.size() != 0) {
                omap["contains"] = Object(contlist);
            }

            Object::ListType args(1,Object(omap));
            s->SetArgs(args);
            s->SetTo(op.GetFrom());
            return(oplist(1,s));
        }
    }
    return(BaseEntity::Operation(op));
}

int WorldRouter::idle()
{
    updateTime();
    RootOperation * op;
    while ((op = getOperationFromQueue()) != NULL) {
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
