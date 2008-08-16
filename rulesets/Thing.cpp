// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2006 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: Thing.cpp,v 1.229 2008-08-16 23:21:07 alriddoch Exp $

#include "Thing.h"

#include "Script.h"
#include "Motion.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/compose.hpp"
#include "common/Property.h"

#include "common/Burn.h"
#include "common/Nourish.h"
#include "common/Update.h"
#include "common/Pickup.h"
#include "common/Drop.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Objects::smart_dynamic_cast;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Drop;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Pickup;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

static const bool debug_flag = false;

/// \brief Constructor for physical or tangiable entities.
Thing::Thing(const std::string & id, long intId) :
       Identified(id, intId),
       Thing_parent(id, intId)
{
    m_motion = new Motion(*this);
}

Thing::~Thing()
{
    assert(m_motion != 0);
    delete m_motion;
}

void Thing::DeleteOperation(const Operation & op, OpVector & res)
{
    if (m_location.m_loc == 0) {
        log(ERROR, String::compose("Deleting %1(%2) when it is not "
                                   "in the world.", getType(), getId()));
        assert(m_location.m_loc != 0);
        return;
    }
    // The actual destruction and removal of this entity will be handled
    // by the WorldRouter
    Sight s;
    s->setArgs1(op);
    res.push_back(s);
}

void Thing::MoveOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Thing::move_operation" << std::endl << std::flush;);

    if (m_location.m_loc == 0) {
        log(ERROR, String::compose("Moving %1(%2) when it is not in the world.",
                                   getType(), getId()));
        assert(m_location.m_loc != 0);
        return;
    }

    // Check the validity of the operation.
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Move has no argument", res, getId());
        return;
    }
    RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
    if (!ent.isValid()) {
        error(op, "Move op arg is malformed", res, getId());
        return;
    }
    if (getId() != ent->getId()) {
        error(op, "Move op does not have correct id in argument", res, getId());
        return;
    }

    if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        error(op, "Move op has no loc", res, getId());
        return;
    }
    const std::string & new_loc_id = ent->getLoc();
    Entity * new_loc = 0;
    if (new_loc_id != m_location.m_loc->getId()) {
        // If the LOC has not changed, we don't need to look it up, or do
        // any of the following checks.
        new_loc = BaseWorld::instance().getEntity(new_loc_id);
        if (new_loc == 0) {
            error(op, "Move op loc does not exist", res, getId());
            return;
        }
        debug(std::cout << "LOC: " << new_loc_id << std::endl << std::flush;);
        LocatedEntity * test_loc = new_loc;
        for (; test_loc != 0; test_loc = test_loc->m_location.m_loc) {
            if (test_loc == this) {
                error(op, "Attempt to move into itself", res, getId());
                return;
            }
        }
        assert(new_loc != 0);
        assert(m_location.m_loc != new_loc);
    }

    if (!ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
        error(op, "Move op has no pos", res, getId());
        return;
    }

    // Up until this point nothing should have changed, but the changes
    // have all now been checked for validity.

    // Check if the location has changed
    if (new_loc != 0) {
        // new_loc should only be non-null if the LOC specified is
        // different from the current LOC
        assert(m_location.m_loc != new_loc);
        // Check for pickup, ie if the new LOC is the actor, and the
        // previous LOC is the actor's LOC.
        if (new_loc->getId() == op->getFrom() &&
            m_location.m_loc == new_loc->m_location.m_loc) {

            Pickup p;
            p->setFrom(op->getFrom());
            p->setTo(getId());
            Sight s;
            s->setArgs1(p);
            res.push_back(s);

            Anonymous wield_arg;
            wield_arg->setId(getId());
            Wield w;
            w->setTo(op->getFrom());
            w->setArgs1(wield_arg);
            res.push_back(w);
        }
        // Check for drop, ie if the old LOC is the actor, and the
        // new LOC is the actor's LOC.
        if (m_location.m_loc->getId() == op->getFrom() &&
            new_loc == m_location.m_loc->m_location.m_loc) {

            Drop d;
            d->setFrom(op->getFrom());
            d->setTo(getId());
            Sight s;
            s->setArgs1(d);
            res.push_back(s);
        }

        // Update loc
        changeContainer(new_loc);
    }

    std::string mode;

    if (hasAttr("mode")) {
        Element mode_attr;
        getAttr("mode", mode_attr);
        if (mode_attr.isString()) {
            mode = mode_attr.String();
        } else {
            log(ERROR, String::compose("Mode on entity is a \"%1\" in "
                                       "Thing::MoveOperation",
                                       Element::typeName(mode_attr.getType())));
        }
    }

    // Move ops often include a mode change, so we handle it here, even
    // though it is not a special attribute for efficiency. Otherwise
    // an additional Set op would be required.
    Element attr_mode;
    if (ent->copyAttr("mode", attr_mode) == 0) {
        if (!attr_mode.isString()) {
            log(ERROR, "Non string mode set in Thing::MoveOperation");
        } else {
            // Update the mode
            setAttr("mode", attr_mode);
            m_motion->setMode(attr_mode.String());
            mode = attr_mode.String();
        }
    }

    const double & current_time = BaseWorld::instance().getTime();

    Point3D old_pos = m_location.pos();

    // Update pos
    fromStdVector(m_location.m_pos, ent->getPos());
    // FIXME Quick height hack
    m_location.m_pos.z() = BaseWorld::instance().constrainHeight(m_location.m_loc,
                                                    m_location.pos(),
                                                    mode);
    m_location.update(current_time);
    m_update_flags |= a_pos;

    if (ent->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
        // Update velocity
        fromStdVector(m_location.m_velocity, ent->getVelocity());
        // Velocity is not persistent so has no flag
    }

    Element attr_orientation;
    if (ent->copyAttr("orientation", attr_orientation) == 0) {
        // Update orientation
        m_location.m_orientation.fromAtlas(attr_orientation.asList());
        m_update_flags |= a_orient;
    }

    // At this point the Location data for this entity has been updated.

    bool moving = false;

    if (m_location.velocity().isValid() &&
        m_location.velocity().sqrMag() > WFMATH_EPSILON) {
        moving = true;
    }

    // Take into account terrain following etc.
    // Take into account mode also.
    // m_motion->adjustNewPostion();

    float update_time = consts::move_tick;

    if (moving) {
        // If we are moving, check for collisions
        update_time = m_motion->checkCollisions();

        if (m_motion->collision()) {
            if (update_time < WFMATH_EPSILON) {
                moving = m_motion->resolveCollision();
            } else {
                m_motion->m_collisionTime = current_time + update_time;
            }
        }
    }

    Operation m(op.copy());
    RootEntity marg = smart_dynamic_cast<RootEntity>(m->getArgs().front());
    assert(marg.isValid());
    m_location.addToEntity(marg);

    Sight s;
    s->setArgs1(m);

    res.push_back(s);

    // Serial number must be changed regardless of whether we will use it
    ++m_motion->serialno();

    // If we are moving, schedule an update to track the movement
    if (moving) {
        debug(std::cout << "Move Update in " << update_time << std::endl << std::flush;);

        Update u;
        u->setFutureSeconds(update_time);
        u->setTo(getId());

        u->setRefno(m_motion->serialno());

        res.push_back(u);
    }

    // This code handles sending Appearance and Disappearance operations
    // to this entity and others to indicate if one has gained or lost
    // sight of the other because of this movement

    // FIXME Why only for a perceptive moving entity? Surely other entities
    // must gain/lose sight of this entity if it's moving?
    if (isPerceptive()) {
        checkVisibility(old_pos, res);
    }
    m_seq++;

    onUpdated();
}

/// \brief Check changes in visibility of this entity
///
/// Check how this entity's position has changed since the last update
/// and how this has affected which entities it can see, and which can see
/// it. Return Appearance and Disappearance operations as required.
/// @param old_pos The coordinates of this entity before the update
/// @param res Resulting operations are returned here
void Thing::checkVisibility(const Point3D & old_pos, OpVector & res)
{
    debug(std::cout << "testing range" << std::endl;);
    float fromSquSize = m_location.squareBoxSize();
    std::vector<Root> appear, disappear;

    Anonymous this_ent;
    this_ent->setId(getId());
    this_ent->setStamp(m_seq);

    assert(m_location.m_loc != 0);
    assert(m_location.m_loc->m_contains != 0);
    LocatedEntitySet::const_iterator I = m_location.m_loc->m_contains->begin();
    LocatedEntitySet::const_iterator Iend = m_location.m_loc->m_contains->end();
    for(; I != Iend; ++I) {
        float old_dist = squareDistance((*I)->m_location.pos(), old_pos),
              new_dist = squareDistance((*I)->m_location.pos(), m_location.pos()),
              squ_size = (*I)->m_location.squareBoxSize();

        // Build appear and disappear lists, and send operations
        // Also so operations to (dis)appearing perceptive
        // entities saying that we are (dis)appearing
        // FIXME Should this be Entity *
        Entity * viewer = dynamic_cast<Entity *>(*I);
        assert(viewer != 0);
        if (viewer->isPerceptive()) {
            bool was_in_range = ((fromSquSize / old_dist) > consts::square_sight_factor),
                 is_in_range = ((fromSquSize / new_dist) > consts::square_sight_factor);
            if (was_in_range != is_in_range) {
                if (was_in_range) {
                    // Send operation to the entity in question so it
                    // knows it is losing sight of us.
                    Disappearance d;
                    d->setArgs1(this_ent);
                    d->setTo((*I)->getId());
                    res.push_back(d);
                } else /*if (is_in_range)*/ {
                    // Send operation to the entity in question so it
                    // knows it is gaining sight of us.
                    // FIXME We don't need to do this, cos its about
                    // to get our Sight(Move)
                    Appearance a;
                    a->setArgs1(this_ent);
                    a->setTo((*I)->getId());
                    res.push_back(a);
                }
            }
        }
        
        bool could_see = ((squ_size / old_dist) > consts::square_sight_factor),
             can_see = ((squ_size / new_dist) > consts::square_sight_factor);
        if (could_see ^ can_see) {
            Anonymous that_ent;
            that_ent->setId((*I)->getId());
            that_ent->setStamp((*I)->getSeq());
            if (could_see) {
                // We are losing sight of that object
                disappear.push_back(that_ent);
                debug(std::cout << getId() << ": losing site of "
                                << (*I)->getId() << std::endl;);
            } else /*if (can_see)*/ {
                // We are gaining sight of that object
                appear.push_back(that_ent);
                debug(std::cout << getId() << ": gaining site of "
                                << (*I)->getId() << std::endl;);
            }
        }
    }
    if (!appear.empty()) {
        // Send an operation to ourselves with a list of entities
        // we are losing sight of
        Appearance a;
        a->setArgs(appear);
        a->setTo(getId());
        res.push_back(a);
    }
    if (!disappear.empty()) {
        // Send an operation to ourselves with a list of entities
        // we are gaining sight of
        Disappearance d;
        d->setArgs(disappear);
        d->setTo(getId());
        res.push_back(d);
    }
}

void Thing::SetOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Set has no argument", res, getId());
        return;
    }
    const Root & ent = args.front();
    merge(ent->asMessage());
    Sight s;
    s->setArgs1(op);
    res.push_back(s);
    m_seq++;
    if (m_update_flags != 0) {
        onUpdated();
    }
}

/// \brief Generate a Sight(Set) operation giving an update on named attributes
///
/// When another operation causes the properties of an entity to be changed,
/// it can trigger propagation of this change by sending an Update operation
/// nameing the attributes or properties that need to be updated. This
/// member function handles the Operation, sending a Sight(Set) for
/// any perceptible changes, and will in future handle persisting those
/// changes. Should this also handle side effects?
/// The main reason for this up is that if other ops need to generate a
/// Set op to update attributes, there are race conditions all over the
/// place.
/// @param op Update operation that notifies of the changes.
/// @param res The result of the operation is returned here.
void Thing::updateProperties(const Operation & op, OpVector & res) const
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
       return;
    }

    RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());
    if (!arg.isValid()) {
        error(op, "Update op has malformed args", res, getId());
        return;
    }

    debug(std::cout << "Generating property update" << std::endl << std::flush;);

    Anonymous set_arg;
    set_arg->setId(getId());

    // Check if any of the hard attributes on RootEntity of changed.
    // All other hard attributes cannot be changed this way. Location related
    // attrs can only be changed by a Move or move relate update.
    // ID and PARENTS are immutable.
    if (!arg->isDefaultName()) {
        // Update the name
        // FIXME How?
    }
    if (!arg->isDefaultContains()) {
        // Update the contains
        // FIXME Really?
        // Surely better to go through the Property?
    }

    // FIXME SLOW!
    MapType attrs = arg->asMessage();
    MapType::const_iterator I = attrs.begin();
    MapType::const_iterator Iend = attrs.end();

    PropertyDict::const_iterator J;
    PropertyDict::const_iterator Jend = m_properties.end();

    for (; I != Iend; ++I) {
        const std::string & attr = I->first;
        debug(std::cout << "  " << attr << std::endl << std::flush;);

        J = m_properties.find(attr);
        if (J != Jend) {
            // Dump the property value into the Sight(Set()) arg
            J->second->add(attr, set_arg);
        } else {
            error(op, String::compose("Got update for non-existant property "
                                      "\"%1\"", attr), res, getId());
        }
    }

    Set set;
    set->setTo(getId());
    set->setFrom(getId());
    set->setSeconds(op->getSeconds());
    set->setArgs1(set_arg);

    Sight sight;
    sight->setArgs1(set);
    res.push_back(sight);
}

void Thing::UpdateOperation(const Operation & op, OpVector & res)
{
    // If it has no refno, then it is a generic request to broadcast
    // an update of some properties which have changed.
    if (op->isDefaultRefno()) {
        updateProperties(op, res);
        return;
    }

    // If LOC is null, this cannot be part of the world, or must be the
    // world itself, so should not be involved in any movement.
    if (m_location.m_loc == 0) {
        log(ERROR, String::compose("Updating %1(%2) when it is not in the world.",
                                   getType(), getId()));
        return;
    }

    // If it has a refno, then it is a movement update. If it does not
    // match the current movement serialno, then its obsolete, and can
    // be discarded.
    if (op->getRefno() != m_motion->serialno()) {
        return;
    }

    // If somehow a movement update arrives with the correct refno, but
    // we are not moving, then something has gone wrong.
    if (!m_location.velocity().isValid() ||
        m_location.velocity().sqrMag() < WFMATH_EPSILON) {
        log(ERROR, "Update got for entity not moving");
        return;
    }

    // This is where we will handle movement simulation from now on, rather
    // than in the mind interface. The details will be sorted by a new type
    // of object which will handle the specifics.

    const double & current_time = BaseWorld::instance().getTime();
    double time_diff = current_time - m_location.timeStamp();

    std::string mode;

    if (hasAttr("mode")) {
        Element mode_attr;
        getAttr("mode", mode_attr);
        if (mode_attr.isString()) {
            mode = mode_attr.String();
        } else {
            log(ERROR, String::compose("Mode on entity is a \"%1\" "
                                       "in Thing::UpdateOperation",
                                       Element::typeName(mode_attr.getType())));
        }
    }

    Point3D old_pos = m_location.pos();

    bool moving = true;

    // Check if a predicted collision is due.
    if (m_motion->collision()) {
        if (current_time >= m_motion->m_collisionTime) {
            time_diff = m_motion->m_collisionTime - m_location.timeStamp();
            // This flag signals that collision resolution is required later.
            // Whether or not we are actually moving is determined by the
            // collision resolution.
            moving = false;
        }
    }

    // Update entity position
    m_location.m_pos += (m_location.velocity() * time_diff);

    // Collision resolution has to occur after position has been updated.
    if (!moving) {
        moving = m_motion->resolveCollision();
    }

    // Adjust the position to world constraints - essentially fit
    // to the terrain height at this stage.
    m_location.m_pos.z() = BaseWorld::instance().constrainHeight(m_location.m_loc,
                                                    m_location.pos(),
                                                    mode);
    m_location.update(current_time);
    m_update_flags |= a_pos;

    float update_time = consts::move_tick;

    if (moving) {
        // If we are moving, check for collisions
        update_time = m_motion->checkCollisions();

        if (m_motion->collision()) {
            if (update_time < WFMATH_EPSILON) {
                moving = m_motion->resolveCollision();
            } else {
                m_motion->m_collisionTime = current_time + update_time;
            }
        }
    }

    Move m;
    Anonymous move_arg;
    move_arg->setId(getId());
    m_location.addToEntity(move_arg);
    m->setArgs1(move_arg);
    m->setFrom(getId());
    m->setTo(getId());

    Sight s;
    s->setArgs1(m);

    res.push_back(s);

    if (moving) {
        debug(std::cout << "New Update in " << update_time << std::endl << std::flush;);

        Update u;
        u->setFutureSeconds(update_time);
        u->setTo(getId());

        // If the update op has no serial number, we need our own
        // ref number
        if (op->isDefaultSerialno()) {
            u->setRefno(++m_motion->serialno());
        } else {
            // We should respect the serial number if it is present
            // as the core code will set the reference number
            // correctly.
            m_motion->serialno() = op->getSerialno();
        }

        res.push_back(u);
    }

    // This code handles sending Appearance and Disappearance operations
    // to this entity and others to indicate if one has gained or lost
    // sight of the other because of this movement

    // FIXME Why only for a perceptive moving entity? Surely other entities
    // must gain/lose sight of this entity if it's moving?
    if (isPerceptive()) {
        checkVisibility(old_pos, res);
    }
    onUpdated();
}

void Thing::LookOperation(const Operation & op, OpVector & res)
{
    Sight s;

    Anonymous new_ent;
    addToEntity(new_ent);
    s->setArgs1(new_ent);

    s->setTo(op->getFrom());

    res.push_back(s);
}

void Thing::CreateOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
       return;
    }
    try {
        RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
        if (!ent.isValid()) {
            error(op, "Entity to be created is malformed", res, getId());
            return;
        }
        const std::list<std::string> & parents = ent->getParents();
        if (parents.empty()) {
            error(op, "Entity to be created has empty parents", res, getId());
            return;
        }
        if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG) &&
            (m_location.m_loc != 0)) {
            ent->setLoc(m_location.m_loc->getId());
            if (!ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
                ::addToEntity(m_location.pos(), ent->modifyPos());
            }
        }
        const std::string & type = parents.front();
        debug( std::cout << getId() << " creating " << type;);

        Entity * obj = BaseWorld::instance().addNewEntity(type,ent);

        if (obj == 0) {
            error(op, "Create op failed.", res, op->getFrom());
            return;
        }

        Operation c(op.copy());

        Anonymous new_ent;
        obj->addToEntity(new_ent);
        c->setArgs1(new_ent);

        Sight s;
        s->setArgs1(c);
        res.push_back(s);
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "EXCEPTION: Malformed object to be created");
        error(op, "Malformed object to be created", res, getId());
        return;
    }
}
