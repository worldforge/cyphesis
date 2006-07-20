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

#include "Thing.h"

#include "Script.h"
#include "Motion.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/compose.hpp"

#include "common/Burn.h"
#include "common/Nourish.h"
#include "common/Setup.h"
#include "common/Tick.h"
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
using Atlas::Objects::Operation::Tick;
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
Thing::Thing(const std::string & id, long intId) : Thing_parent(id, intId)
{
    m_motion = new Motion(*this);
}

Thing::~Thing()
{
    assert(m_motion != 0);
    delete m_motion;
}

void Thing::SetupOperation(const Operation & op, OpVector & res)
{
    if (m_script->operation("setup", op, res) != 0) {
        return;
    }

    Tick tick;
    tick->setTo(getId());

    res.push_back(tick);
}

void Thing::ActionOperation(const Operation & op, OpVector & res)
{
    log(ERROR, String::compose("Action::Operation called on %1(%2), but it should be abstract", getId(), getType()).c_str());
    if (m_script->operation("action", op, res) != 0) {
        return;
    }
    Sight s;
    s->setArgs1(op);
    res.push_back(s);
}

void Thing::DeleteOperation(const Operation & op, OpVector & res)
{
    if (m_script->operation("delete", op, res) != 0) {
        return;
    }
    // The actual destruction and removal of this entity will be handled
    // by the WorldRouter
    Sight s;
    s->setArgs1(op);
    res.push_back(s);
}

void Thing::BurnOperation(const Operation & op, OpVector & res)
{
    if (m_script->operation("burn", op, res) != 0) {
        return;
    }
    if (op->getArgs().empty()) {
        error(op, "Fire op has no argument", res, getId());
        return;
    }
    MapType::const_iterator I = m_attributes.find("burn_speed");
    if ((I == m_attributes.end()) || !I->second.isNum()) {
        return;
    }
    double bspeed = I->second.asNum();
    const Root & fire_ent = op->getArgs().front();
    double consumed = bspeed * fire_ent->getAttr("status").asNum();

    const std::string & to = fire_ent->getId();
    Anonymous nour_ent;
    nour_ent->setId(to);
    nour_ent->setAttr("mass", consumed);

    Set s;
    s->setTo(getId());

    Anonymous self_ent;
    self_ent->setId(getId());
    self_ent->setAttr("status", m_status - (consumed / m_mass));
    s->setArgs1(self_ent);

    res.push_back(s);

    Nourish n;
    n->setTo(to);
    n->setArgs1(nour_ent);

    res.push_back(n);
}

void Thing::MoveOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Thing::move_operation" << std::endl << std::flush;);

    // FIXME This is probably here in case the op is handled by a script.
    // Should it reall be here, or would it be better to move in after
    // the checks, and make the script responsible for doing this if it
    // needs to? See also the start of SetOperation
    m_seq++;

    if (m_script->operation("move", op, res) != 0) {
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
        new_loc = m_world->getEntity(new_loc_id);
        if (new_loc == 0) {
            error(op, "Move op loc does not exist", res, getId());
            return;
        }
        debug(std::cout << "LOC: " << new_loc_id << std::endl << std::flush;);
        Entity * test_loc = new_loc;
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
            log(ERROR, String::compose("Mode on entity is a %1 in Thing::MoveOperation", Element::typeName(mode_attr.getType())).c_str());
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

    const double & current_time = m_world->getTime();

    Point3D oldpos = m_location.pos();

    // Update pos
    fromStdVector(m_location.m_pos, ent->getPos());
    // FIXME Quick height hack
    m_location.m_pos.z() = m_world->constrainHeight(m_location.m_loc,
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
    if (isPerceptive()) {
        checkVisibility(oldpos, res);
    }
    updated.emit();
}

void Thing::checkVisibility(const Point3D & oldpos, OpVector & res)
{
    debug(std::cout << "testing range" << std::endl;);
    float fromSquSize = m_location.squareBoxSize();
    std::vector<Root> appear, disappear;

    Anonymous this_ent;
    this_ent->setId(getId());
    this_ent->setStamp(m_seq);

    EntitySet::const_iterator I = m_location.m_loc->m_contains.begin();
    EntitySet::const_iterator Iend = m_location.m_loc->m_contains.end();
    for(; I != Iend; ++I) {
        float oldDist = squareDistance((*I)->m_location.pos(), oldpos),
              newDist = squareDistance((*I)->m_location.pos(), m_location.pos()),
              oSquSize = (*I)->m_location.squareBoxSize();

        // Build appear and disappear lists, and send operations
        // Also so operations to (dis)appearing perceptive
        // entities saying that we are (dis)appearing
        if ((*I)->isPerceptive()) {
            bool wasInRange = ((fromSquSize / oldDist) > consts::square_sight_factor),
                 isInRange = ((fromSquSize / newDist) > consts::square_sight_factor);
            if (wasInRange != isInRange) {
                if (wasInRange) {
                    // Send operation to the entity in question so it
                    // knows it is losing sight of us.
                    Disappearance d;
                    d->setArgs1(this_ent);
                    d->setTo((*I)->getId());
                    res.push_back(d);
                } else /*if (isInRange)*/ {
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
        
        bool couldSee = ((oSquSize / oldDist) > consts::square_sight_factor),
             canSee = ((oSquSize / newDist) > consts::square_sight_factor);
        if (couldSee ^ canSee) {
            Anonymous that_ent;
            that_ent->setId((*I)->getId());
            that_ent->setStamp((*I)->getSeq());
            if (couldSee) {
                // We are losing sight of that object
                disappear.push_back(that_ent);
                debug(std::cout << getId() << ": losing site of "
                                << (*I)->getId() << std::endl;);
            } else /*if (canSee)*/ {
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
    m_seq++;
    if (m_script->operation("set", op, res) != 0) {
        return;
    }
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
    if (m_status < 0) {
        Delete d;
        Anonymous darg;
        darg->setId(getId());
        d->setArgs1(darg);
        d->setTo(getId());
        res.push_back(d);
    }
    if (m_update_flags != 0) {
        updated.emit();
    }
}

void Thing::UpdateOperation(const Operation & op, OpVector & res)
{
    if (op->getRefno() != m_motion->serialno()) {
        return;
    }

    if (!m_location.velocity().isValid() ||
        m_location.velocity().sqrMag() < WFMATH_EPSILON) {
        log(ERROR, "Update got for entity not moving");
        return;
    }

    // This is where we will handle movement simulation from now on, rather
    // than in the mind interface. The details will be sorted by a new type
    // of object which will handle the specifics.

    const double & current_time = m_world->getTime();
    double time_diff = current_time - m_location.timeStamp();

    std::string mode;

    if (hasAttr("mode")) {
        Element mode_attr;
        getAttr("mode", mode_attr);
        if (mode_attr.isString()) {
            mode = mode_attr.String();
        } else {
            log(ERROR, String::compose("Mode on entity is a %1 in Thing::UpdateOperation", Element::typeName(mode_attr.getType())).c_str());
        }
    }

    Point3D oldpos = m_location.pos();

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
    m_location.m_pos.z() = m_world->constrainHeight(m_location.m_loc,
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
    if (isPerceptive()) {
        checkVisibility(oldpos, res);
    }
    updated.emit();
}
