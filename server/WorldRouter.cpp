// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "WorldRouter.h"

#include "ServerRouting.h"
#include "EntityFactory.h"

#include "rulesets/World.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/const.h"
#include "common/globals.h"
#include "common/stringstream.h"
#include "common/globals.h"
#include "common/Database.h"

#include "common/Setup.h"

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>

static const bool debug_flag = false;

inline void WorldRouter::updateTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double tmp_time = (double)(tv.tv_sec - m_initTime) + (double)tv.tv_usec/1000000;
    m_realTime = tmp_time;
}


WorldRouter::WorldRouter() : BaseWorld(consts::rootWorldId,
                                       *new World(consts::rootWorldId))
{
    // setId(consts::rootWorldId);
    m_initTime = time(NULL) - timeoffset;
    updateTime();
    // m_gameWorld.setId(getId());
    m_gameWorld.m_world = this;
    m_gameWorld.setType("world");
    m_eobjects[getId()] = &m_gameWorld;
    m_perceptives.insert(&m_gameWorld);
    m_objectList.insert(&m_gameWorld);
    //WorldTime tmp_date("612-1-1 08:57:00");
    EntityFactory::init(*this);
}

WorldRouter::~WorldRouter()
{
    OpQueue::const_iterator I = m_operationQueue.begin();
    for (; I != m_operationQueue.end(); I++) {
        delete *I;
    }
    m_eobjects.erase(m_gameWorld.getId());
    EntityDict::const_iterator J = m_eobjects.begin();
    for(; J != m_eobjects.end(); J++) {
        delete J->second;
    }
    // This should be deleted here rather than in the base class because
    // we created it, and BaseWorld should not even know what it is.
    delete &m_gameWorld;
}

inline void WorldRouter::addOperationToQueue(RootOperation & op,
                         const BaseEntity * obj)
{
    if (op.getFrom() == "cheat") {
        op.setFrom(op.getTo());
    } else {
        op.setFrom(obj->getId());
    }
    updateTime();
    double t = m_realTime;
    t = t + op.getFutureSeconds();
    op.setSeconds(t);
    op.setFutureSeconds(0.0);
    OpQueue::iterator I;
    for(I = m_operationQueue.begin();
        (I != m_operationQueue.end()) && ((*I)->getSeconds() <= t) ; I++);
    m_operationQueue.insert(I, &op);
}

inline RootOperation * WorldRouter::getOperationFromQueue()
{
    std::list<RootOperation *>::const_iterator I = m_operationQueue.begin();
    if ((I == m_operationQueue.end()) || ((*I)->getSeconds() > m_realTime)) {
        return NULL;
    }
    debug(std::cout << "pulled op off queue" << std::endl << std::flush;);
    RootOperation * op = *I;
    m_operationQueue.pop_front();
    return op;
}

inline void WorldRouter::setSerialno(OpVector & ops)
{
    for (OpVector::const_iterator I = ops.begin(); I != ops.end(); ++I) {
       (*I)->setSerialno(getSerialNo());
    }
}

inline void WorldRouter::setSerialnoOp(RootOperation & op)
{
    op.setSerialno(getSerialNo());
}

Entity * WorldRouter::addObject(Entity * obj, bool setup)
{
    debug(std::cout << "WorldRouter::addObject(Entity *)" << std::endl
                    << std::flush;);
    m_eobjects[obj->getId()] = obj;
    m_objectList.insert(obj);
    if (!obj->m_location.isValid()) {
        debug(std::cout << "set loc " << &m_gameWorld  << std::endl
                        << std::flush;);
        obj->m_location.m_loc = &m_gameWorld;
        obj->m_location.m_pos = Vector3D(0,0,0);
        debug(std::cout << "loc set with loc " << obj->m_location.m_loc->getId()
                        << std::endl << std::flush;);
    }
    bool cont_change = obj->m_location.m_loc->m_contains.empty();
    obj->m_location.m_loc->m_contains.insert(obj);
    if (cont_change) {
        obj->m_location.m_loc->m_update_flags |= a_cont;
        obj->m_location.m_loc->updated.emit();
    }
    debug(std::cout << "Entity loc " << obj->m_location << std::endl
                    << std::flush;);
    obj->m_world = this;
    if (consts::enable_omnipresence &&
        (obj->getAttributes().find("omnipresent") !=
         obj->getAttributes().end())) {
        m_omnipresentList.insert(obj);
    }
    if (setup) {
        Setup * s = new Setup(Setup::Instantiate());
        s->setTo(obj->getId());
        s->setFutureSeconds(-0.1);
        s->setSerialno(getSerialNo());
        addOperationToQueue(*s, this);
    }
    return (obj);
}

Entity * WorldRouter::addNewObject(const std::string & typestr,
                                   const Element::MapType & ent)
{
    debug(std::cout << "WorldRouter::addNewObject(std::string, ent)"
                    << std::endl << std::flush;);
    std::string id;
    Database::instance()->getEntityId(id);
    Entity * obj = EntityFactory::instance()->newEntity(id, typestr, ent);
    if (obj == 0) {
        std::string msg = std::string("Attempt to create an entity of type \"")
                          + typestr + "\" but type is unknown";
        log(ERROR, msg.c_str());
        return 0;
    }
    return addObject(obj);
}

void WorldRouter::delObject(Entity * obj)
{
    if (consts::enable_omnipresence) {
        m_omnipresentList.erase(obj);
    }
    m_perceptives.erase(obj);
    m_objectList.erase(obj);
    m_eobjects.erase(obj->getId());
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
    const Element::ListType & parents = op.getParents();
    if (!parents.empty() && (parents.front().isString())) {
        const std::string & parent = parents.front().asString();
        if ((parent == "sight") || (parent == "sound")) {
            return m_perceptives;
        }
        std::string msg = std::string("Broadcasting ") + parent + " op from "
                                                       + op.getFrom();
        log(WARNING, msg.c_str());
    } else {
        std::string msg = std::string("Broadcasting op with no parent from ")
                                                       + op.getFrom();
        log(ERROR, msg.c_str());
    }
    return m_objectList;
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

OpVector WorldRouter::operation(const RootOperation & op)
{
    // const RootOperation & op = *op_ptr;
    std::string to = op.getTo();
    debug(std::cout << "WorldRouter::operation {"
                    << op.getParents().front().asString() << ":"
                    << op.getFrom() << ":" << to << "}" << std::endl
                    << std::flush;);

    if (!to.empty() && (to != "all")) {
        EntityDict::const_iterator I = m_eobjects.find(to);
        if (I == m_eobjects.end()) {
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
        if ((op.getParents().front().asString() == "delete") &&
            (to_entity != &m_gameWorld)) {
            delObject(to_entity);
            to_entity->destroy();
            delete to_entity;
        }
    } else {
        RootOperation newop = op;
        const EntitySet & broadcast = broadcastList(op);
        const std::string & from = newop.getFrom();
        EntityDict::const_iterator J = m_eobjects.find(from);
        if (from.empty() || (J == m_eobjects.end()) || (!consts::enable_ranges)) {
            EntitySet::const_iterator I;
            for(I = broadcast.begin(); I != broadcast.end(); I++) {
                newop.setTo((*I)->getId());
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
                if ((!J->second->m_location.inRange((*I)->m_location,
                                                       consts::sight_range))) {
                    debug(std::cout << "Op from " << from
                                    << " cannot be seen by " << (*I)->getId()
                                    << std::endl << std::flush;);
                    continue;
                }
                newop.setTo((*I)->getId());
                deliverTo(newop, *I);
            }
        }
    }
                
    return OpVector();
}

OpVector WorldRouter::LookOperation(const Look & op)
{
    debug(std::cout << "WorldRouter::Operation(Look)" << std::endl << std::flush;);
    const std::string & from = op.getFrom();
    EntityDict::const_iterator J = m_eobjects.find(from);
    if (J != m_eobjects.end()) {
        m_perceptives.insert(J->second);
    }

    return OpVector();
}

int WorldRouter::idle()
{
    updateTime();
    RootOperation * op;
    while ((op = getOperationFromQueue()) != NULL) {
        try {
            operation(*op);
        }
        catch (...) {
            std::string msg = std::string("Exception caught in world.idle()")
                            + " thrown while processing "
                            + op->getParents().front().asString()
                            + " operation sent to " + op->getTo()
                            + " from " + op->getFrom() + ".";
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
    EntityDict::const_iterator I = m_eobjects.begin();
    for(; I != m_eobjects.end(); ++I) {
        if (I->second->getName() == name) {
            return I->second;
        }
    }
    return NULL;
}

Entity * WorldRouter::findByType(const std::string & type)
{
    EntityDict::const_iterator I = m_eobjects.begin();
    for(; I != m_eobjects.end(); ++I) {
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
