// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "WorldRouter.h"

#include "ServerRouting.h"
#include "EntityFactory.h"

#include <rulesets/World.h>

#include <common/log.h>
#include <common/debug.h>
#include <common/const.h>
#include <common/globals.h>
#include <common/stringstream.h>
#include <common/BaseWorld.h>
#include <common/globals.h>

#include <common/Setup.h>

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>

static const bool debug_flag = false;

inline void WorldRouter::updateTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double tmp_time = (double)(tv.tv_sec - initTime) + (double)tv.tv_usec/1000000;
    realTime = tmp_time;
}


WorldRouter::WorldRouter(ServerRouting & srvr) : BaseWorld(*new World()),
                                                 nextId(0),
                                                 server(srvr)
{
    setId(consts::rootWorldId);
    initTime = time(NULL) - timeoffset;
    updateTime();
    gameWorld.setId(getId());
    gameWorld.world = this;
    eobjects[getId()] = &gameWorld;
    perceptives.insert(&gameWorld);
    objectList.insert(&gameWorld);
    //WorldTime tmp_date("612-1-1 08:57:00");
    EntityFactory::init(*this);
}

WorldRouter::~WorldRouter()
{
    OpQueue::const_iterator I = operationQueue.begin();
    for (; I != operationQueue.end(); I++) {
        delete *I;
    }
    eobjects.erase(gameWorld.getId());
    EntityDict::const_iterator J = eobjects.begin();
    for(; J != eobjects.end(); J++) {
        delete J->second;
    }
    // This should be deleted here rather than in the base class because
    // we created it, and BaseWorld should not even know what it is.
    delete &gameWorld;
}

inline void WorldRouter::addOperationToQueue(RootOperation & op,
                         const BaseEntity * obj)
{
    if (op.GetFrom() == "cheat") {
        op.SetFrom(op.GetTo());
    } else {
        op.SetFrom(obj->getId());
    }
    updateTime();
    double t = realTime;
    t = t + op.GetFutureSeconds();
    op.SetSeconds(t);
    op.SetFutureSeconds(0.0);
    OpQueue::iterator I;
    for(I = operationQueue.begin();
        (I != operationQueue.end()) && ((*I)->GetSeconds() <= t) ; I++);
    operationQueue.insert(I, &op);
}

inline RootOperation * WorldRouter::getOperationFromQueue()
{
    std::list<RootOperation *>::const_iterator I = operationQueue.begin();
    if ((I == operationQueue.end()) || ((*I)->GetSeconds() > realTime)) {
        return NULL;
    }
    debug(std::cout << "pulled op off queue" << std::endl << std::flush;);
    RootOperation * op = *I;
    operationQueue.pop_front();
    return op;
}

inline void WorldRouter::setSerialno(OpVector & ops)
{
    for (OpVector::const_iterator I = ops.begin(); I != ops.end(); ++I) {
       (*I)->SetSerialno(server.getSerialNo());
    }
}

inline void WorldRouter::setSerialnoOp(RootOperation & op)
{
    op.SetSerialno(server.getSerialNo());
}

inline const std::string WorldRouter::getNewId(const std::string & name)
{
    std::stringstream buf;
#ifdef DEBUG
    buf << name << "_" << ++nextId;
#else
    buf << ++nextId;
#endif
    std::string full_id = buf.str();
    size_t index;
    while ((index = full_id.find(' ', 0)) != std::string::npos) {
        full_id[index] = '_';
    }
    return full_id;
}

Entity * WorldRouter::addObject(Entity * obj)
{
    debug(std::cout << "WorldRouter::addObject(Entity *)" << std::endl
                    << std::flush;);
    eobjects[obj->getId()] = obj;
    objectList.insert(obj);
    if (!obj->location.isValid()) {
        debug(std::cout << "set loc " << &gameWorld  << std::endl
                        << std::flush;);
        obj->location.ref = &gameWorld;
        obj->location.coords = Vector3D(0,0,0);
        debug(std::cout << "loc set with ref " << obj->location.ref->getId()
                        << std::endl << std::flush;);
    }
    obj->location.ref->contains.insert(obj);
    debug(std::cout << "Entity loc " << obj->location << std::endl
                    << std::flush;);
    obj->world = this;
    if (consts::enable_omnipresence &&
        (obj->getAttributes().find("omnipresent") !=
         obj->getAttributes().end())) {
        omnipresentList.insert(obj);
    }
    Setup * s = new Setup(Setup::Instantiate());
    s->SetTo(obj->getId());
    s->SetFutureSeconds(-0.1);
    s->SetSerialno(server.getSerialNo());
    addOperationToQueue(*s, this);
    return (obj);
}

Entity * WorldRouter::addObject(const std::string & typestr,
                                const Fragment::MapType & ent,
                                const std::string & cid)
{
    debug(std::cout << "WorldRouter::addObject(std::string, ent)" << std::endl
                    << std::flush;);
    std::string id;
    if (cid.empty()) {
        id = getNewId(typestr);
    } else {
        id = cid;
    }
    Entity * obj = EntityFactory::instance()->newEntity(id, typestr, ent);
    return addObject(obj);
}

void WorldRouter::delObject(Entity * obj)
{
    if (consts::enable_omnipresence) {
        omnipresentList.erase(obj);
    }
    perceptives.erase(obj);
    objectList.erase(obj);
    eobjects.erase(obj->getId());
}

OpVector WorldRouter::message(const RootOperation & op)
{
    debug(std::cout << "FATAL: Wrong type of WorldRouter message function called" << std::endl << std::flush;);
    return OpVector();
}

OpVector WorldRouter::message(RootOperation & op, const Entity * obj)
{
    addOperationToQueue(op, obj);
    return OpVector();
}

inline const EntitySet& WorldRouter::broadcastList(const RootOperation & op) const
{
    const Fragment::ListType & parents = op.GetParents();
    if (!parents.empty() && (parents.front().IsString())) {
        const std::string & parent = parents.front().AsString();
        if ((parent == "sight") || (parent == "sound")) {
            return perceptives;
        }
        std::string msg = std::string("Broadcasting ") + parent + " op from "
                                                       + op.GetFrom();
        log(WARNING, msg.c_str());
    } else {
        std::string msg = std::string("Broadcasting op with no parent from ")
                                                       + op.GetFrom();
        log(ERROR, msg.c_str());
    }
    return objectList;
}

inline void WorldRouter::deliverTo(const RootOperation & op, Entity * e)
{
    OpVector res = e->operation(op);
    setRefno(res, op);
    for(OpVector::const_iterator I = res.begin(); I != res.end(); I++) {
        setSerialnoOp(**I);
        message(**I, e);
    }
}

OpVector WorldRouter::operation(const RootOperation * op_ptr)
{
    const RootOperation & op = *op_ptr;
    std::string to = op.GetTo();
    debug(std::cout << "WorldRouter::operation {"
                    << op.GetParents().front().AsString() << ":"
                    << to << "}" << std::endl
                    << std::flush;);

    if (!to.empty() && (to != "all")) {
        EntityDict::const_iterator I = eobjects.find(to);
        if (I == eobjects.end()) {
            debug(std::cerr << "WARNING: Op to=\"" << to << "\""
                            << " does not exist" << std::endl << std::flush;);
            return OpVector();
        }
        Entity * to_entity = I->second;
        // This check is here because some bugs used to exist that
        // added NULL entries into the world dict. These should no
        // longer be present, so this check can be removed
        // after some testing. 2002-05-18
        if (to_entity == NULL) {
            std::string msg = std::string("CRITICAL: Op to=") + to + " is NULL";
            log(CRITICAL, msg.c_str());
            return OpVector();
        }
        deliverTo(op, to_entity);
        if ((op.GetParents().front().AsString() == "delete") &&
            (to_entity != &gameWorld)) {
            delObject(to_entity);
            to_entity->destroy();
            delete to_entity;
        }
    } else {
        RootOperation newop = op;
        const EntitySet & broadcast = broadcastList(op);
        const std::string & from = newop.GetFrom();
        EntityDict::const_iterator J = eobjects.find(from);
        if (from.empty() || (J == eobjects.end()) || (!consts::enable_ranges)) {
            EntitySet::const_iterator I;
            for(I = broadcast.begin(); I != broadcast.end(); I++) {
                newop.SetTo((*I)->getId());
                deliverTo(newop, *I);
            }
        } else {
            // This check is here because some bugs used to exist that
            // added NULL entries into the world dict. These should no
            // longer be present, so this check can be removed
            // after some testing. 2002-05-18
            if (J->second == NULL) {
                std::string msg = std::string("CRITICAL: Op from=") + from
                                + " is NULL";
                log(CRITICAL, msg.c_str());
                return OpVector();
            }
            EntitySet::const_iterator I;
            for(I = broadcast.begin(); I != broadcast.end(); I++) {
                if ((!J->second->location.inRange((*I)->location,
                                                       consts::sight_range))) {
                    debug(std::cout << "Op from " <<from<< " cannot be seen by "
                                  << (*I)->getId() << std::endl << std::flush;);
                    continue;
                }
                newop.SetTo((*I)->getId());
                deliverTo(newop, *I);
            }
        }
    }
                
    return OpVector();
}

OpVector WorldRouter::operation(const RootOperation & op)
{
    return operation(&op);
}

OpVector WorldRouter::LookOperation(const Look & op)
{
    debug(std::cout << "WorldRouter::Operation(Look)" << std::endl << std::flush;);
    const std::string & from = op.GetFrom();
    EntityDict::const_iterator J = eobjects.find(from);
    if (J == eobjects.end()) {
        debug(std::cout << "FATAL: Op has invalid from" << std::endl
                        << std::flush;);
    } else {
        debug(std::cout << "Adding [" << from << "] to perceptives"
                        << std::endl << std::flush;);
        perceptives.insert(J->second);
        if (consts::enable_ranges) {
            Sight * s = new Sight(Sight::Instantiate());

            Fragment::MapType omap;
            omap["id"] = getId();
            omap["parents"] = Fragment::ListType(1, "world");
            omap["objtype"] = "object";
            Entity * opFrom = J->second;
            const Vector3D & fromLoc = opFrom->getXyz();
            Fragment::ListType contlist;
            EntitySet::const_iterator I;
            for(I = gameWorld.contains.begin(); I != gameWorld.contains.end(); I++) {
                if ((*I)->location.inRange(fromLoc, consts::sight_range)) {
                    contlist.push_back(Fragment((*I)->getId()));
                }
            }
            if (!contlist.empty()) {
                omap["contains"] = Fragment(contlist);
            }

            Fragment::ListType args(1,Fragment(omap));
            s->SetArgs(args);
            s->SetTo(op.GetFrom());
            return OpVector(1,s);
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
            std::string msg = std::string("Exception caught in world.idle()")
                            + " thrown while processing "
                            + op->GetParents().front().AsString()
                            + " operation sent to " + op->GetTo()
                            + " from " + op->GetFrom() + ".";
            log(ERROR, msg.c_str());
        }
        delete op;
    }
    if (op == NULL) {
        return 0;
    }
    return 1;
}

Entity * WorldRouter::findByName(const std::string & name)
{
    EntityDict::const_iterator I = eobjects.begin();
    for(; I != eobjects.end(); ++I) {
        if (I->second->getName() == name) {
            return I->second;
        }
    }
    return NULL;
}

Entity * WorldRouter::findByType(const std::string & type)
{
    EntityDict::const_iterator I = eobjects.begin();
    for(; I != eobjects.end(); ++I) {
        if (I->second->getType() == type) {
            return I->second;
        }
    }
    return NULL;
}

#if 0
const double WorldRouter::upTime() const {
    return realTime - timeoffset;
}
#endif
