// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>

#include <common/Setup.h>

#include "WorldRouter.h"
#include "ServerRouting.h"
#include "EntityFactory.h"

#include <rulesets/World.h>
#include <common/debug.h>
#include <common/const.h>
#include <common/globals.h>
#include <common/stringstream.h>

using Atlas::Message::Object;

static const bool debug_flag = false;

WorldRouter::WorldRouter(ServerRouting & srvr) : BaseWorld(*new World()),
                                                 nextId(0),
                                                 server(srvr)
{
    setId("world_0");
    initTime = time(NULL) - timeoffset;
    updateTime();
    gameWorld.setId(getId());
    gameWorld.world=this;
    server.idDict[getId()]=&gameWorld;
    eobjects[getId()]=&gameWorld;
    perceptives.insert(&gameWorld);
    objectList.insert(&gameWorld);
    //WorldTime tmp_date("612-1-1 08:57:00");
}

WorldRouter::~WorldRouter()
{
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
        op.SetFrom(obj->getId());
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
    debug(std::cout << "pulled op off queue" << std::endl << std::flush;);
    RootOperation * op = *I;
    operationQueue.pop_front();
    op->SetSerialno(server.getSerialNo());
    return op;
}

inline std::string WorldRouter::getNewId(const std::string & name)
{
    std::stringstream buf;
    buf << name << "_" << ++nextId;
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
    if (obj->getId().empty()) {
        obj->setId(getNewId(obj->getName()));
    }
    server.idDict[obj->getId()]=eobjects[obj->getId()]=obj;
    objectList.insert(obj);
    if (!obj->location) {
        debug(std::cout << "set loc " << &gameWorld  << std::endl
                        << std::flush;);
        obj->location.ref=&gameWorld;
        obj->location.coords=Vector3D(0,0,0);
        debug(std::cout << "loc set with ref " << obj->location.ref->getId()
                        << std::endl << std::flush;);
    }
    if (obj->location.ref==&gameWorld) {
        debug(std::cout << "loc is world" << std::endl << std::flush;);
        gameWorld.contains.insert(obj);
        // FIXME Check here and in Thing/Entity::CreateOperation() and
        // sort out a clean way to ensure contains is correct
    }
    debug(std::cout << "Entity loc " << obj->location << std::endl
                    << std::flush;);
    obj->world=this;
    if (obj->isOmnipresent()) {
        omnipresentList.insert(obj);
    }
    Setup * s = new Setup(Setup::Instantiate());
    s->SetTo(obj->getId());
    s->SetFutureSeconds(-0.1);
    addOperationToQueue(*s, this);
    return (obj);
}

Entity * WorldRouter::addObject(const std::string & typestr,
                                const Object::MapType & ent,
                                const std::string & id)
{
    debug(std::cout << "WorldRouter::addObject(std::string, ent)" << std::endl
                    << std::flush;);
    Entity * obj;
    obj = EntityFactory::instance()->newThing(typestr, ent, eobjects);
    obj->setId(id);
    return addObject(obj);
}

void WorldRouter::delObject(Entity * obj)
{
    // Remove object from contains of its real ref?
    if (obj->location.ref != NULL) {
        obj->location.ref->contains.erase(obj);
    }
    // Remove object from world just to make sure
    // For the love of god, do we really need to do this?
    gameWorld.contains.erase(obj);
    omnipresentList.erase(obj);
    perceptives.erase(obj);
    objectList.erase(obj);
    eobjects.erase(obj->getId());
    server.idDict.erase(obj->getId());
}

oplist WorldRouter::message(const RootOperation & op)
{
    debug(std::cout << "FATAL: Wrong type of WorldRouter message function called" << std::endl << std::flush;);
    return oplist();
}

oplist WorldRouter::message(RootOperation & op, const Entity * obj)
{
    addOperationToQueue(op, obj);
    return oplist();
}

inline const eset_t& WorldRouter::broadcastList(const RootOperation & op) const
{
    const Object::ListType & parents = op.GetParents();
    if (!parents.empty() && (parents.front().IsString())) {
        const std::string & parent = parents.front().AsString();
        if ((parent == "sight") || (parent == "sound")) {
            return perceptives;
        }
    }
    return objectList;
}

oplist WorldRouter::operation(const RootOperation * op)
{
    const RootOperation & op_ref = *op;
    std::string to = op_ref.GetTo();
    debug(std::cout << "WorldRouter::operation {" << to << "}" << std::endl
                    << std::flush;);
    op_no_t op_type = opEnumerate(*op);

    if (!to.empty() && (to != "all")) {
        edict_t::const_iterator I = eobjects.find(to);
        if (I == eobjects.end()) {
            debug(std::cerr << "WARNING: Op to=\"" << to << "\""
                            << " does not exist" << std::endl << std::flush;);
            return oplist();
        }
        Entity * to_entity = I->second;
        if (to_entity == NULL) {
            std::cerr << "CRITICAL: Op to=\"" << to << "\"" << " is NULL"
                      << std::endl << std::flush;
            return oplist();
        }
        oplist res = to_entity->operation(op_ref);
        for(oplist::const_iterator I = res.begin(); I != res.end(); I++) {
            message(**I, to_entity);
        }
        if ((op_type == OP_DELETE) && (to_entity != &gameWorld)) {
            delObject(to_entity);
            to_entity->destroy();
            delete to_entity;
            to_entity = NULL;
        }
    } else {
        RootOperation newop = op_ref;
        const eset_t & broadcast = broadcastList(op_ref);
        const std::string & from = newop.GetFrom();
        edict_t::const_iterator J = eobjects.find(from);
        if (from.empty() || (J == eobjects.end()) || (!consts::enable_ranges)) {
            eset_t::const_iterator I;
            for(I = broadcast.begin(); I != broadcast.end(); I++) {
                newop.SetTo((*I)->getId());
                // FIXME: There must be a more efficient way to deliver,
                // with less chance of a loop.
                operation(&newop);
            }
        } else {
            // FIXME This is temporary until we find out how NULL pointers
            // get in there
            if (J->second == NULL) {
                std::cerr << "ERROR: NULL pointer in world dictionary. "
                          << "We will probably crash now." << std::endl
                          << std::flush;
            }
            eset_t::const_iterator I;
            for(I = broadcast.begin(); I != broadcast.end(); I++) {
                if ((!J->second->location.inRange((*I)->location,
                                                       consts::sight_range))) {
                    debug(std::cout << "Op from " <<from<< " cannot be seen by "
                                   << (*I)->getId() << std::endl << std::flush;);
                    continue;
                }
                newop.SetTo((*I)->getId());
                // FIXME: There must be a more efficient way to deliver,
                // with less chance of a loop.
                operation(&newop);
            }
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
    debug(std::cout << "WorldRouter::Operation(Look)" << std::endl << std::flush;);
    const std::string & from = op.GetFrom();
    edict_t::const_iterator J = eobjects.find(from);
    if (J == eobjects.end()) {
        debug(std::cout << "FATAL: Op has invalid from" << std::endl
                        << std::flush;);
    } else {
        debug(std::cout << "Adding [" << from << "] to perceptives"
                        << std::endl << std::flush;);
        perceptives.insert(J->second);
        if (consts::enable_ranges) {
            Sight * s = new Sight(Sight::Instantiate());

            Object::MapType omap;
            omap["id"] = getId();
            omap["parents"] = Object::ListType(1, "world");
            omap["objtype"] = "object";
            Entity * opFrom = J->second;
            const Vector3D & fromLoc = opFrom->getXyz();
            Object::ListType contlist;
            eset_t::const_iterator I;
            for(I = gameWorld.contains.begin(); I != gameWorld.contains.end(); I++) {
                if ((*I)->location.inRange(fromLoc, consts::sight_range)) {
                    contlist.push_back(Object((*I)->getId()));
                }
            }
            if (!contlist.empty()) {
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
            std::cerr << "EXCEPTION: Caught in world.idle()" << std::endl;
            std::cerr << "         : Thrown while processing ";
            std::cerr << op->GetParents().front().AsString();
            std::cerr << " operation sent to " << op->GetTo();
            std::cerr << " from " << op->GetFrom() << "." << std::endl
                      << std::flush;
        }
        delete op;
    }
    if (op==NULL) {
        return 0;
    }
    return 1;
}

#if 0
const double WorldRouter::upTime() const {
    return realTime - timeoffset;
}
#endif
