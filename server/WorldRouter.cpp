// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>

#include <common/Setup.h>

#include "WorldRouter.h"
#include "ServerRouting.h"

#include <rulesets/Entity.h>
#include <rulesets/World.h>
#include <rulesets/EntityFactory.h>
#include <common/debug.h>
#include <common/const.h>
#include <common/globals.h>

#include <strstream>

extern "C" {
    #include <stdio.h>
}

using Atlas::Message::Object;

static const bool debug_flag = false;

WorldRouter::WorldRouter(ServerRouting & srvr) : server(srvr),
                                                 gameWorld(*new World()),
                                                 nextId(0)
{
    fullid = "world_0";
    initTime = time(NULL) - timeoffset;
    updateTime();
    gameWorld.fullid = fullid;
    gameWorld.world=this;
    server.idDict[fullid]=&gameWorld;
    eobjects[fullid]=&gameWorld;
    perceptives.push_back(&gameWorld);
    objectList.push_back(&gameWorld);
    //WorldTime tmp_date("612-1-1 08:57:00");
}

WorldRouter::~WorldRouter()
{
    delete &gameWorld;
    opqueue::const_iterator I = operationQueue.begin();
    for (; I != operationQueue.end(); I++) {
        delete *I;
    }
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
    double t = realTime;
    t = t + op.GetFutureSeconds();
    op.SetSeconds(t);
    op.SetFutureSeconds(0.0);
    opqueue::iterator I;
    for(I = operationQueue.begin();
        (I != operationQueue.end()) && ((*I)->GetSeconds() <= t) ; I++);
    operationQueue.insert(I, &op);
}

inline RootOperation * WorldRouter::getOperationFromQueue()
{
    std::list<RootOperation *>::iterator I = operationQueue.begin();
    if ((I == operationQueue.end()) || ((*I)->GetSeconds() > realTime)) {
        return NULL;
    }
    debug(cout << "pulled op off queue" << endl << flush;);
    operationQueue.pop_front();
    return *I;
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
    return full_id;
}

Entity * WorldRouter::addObject(Entity * obj)
{
    debug(cout << "WorldRouter::addObject(Entity *)" << endl << flush;);
    if (obj->fullid.empty()) {
        obj->fullid=getId(obj->name);
    }
    server.idDict[obj->fullid]=eobjects[obj->fullid]=obj;
    objectList.push_back(obj);
    if (!obj->location) {
        debug(cout << "set loc " << &gameWorld  << endl << flush;);
        obj->location.ref=&gameWorld;
        obj->location.coords=Vector3D(0,0,0);
        debug(cout << "loc set with ref " << obj->location.ref->fullid << endl << flush;);
    }
    if (obj->location.ref==&gameWorld) {
        debug(cout << "loc is world" << endl << flush;);
        gameWorld.contains.push_back(obj);
        gameWorld.contains.unique();
    }
    debug(cout << "Entity loc " << obj->location << endl << flush;);
    obj->world=this;
    if (obj->omnipresent) {
        omnipresentList.push_back(obj);
    }
    Setup * s = new Setup(Setup::Instantiate());
    s->SetTo(obj->fullid);
    s->SetFutureSeconds(-0.1);
    addOperationToQueue(*s, this);
    return (obj);
}

Entity * WorldRouter::addObject(const string & typestr, const Object & ent,
                                const string & id)
{
    debug(cout << "WorldRouter::addObject(string, ent)" << endl << flush;);
    Entity * obj;
    obj = EntityFactory::instance()->newThing(typestr, ent, this);
    obj->fullid = id;
    return addObject(obj);
}

void WorldRouter::delObject(Entity * obj)
{
    // Remove object from contains of its real ref?
    if (obj->location.ref != NULL) {
        obj->location.ref->contains.remove(obj);
    }
    // Remove object from world just to make sure
    gameWorld.contains.remove(obj);
    omnipresentList.remove(obj);
    perceptives.remove(obj);
    objectList.remove(obj);
    eobjects.erase(obj->fullid);
    server.idDict.erase(obj->fullid);
}

oplist WorldRouter::message(const RootOperation & op)
{
    debug(cout << "FATAL: Wrong type of WorldRouter message function called" << endl << flush;);
    return oplist();
}

oplist WorldRouter::message(RootOperation & op, const Entity * obj)
{
    addOperationToQueue(op, obj);
    return oplist();
}

inline const elist_t& WorldRouter::broadcastList(const RootOperation & op) const
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
    op_no_t op_type = opEnumerate(*op);

    if ((to.size() != 0) && (to!="all")) {
        edict_t::const_iterator I = eobjects.find(to);
        if (I == eobjects.end()) {
            debug(cerr << "WARNING: Op to=\"" << to << "\"" << " does not exist"
                       << endl << flush;);
            return oplist();
        }
        Entity * to_entity = I->second;
        if (to_entity == NULL) {
            cerr << "CRITICAL: Op to=\"" << to << "\"" << " is NULL"
                 << endl << flush;
            return oplist();
        }
        oplist res = to_entity->operation(op_ref);
        for(oplist::const_iterator I = res.begin(); I != res.end(); I++) {
            message(**I, to_entity);
        }
        if ((op_type == OP_DELETE) && (to_entity != &gameWorld)) {
            delObject(to_entity);
            to_entity->destroy();
            to_entity->deleted = true;
            delete to_entity;
            to_entity = NULL;
        }
    } else {
        RootOperation newop = op_ref;
        const elist_t & broadcast = broadcastList(op_ref);
        elist_t::const_iterator I;
        for(I = broadcast.begin(); I != broadcast.end(); I++) {
            if (consts::enable_ranges) {
                const string & from = newop.GetFrom();
                edict_t::const_iterator J = eobjects.find(from);
                if ((from.size() != 0) &&
                    (J != eobjects.end()) &&
                    (!J->second->location.inRange((*I)->location,
                                                       consts::sight_range))) {
                        debug(cout << "Op from " <<from<< " cannot be seen by "
                                   << (*I)->fullid << endl << flush;);
                             
                        continue;
                }
            }
            newop.SetTo((*I)->fullid);
            // FIXME THere must be a more efficient way to deliver, with less
            // chance of a loop.
            operation(&newop);
        }
    }
                
    return oplist();
}

oplist WorldRouter::operation(const RootOperation & op)
{
    return operation(&op);
}

oplist WorldRouter::lookOperation(const Look & op)
{
    debug(cout << "WorldRouter::Operation(Look)" << endl << flush;);
    const string & from = op.GetFrom();
    edict_t::const_iterator J = eobjects.find(from);
    if (J == eobjects.end()) {
        debug(cout << "FATAL: Op has invalid from" << endl << flush;);
    } else {
        debug(cout << "Adding [" << from << "] to perceptives" << endl << flush;);
        perceptives.push_back(J->second);
        perceptives.unique();
        if (consts::enable_ranges) {
            Sight * s = new Sight(Sight::Instantiate());

            Object::MapType omap;
            omap["id"] = fullid;
            Entity * opFrom = J->second;
            const Vector3D & fromLoc = opFrom->getXyz();
            Object::ListType contlist;
            elist_t::const_iterator I;
            for(I = gameWorld.contains.begin(); I != gameWorld.contains.end(); I++) {
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
            return oplist(1,s);
        }
    }
    return BaseEntity::LookOperation(op);
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
        return 0;
    }
    return 1;
}

const double WorldRouter::upTime() const {
    return realTime - timeoffset;
}

