// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "WorldRouter.h"

#include "EntityFactory.h"

#include "rulesets/World.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/const.h"
#include "common/globals.h"
#include "common/Database.h"
#include "common/random.h"
#include "common/serialno.h"
#include "common/compose.hpp"

#include "common/Setup.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>

#include <sstream>
#include <algorithm>

using Atlas::Message::Element;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Entity::RootEntity;

static const bool debug_flag = false;

struct OpQueEntry {
    Operation op;
    Entity & from;

    explicit OpQueEntry(const Operation & o, Entity & f);
    OpQueEntry(const OpQueEntry & o);
    ~OpQueEntry();

    const Operation & operator*() const {
        return op;
    }

    Atlas::Objects::Operation::RootOperationData * operator->() const {
        return op.get();
    }
};

inline OpQueEntry::OpQueEntry(const Operation & o, Entity & f) : op(o), from(f)
{
    from.incRef();
}

inline OpQueEntry::OpQueEntry(const OpQueEntry & o) : op(o.op), from(o.from)
{
    from.incRef();
}

inline OpQueEntry::~OpQueEntry()
{
    from.decRef();
}


/// \brief Update the in-game time.
///
/// Reads the system time, and applies the necessary offsets to calculate
/// the in-game time. This is the stored, and can be accessed using getTime().
void WorldRouter::updateTime(int sec, int usec)
{
    double tmp_time = (double)(sec + timeoffset - m_initTime) + (double)usec/1000000;
    m_realTime = tmp_time;
}


/// \brief Constructor for the world object.
///
/// The Entity representing the world is implicity constructed.
/// Currently the world entity is included in the perceptives list,
/// but I am not clear why. Need to look into why.
WorldRouter::WorldRouter() : BaseWorld(*new World(consts::rootWorldId, consts::rootWorldIntId))
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    m_initTime = tv.tv_sec;
    updateTime(tv.tv_sec, tv.tv_usec);
    m_gameWorld.incRef();
    m_gameWorld.m_world = this;
    m_gameWorld.setType("world");
    m_eobjects[m_gameWorld.getId()] = &m_gameWorld;
    m_perceptives.insert(&m_gameWorld);
    m_objectList.insert(&m_gameWorld);
    //WorldTime tmp_date("612-1-1 08:57:00");
    EntityFactory::init(*this);
}

/// \brief Destructor for the world object.
///
/// Destruction of the world object implicity deletes all IG objects in
/// the server, clears the operation queue
WorldRouter::~WorldRouter()
{
    { 
        debug(std::cout << "Flushing world with " << m_eobjects.size()
                        << " entities" << std::endl << std::flush;);
    }
    EntityDict::const_iterator Jend = m_eobjects.end();
    for (EntityDict::const_iterator J = m_eobjects.begin(); J != Jend; ++J) {
        J->second->decRef();
    }
    // This should be deleted here rather than in the base class because
    // we created it, and BaseWorld should not even know what it is.
    m_gameWorld.decRef();
}

/// \brief Add an operation to the ordered op queue.
///
/// Any time adjustment required is made to the operation, and it
/// is added to the apropriate place in the chronologically ordered
/// queue. The From attribute of the operation is set to the id of
/// the entity that is responsible for adding the operation to the
/// queue, unless it is set to "cheat". This is used to spook
/// operations when they come from an admin.
void WorldRouter::addOperationToQueue(const Operation & op, Entity & ent)
{
    assert(op.isValid());
    if (op->getFrom() == "cheat") {
        op->setFrom(op->getTo());
    } else {
        op->setFrom(ent.getId());
    }
    double fs = op->getFutureSeconds();
    if (fs == 0.) {
        op->setSeconds(m_realTime);
        m_immediateQueue.push_back(OpQueEntry(op, ent));
        return;
    }
    double t = m_realTime + fs;
    op->setSeconds(t);
    op->setFutureSeconds(0.);
    OpQueue::iterator I = m_operationQueue.begin();
    OpQueue::iterator Iend = m_operationQueue.end();
    for (; (I != Iend) && ((*I).op->getSeconds() <= t) ; ++I);
    m_operationQueue.insert(I, OpQueEntry(op, ent));
}

/// \brief Get the next due operation from the queue.
///
/// If the operation at the end of the queue is now due, return it.
/// This function is now unused, and has become obsolete now there are
/// two queues. If this function is needed again, it will need to be
/// recoded. See idle for sample code that checks for the next due operation.
/// @return a pointer to the operation due for dispatch, or 0 if none
/// is due.
Operation WorldRouter::getOperationFromQueue()
{
    OpQueue::const_iterator I = m_operationQueue.begin();
    if ((I == m_operationQueue.end()) || ((*I)->getSeconds() > m_realTime)) {
        return NULL;
    }
    debug(std::cout << "pulled op off queue" << std::endl << std::flush;);
    const Operation & op = (**I);
    m_operationQueue.pop_front();
    return op;
}

/// \brief Provide an adjusted heigh for the given entity.
///
/// If the position has a parent which has an associated geometry
/// which define its childrens position, e.g terrain or a floor,
/// calculate what the Z coord, being the height of the entity.
/// This function recurses through the parents until it finds
/// A parent which defines the height.
/// @return the modified Z coord of the position.
float WorldRouter::constrainHeight(Entity * parent, const Point3D & pos,
                                   const std::string & mode)
{
    assert(parent != 0);
    World * wrld = dynamic_cast<World*>(parent);
    if (wrld != 0) {
        float h;
        h = wrld->getHeight(pos.x(), pos.y());
        if (mode == "fixed") {
            h = pos.z();
        } else if (mode == "floating") {
            h = 0;
        } else if (mode == "swimming") {
            h = std::max(h, std::min(0.f, pos.z()));
        }
        debug(std::cout << "Fix height " << pos.z() << " to " << h
                        << std::endl << std::flush;);
        return h;
    } else {
        static const Quaternion identity(Quaternion().identity());
        assert(parent->m_location.m_loc != 0);
        // FIXME take account of orientation
        const Point3D & ppos = parent->m_location.pos();
        debug(std::cout << "parent " << parent->getId() << " of type "
                        << parent->getType() << " pos " << ppos.z()
                        << " my pos " << pos.z()
                        << std::endl << std::flush;);
        float h;
        // FIXME Recently swapped argument order as it appears to fix
        // a bug. Check carefully whether this is doing the right thing.
        const Quaternion & parent_orientation = parent->m_location.orientation().isValid() ? parent->m_location.orientation() : identity;
        h =  ppos.z() - constrainHeight(parent->m_location.m_loc,
                            pos.toParentCoords(parent->m_location.pos(),
                                               parent_orientation),
                            mode
                           );
        debug(std::cout << "Correcting height from " << pos.z() << " to " << h
                        << std::endl << std::flush;);
        return h;
    }
}

/// \brief Add a new entity to the world.
///
/// Adds a new entity to the lists maintained by the WorldRouter.
/// Verify that the entity has a valid location, setting to
/// the default spawn area if necessary. Handle inserting the
/// entity into the loc/contains tree maintained by the Entity
/// class. Handle omnipresent entities, and sending a Setup op
/// to the entity.
Entity * WorldRouter::addEntity(Entity * ent, bool setup)
{
    debug(std::cout << "WorldRouter::addEntity(Entity *)" << std::endl
                    << std::flush;);
    m_eobjects[ent->getId()] = ent;
    m_objectList.insert(ent);
    assert(ent->m_location.isValid());

    if (!ent->m_location.isValid()) {
        log(ERROR, "Entity added to world with invalid location!");
        debug(std::cout << "set loc " << &m_gameWorld  << std::endl
                        << std::flush;);
        ent->m_location.m_loc = &m_gameWorld;
        ent->m_location.m_pos = Point3D(uniform(-8,8), uniform(-8,8), 0);
        debug(std::cout << "loc set with loc " << ent->m_location.m_loc->getId()
                        << std::endl << std::flush;);
    }
    ent->m_location.update(getTime());
    // FIXME
    std::string mode;
    if (ent->has("mode")) {
        Element mode_attr;
        ent->get("mode", mode_attr);
        if (mode_attr.isString()) {
            mode = mode_attr.String();
        } else {
            log(ERROR, String::compose("Mode on entity is a %1 in WorldRouter::addEntity", Element::typeName(mode_attr.getType())).c_str());
        }
    }
    ent->m_location.m_pos.z() = constrainHeight(ent->m_location.m_loc,
                                                ent->m_location.pos(), mode);
    bool cont_change = ent->m_location.m_loc->m_contains.empty();
    ent->m_location.m_loc->m_contains.insert(ent);
    ent->m_location.m_loc->incRef();
    if (cont_change) {
        ent->m_location.m_loc->m_update_flags |= a_cont;
        ent->m_location.m_loc->updated.emit();
    }
    debug(std::cout << "Entity loc " << ent->m_location << std::endl
                    << std::flush;);
    ent->m_world = this;
    if (consts::enable_omnipresence &&
        (ent->getAttributes().find("omnipresent") !=
         ent->getAttributes().end())) {
        m_omnipresentList.insert(ent);
    }
    if (setup) {
        Setup s;
        s->setTo(ent->getId());
        s->setFutureSeconds(-0.1);
        s->setSerialno(newSerialNo());
        message(s, m_gameWorld);
    }
    return ent;
}

/// \brief Create a new entity and add to the world.
///
/// Construct a new entity using the entity description provided,
/// and pass it to addEntity().
/// @return a pointer to the new entity.
Entity * WorldRouter::addNewEntity(const std::string & typestr,
                                   const RootEntity & attrs)
{
    debug(std::cout << "WorldRouter::addNewEntity(\"" << typestr << "\", attrs)"
                    << std::endl << std::flush;);
    std::string id;
    if (consts::enable_database) {
        Database::instance()->newId(id);
    } else {
        newId(id);
    }
    assert(!id.empty());

    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        log(ERROR, String::compose("Unable to convert ID \"%1\" to an integer", id).c_str());
    }

    Entity * ent = EntityFactory::instance()->newEntity(id, intId, typestr, attrs);
    if (ent == 0) {
        std::string msg = std::string("Attempt to create an entity of type \"")
                          + typestr + "\" but type is unknown or forbidden";
        log(ERROR, msg.c_str());
        return 0;
    }
    return addEntity(ent);
}

/// \brief Remove an entity from the world.
///
/// Remove an entity from the various lists in which it is stored.
/// The entity is removed from the LOC/CONTAINS tree, and the
/// reference held by the world is decremented. There may still be
/// a reference held by an operation in the queue from the removed
/// entity.
void WorldRouter::delEntity(Entity * ent)
{
    if (ent == &m_gameWorld) {
        log(WARNING, "Attempt to delete game world");
        return;
    }
    if (consts::enable_omnipresence) {
        m_omnipresentList.erase(ent);
    }
    m_perceptives.erase(ent);
    m_objectList.erase(ent);
    m_eobjects.erase(ent->getId());
    ent->destroy();
    ent->decRef();
}

/// \brief Pass an operation to the World.
///
/// Pass an operation to addOperationToQueue()
/// so it gets added to the queue for dispatch.
void WorldRouter::message(const Operation & op, Entity & ent)
{
    addOperationToQueue(op, ent);
    debug(std::cout << "WorldRouter::message {"
                    << op->getParents().front() << ":"
                    << op->getFrom() << ":" << op->getTo() << "}" << std::endl
                    << std::flush;);
}

/// \brief Determine the broadcast list to be used to broadcast an operation.
///
/// Check the type of operation, and work out which list of entities
/// it should be broadcast to. This will be perceptives in case 
/// a perception operation, or all entities in any other case.
/// This should probably go, as there is essentially no sane reason
/// for broadcasting a random op to all entities.
/// @return a reference to the list of entities to be used for braodcast.
const EntitySet & WorldRouter::broadcastList(const Operation & op) const
{
    // FIXME Use numeric types
    const std::list<std::string> & parents = op->getParents();
    if (!parents.empty()) {
        const std::string & parent = parents.front();
        if ((parent == "sight") || (parent == "sound") ||
            (parent == "appearance") || (parent == "disappearance")) {
            return m_perceptives;
        }
        std::string msg = std::string("Broadcasting ") + parent + " op from "
                                                       + op->getFrom();
        log(WARNING, msg.c_str());
    } else {
        std::string msg = std::string("Broadcasting op with no parent from ")
                                                       + op->getFrom();
        log(ERROR, msg.c_str());
    }
    return m_objectList;
}

/// \brief Deliver an operation to its target.
///
/// Pass the operation to the target entity. The resulting operations
/// have their ref numbers set, and are added to the queue for
/// dispatch.
void WorldRouter::deliverTo(const Operation & op, Entity & ent)
{
    OpVector res;
    ent.operation(op, res);
    OpVector::const_iterator Iend = res.end();
    for(OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
        if (op->getFrom() == (*I)->getTo()) {
            (*I)->setRefno(op->getSerialno());
        }
        (*I)->setSerialno(newSerialNo());
        message(*I, ent);
    }
}

/// \brief Main in-game operation dispatch function.
///
/// Operations are passed here when they are due for dispatch.
/// Determine the target of the operation and deliver it directly,
/// or broadcast if broadcast is required. This function implements
/// sight ranges for perception operations.
/// @param op operation to be dispatched to the world. This is non-const
/// so that broadcast ops can have their TO set correctly for each target.
void WorldRouter::operation(const Operation & op, Entity & from)
{
    const std::string & to = op->getTo();
    debug(std::cout << "WorldRouter::operation {"
                    << op->getParents().front() << ":"
                    << op->getFrom() << ":" << to << "}" << std::endl
                    << std::flush;);

    if (!to.empty()) {
        Entity * to_entity = 0;

        if (to == from.getId()) {
            if (from.isDestroyed()) {
                // Entity no longer exists
                return;
            }
            to_entity = &from;
        } else {
            EntityDict::const_iterator I = m_eobjects.find(to);
            if (I == m_eobjects.end()) {
                debug(std::cerr << "WARNING: Op to=\"" << to << "\""
                                << " does not exist"
                                << std::endl << std::flush;);
                return;
            }
            to_entity = I->second;
        }

        assert(to_entity != 0);

        deliverTo(op, *to_entity);
        if (op->getParents().front() == "delete") { // FIXME numeric type
            assert(op->getClassNo() == Atlas::Objects::Operation::DELETE_NO);
            delEntity(to_entity);
        }
    } else {
        // Where broadcasts go depends on type of op
        const EntitySet & broadcast = broadcastList(op);
        assert(op->getFrom() == from.getId());
        if (!consts::enable_ranges) {
            EntitySet::const_iterator I = broadcast.begin();
            EntitySet::const_iterator Iend = broadcast.end();
            for (; I != Iend; ++I) {
                op->setTo((*I)->getId());
                deliverTo(op, **I);
            }
        } else {
            float fromSquSize = boxSquareSize(from.m_location.bBox());
            EntitySet::const_iterator I = broadcast.begin();
            EntitySet::const_iterator Iend = broadcast.end();
            for (; I != Iend; ++I) {
                // Calculate square distance to target
                float dist = squareDistance(from.m_location, (*I)->m_location);
                float view_factor = fromSquSize / dist;
                if (view_factor < consts::square_sight_factor) {
                    debug(std::cout << "Op from " << from.getId()
                                    << " cannot be seen by " << (*I)->getId()
                                    << std::endl << std::flush;);
                    continue;
                }
                op->setTo((*I)->getId());
                deliverTo(op, **I);
            }
        }
    }
}

/// Add entity provided to this list of perceptive entities.
/// Look up the entity with the id provided, and add a pointer
/// to the entity to the set of perceptive entities. This method is
/// called when key events occur that indicate that the entity in
/// question can receive broadcase perception operations.
void WorldRouter::addPerceptive(const std::string & id)
{
    debug(std::cout << "WorldRouter::addPerceptive" << std::endl << std::flush;);
    EntityDict::const_iterator J = m_eobjects.find(id);
    if (J != m_eobjects.end()) {
        m_perceptives.insert(J->second);
    }
}

/// Main world loop function.
/// This function is called whenever the communications code is idle.
/// It updates the in-game time, and dispatches operations that are
/// now due for dispatch. The number of operations dispatched is limited
/// to 10 to ensure that client communications are always handled in a timely
/// manner. If the maximum number of operations are dispatched, the return 
/// value indicates that this is the case, and the communications code
/// will call this function again as soon as possible rather than sleeping.
/// This ensures that the maximum possible number of operations are dispatched
/// without becoming unresponsive to client communications traffic.
/// @param sec world time seconds component
/// @param usec world time microseconds component
bool WorldRouter::idle(int sec, int usec)
{
    updateTime(sec, usec);
    unsigned int op_count = 0;
    OpQueue::iterator I = m_operationQueue.begin();
    OpQueue::iterator Iend = m_operationQueue.end();
    while ((++op_count < 10) && (I != Iend) &&
           ((*I)->getSeconds() <= m_realTime)) {
        assert(I != m_operationQueue.end());
        OpQueEntry & oqe = *I;
        Dispatching.emit(oqe.op);
        try {
            operation(oqe.op, oqe.from);
        }
        catch (...) {
            std::string msg = std::string("Exception caught in world.idle()")
                            + " thrown while processing "
                            // + oqe->getParents().front()
                            + " operation sent to " + oqe->getTo()
                            + " from " + oqe->getFrom() + ".";
            log(ERROR, msg.c_str());
        }
        m_operationQueue.erase(I);
        I = m_operationQueue.begin();
    }

    I = m_immediateQueue.begin();
    Iend = m_immediateQueue.end();
    while ((++op_count < 10) && (I != Iend)) {
        assert(I != m_immediateQueue.end());
        OpQueEntry & oqe = *I;
        Dispatching.emit(oqe.op);
        try {
            operation(oqe.op, oqe.from);
        }
        catch (...) {
            std::string msg = std::string("Exception caught in world.idle()")
                            + " thrown while processing "
                            // + oqe->getParents().front()
                            + " operation sent to " + oqe->getTo()
                            + " from " + oqe->getFrom() + ".";
            log(ERROR, msg.c_str());
        }
        m_immediateQueue.erase(I);
        I = m_immediateQueue.begin();
    }
    // If we have processed the maximum number for this call, return true
    // to tell the server not to sleep when polling clients. This ensures
    // that we keep processing ops at a the maximum rate without leaving
    // clients unattended.
    return (op_count >= 10);
}

/// Find an entity of the given name. This is provided to allow administrators
/// to perform certain admin tasks. It finds and returns the first instance
/// with the name provided in the game world.
/// @param name string specifying name of the instance required.
/// @return a pointer to an entity with the type required, or zero if an
/// instance with this name was not found.
Entity * WorldRouter::findByName(const std::string & name)
{
    EntityDict::const_iterator Iend = m_eobjects.end();
    for (EntityDict::const_iterator I = m_eobjects.begin(); I != Iend; ++I) {
        if (I->second->getName() == name) {
            return I->second;
        }
    }
    return NULL;
}

/// Find an entity of the given type. This is provided to allow administrators
/// to perform certain admin tasks. It finds and returns the first instance
/// of the type provided in the game world.
/// @param type string specifying the class name of the instance required.
/// @return a pointer to an entity of the type required, or zero if no
/// instance was found.
Entity * WorldRouter::findByType(const std::string & type)
{
    EntityDict::const_iterator Iend = m_eobjects.end();
    for(EntityDict::const_iterator I = m_eobjects.begin(); I != Iend; ++I) {
        if (I->second->getType() == type) {
            return I->second;
        }
    }
    return NULL;
}
