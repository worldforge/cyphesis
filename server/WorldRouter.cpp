// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "WorldRouter.h"

#include "EntityFactory.h"

#include "rulesets/World.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/const.h"
#include "common/globals.h"
#include "common/Database.h"
#include "common/random.h"
#include "common/refno.h"
#include "common/serialno.h"

#include "common/Setup.h"

#include <sstream>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Setup;

static const bool debug_flag = false;

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
WorldRouter::WorldRouter() : BaseWorld(*new World(consts::rootWorldId))
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    m_initTime = tv.tv_sec;
    updateTime(tv.tv_sec, tv.tv_usec);
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
        debug(std::cout << "Flushing op queue with " << m_operationQueue.size()
                        << " ops" << std::endl << std::flush;);
    }
    OpQueue::const_iterator Iend = m_operationQueue.end();
    for (OpQueue::const_iterator I = m_operationQueue.begin(); I != Iend; ++I) {
        delete *I;
    }
    { 
        debug(std::cout << "Flushing world with " << m_eobjects.size()
                        << " entities" << std::endl << std::flush;);
    }
    m_eobjects.erase(m_gameWorld.getId());
    EntityDict::const_iterator Jend = m_eobjects.end();
    for (EntityDict::const_iterator J = m_eobjects.begin(); J != Jend; ++J) {
        delete J->second;
    }
    // This should be deleted here rather than in the base class because
    // we created it, and BaseWorld should not even know what it is.
    delete &m_gameWorld;
}

/// \brief Add an operation to the ordered op queue.
///
/// Any time adjustment required is made to the operation, and it
/// is added to the apropriate place in the chronologically ordered
/// queue. The From attribute of the operation is set to the id of
/// the entity that is responsible for adding the operation to the
/// queue, unless it is set to "cheat". This is used to spook
/// operations when they come from an admin.
void WorldRouter::addOperationToQueue(Operation & op, const Entity * obj)
{
    if (op.getFrom() == "cheat") {
        op.setFrom(op.getTo());
    } else {
        op.setFrom(obj->getId());
    }
    double t = m_realTime + op.getFutureSeconds();
    op.setSeconds(t);
    op.setFutureSeconds(0.0);
    OpQueue::iterator I = m_operationQueue.begin();
    OpQueue::iterator Iend = m_operationQueue.end();
    for (; (I != Iend) && ((*I)->getSeconds() <= t) ; ++I);
    m_operationQueue.insert(I, &op);
}

/// \brief Get the next due operation from the queue.
///
/// If the operation at the end of the queue is now due, return it.
/// @return a pointer to the operation due for dispatch, or 0 if none
/// is due.
Operation * WorldRouter::getOperationFromQueue()
{
    OpQueue::const_iterator I = m_operationQueue.begin();
    if ((I == m_operationQueue.end()) || ((*I)->getSeconds() > m_realTime)) {
        return NULL;
    }
    debug(std::cout << "pulled op off queue" << std::endl << std::flush;);
    Operation * op = *I;
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
float WorldRouter::constrainHeight(Entity * parent, const Point3D & pos)
{
    assert(parent != 0);
    World * wrld = dynamic_cast<World*>(parent);
    if (wrld != 0) {
        float h = wrld->getHeight(pos.x(), pos.y());
        debug(std::cout << "Fix height " << pos.z() << " to " << h
                        << std::endl << std::flush;);
        return h;
    } else {
        assert(parent->m_location.m_loc != 0);
        // FIXME take account of orientation
        const Point3D & ppos = parent->m_location.m_pos;
        debug(std::cout << "parent " << parent->getId() << " of type "
                        << parent->getType() << " pos " << ppos.z()
                        << " my pos " << pos.z()
                        << std::endl << std::flush;);
        float h;
        // FIXME Is it safe to use m_orientation without checking it
        h = constrainHeight(parent->m_location.m_loc,
                            pos.toParentCoords(parent->m_location.m_pos,
                                               parent->m_location.m_orientation)
                           ) - ppos.z();
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
Entity * WorldRouter::addObject(Entity * obj, bool setup)
{
    debug(std::cout << "WorldRouter::addObject(Entity *)" << std::endl
                    << std::flush;);
    m_eobjects[obj->getId()] = obj;
    m_objectList.insert(obj);
    assert(obj->m_location.isValid());

    if (!obj->m_location.isValid()) {
        log(ERROR, "Entity added to world with invalid location!");
        debug(std::cout << "set loc " << &m_gameWorld  << std::endl
                        << std::flush;);
        obj->m_location.m_loc = &m_gameWorld;
        obj->m_location.m_pos = Point3D(uniform(-8,8), uniform(-8,8), 0);
        debug(std::cout << "loc set with loc " << obj->m_location.m_loc->getId()
                        << std::endl << std::flush;);
    }
    obj->m_location.m_pos.z() = constrainHeight(obj->m_location.m_loc,
                                                obj->m_location.m_pos);
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
        Setup * s = new Setup;
        s->setTo(obj->getId());
        s->setFutureSeconds(-0.1);
        s->setSerialno(newSerialNo());
        message(*s, &m_gameWorld);
    }
    return (obj);
}

/// \brief Create a new entity and add to the world.
///
/// Construct a new entity using the entity description provided,
/// and pass it to addObject().
/// @return a pointer to the new entity.
Entity * WorldRouter::addNewObject(const std::string & typestr,
                                   const MapType & ent)
{
    debug(std::cout << "WorldRouter::addNewObject(\"" << typestr << "\", ent)"
                    << std::endl << std::flush;);
    std::string id;
    Database::instance()->newId(id);
    Entity * obj = EntityFactory::instance()->newEntity(id, typestr, ent);
    if (obj == 0) {
        std::string msg = std::string("Attempt to create an entity of type \"")
                          + typestr + "\" but type is unknown";
        log(ERROR, msg.c_str());
        return 0;
    }
    return addObject(obj);
}

/// \brief Remove an entity from the world.
///
/// Remove an entity from the various lists in which it is stored.
/// The entity is not deleted, nor any attend made to handle
/// the loc/contains. It would probably be a good idea to move
/// some of this handling to this function.
void WorldRouter::delObject(Entity * obj)
{
    if (consts::enable_omnipresence) {
        m_omnipresentList.erase(obj);
    }
    m_perceptives.erase(obj);
    m_objectList.erase(obj);
    m_eobjects.erase(obj->getId());
}

/// \brief Pass an operation to the World.
///
/// Pass an operation to addOperationToQueue()
/// so it gets added to the queue for dispatch.
void WorldRouter::message(Operation & op, const Entity * obj)
{
    debug(std::cout << "WorldRouter::message {"
                    << op.getParents().front().asString() << ":"
                    << op.getFrom() << ":" << op.getTo() << "}" << std::endl
                    << std::flush;);
    addOperationToQueue(op, obj);
    debug(std::cout << "WorldRouter::message {"
                    << op.getParents().front().asString() << ":"
                    << op.getFrom() << ":" << op.getTo() << "}" << std::endl
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
    const ListType & parents = op.getParents();
    if (!parents.empty() && (parents.front().isString())) {
        const std::string & parent = parents.front().asString();
        if ((parent == "sight") || (parent == "sound") ||
            (parent == "appearance") || (parent == "disappearance")) {
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

/// \brief Deliver an operation to its target.
///
/// Pass the operation to the target entity. The resulting operations
/// have their ref numbers set, and are added to the queue for
/// dispatch.
void WorldRouter::deliverTo(const Operation & op, Entity * e)
{
    OpVector res;
    e->operation(op, res);
    setRefno(res, op);
    OpVector::const_iterator Iend = res.end();
    for(OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
        (*I)->setSerialno(newSerialNo());
        message(**I, e);
    }
}

/// \brief Special version of WorldRouter::deliverTo() for delete ops.
///
/// Delete is special. It causes the target to be removed, but
/// we need to handle the responses first. To prevent a tight loop,
/// we do not attempt to immediatly handle the response to a delete op
/// if it is anothe delete op.
void WorldRouter::deliverDeleteTo(const Operation & op, Entity * e)
{
    OpVector res;
    e->operation(op, res);
    setRefno(res, op);
    OpVector::const_iterator Iend = res.end();
    for(OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
        Operation & newOp = **I;
        newOp.setSerialno(newSerialNo());
        if (newOp.getParents().front().asString() == "delete") {
            // If this is a delete, queue as normal to avoid a recursive loop
            debug(std::cerr << "Handling delete response to delete"
                            << std::endl << std::flush;);
            log(WARNING, "Response to Delete op is another Delete.");
            message(newOp, e);
        } else {
            // Other ops we dispatch immediatly before deleting the source
            debug(std::cerr << "Handling normal response to delete"
                            << std::endl << std::flush;);
            newOp.setFrom(e->getId());
            operation(newOp);
            delete &newOp;
        }
    }
    if (e == &m_gameWorld) {
        log(WARNING, "Attempt to delete game world");
        return;
    }
    delObject(e);
    e->destroy();
    delete e;
}

/// Main in-game operation dispatch function.
/// Operations are passed here when they are due for dispatch.
/// Determine the target of the operation and deliver it directly,
/// or broadcast if broadcast is required. This function implements
/// sight ranges for perception operations.
/// @param op operation to be dispatched to the world. This is non-const
/// so that broadcast ops can have their TO set correctly for each target.
void WorldRouter::operation(Operation & op)
{
    const std::string & to = op.getTo();
    debug(std::cout << "WorldRouter::operation {"
                    << op.getParents().front().asString() << ":"
                    << op.getFrom() << ":" << to << "}" << std::endl
                    << std::flush;);

    if (!to.empty()) {
        EntityDict::const_iterator I = m_eobjects.find(to);
        if (I == m_eobjects.end()) {
            debug(std::cerr << "WARNING: Op to=\"" << to << "\""
                            << " does not exist" << std::endl << std::flush;);
            return;
        }

        Entity * to_entity = I->second;
        assert(to_entity != 0);

        if (op.getParents().front().asString() == "delete") {
            deliverDeleteTo(op, to_entity);
        } else {
            deliverTo(op, to_entity);
        }
    } else {
        // Operation newop = op;
        const EntitySet & broadcast = broadcastList(op);
        const std::string & from = op.getFrom();
        EntityDict::const_iterator J = m_eobjects.find(from);
        if (from.empty()) {
            log(ERROR, "WorldRouter::operation dispatching op with no from");
            if (J != m_eobjects.end()) {
                log(ERROR, "WorldRouter::operation empty id found in IG map");
            }
        }
        if ((J == m_eobjects.end()) || (!consts::enable_ranges)) {
            log(ERROR, "WorldRouter::operation broadcasting op with missing from");
            std::cerr << op.getParents().front().asString() << " op from "
                      << from << std::endl << std::flush;
            EntitySet::const_iterator I = broadcast.begin();
            EntitySet::const_iterator Iend = broadcast.end();
            for (; I != Iend; ++I) {
                op.setTo((*I)->getId());
                deliverTo(op, *I);
            }
        } else {
            Entity * fromEnt = J->second;
            assert(fromEnt != NULL);
            float fromSquSize = boxSquareSize(fromEnt->m_location.m_bBox);
            EntitySet::const_iterator I = broadcast.begin();
            EntitySet::const_iterator Iend = broadcast.end();
            for (; I != Iend; ++I) {
                // Calculate square distance to target
                float dist = squareDistance(fromEnt->m_location, (*I)->m_location);
                float view_factor = fromSquSize / dist;
#if 0
                if (view_factor > consts::square_sight_factor) {
                    std::cout << "Distance from " << fromEnt->getType() << " to "
                              << (*I)->getType() << " is " << dist << " which gives "
                              << fromSquSize << " / " << dist << " = "
                              << view_factor << std::endl << std::flush;
                }
#endif
                if (view_factor < consts::square_sight_factor) {
                    debug(std::cout << "Op from " << from
                                    << " cannot be seen by " << (*I)->getId()
                                    << std::endl << std::flush;);
                    continue;
                }
                op.setTo((*I)->getId());
                deliverTo(op, *I);
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
bool WorldRouter::idle(int sec, int usec)
{
    updateTime(sec, usec);
    unsigned int op_count = 0;
    Operation * op;
    while ((++op_count < 10) && ((op = getOperationFromQueue()) != NULL)) {
        Dispatching.emit(op);
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
