// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#include "Thing.h"

#include "Script.h"
#include "Motion.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"

#include "common/Tick.h"
#include "common/Nourish.h"
#include "common/Update.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;

static const bool debug_flag = false;

/// \brief Constructor for physical or tangiable entities.
Thing::Thing(const std::string & id) : Entity(id)
{
    subscribe("setup", OP_SETUP);
    subscribe("action", OP_ACTION);
    subscribe("delete", OP_DELETE);
    subscribe("burn", OP_BURN);
    subscribe("move", OP_MOVE);
    subscribe("set", OP_SET);
    subscribe("look", OP_LOOK);
    subscribe("update", OP_UPDATE);

    m_motion = new Motion(*this);
}

Thing::~Thing()
{
    assert(m_motion != 0);
    delete m_motion;
}

void Thing::SetupOperation(const Operation & op, OpVector & res)
{
    Appearance * app = new Appearance();
    ListType & args = app->getArgs();
    args.push_back(MapType());
    MapType & arg = args.back().asMap();
    arg["id"] = getId();

    res.push_back(app);

    if (m_script->Operation("setup", op, res) != 0) {
        return;
    }

    Operation * tick = new Tick();
    tick->setTo(getId());

    res.push_back(tick);
}

void Thing::ActionOperation(const Operation & op, OpVector & res)
{
    if (m_script->Operation("action", op, res) != 0) {
        return;
    }
    Operation * s = new Sight();
    s->setArgs(ListType(1,op.asObject()));
    res.push_back(s);
}

void Thing::DeleteOperation(const Operation & op, OpVector & res)
{
    if (m_script->Operation("delete", op, res) != 0) {
        return;
    }
    // The actual destruction and removal of this entity will be handled
    // by the WorldRouter
    Operation * s = new Sight();
    s->setArgs(ListType(1,op.asObject()));
    res.push_back(s);
}

void Thing::BurnOperation(const Operation & op, OpVector & res)
{
    if (m_script->Operation("burn", op, res) != 0) {
        return;
    }
    if (op.getArgs().empty() || !op.getArgs().front().isMap()) {
        error(op, "Fire op has no argument", res, getId());
        return;
    }
    MapType::const_iterator I = m_attributes.find("burn_speed");
    if ((I == m_attributes.end()) || !I->second.isNum()) {
        return;
    }
    double bspeed = I->second.asNum();
    const MapType & fire_ent = op.getArgs().front().asMap();
    double consumed = bspeed * fire_ent.find("status")->second.asNum();

    const std::string & to = fire_ent.find("id")->second.asString();
    MapType nour_ent;
    nour_ent["id"] = to;
    nour_ent["mass"] = consumed;

    Set * s = new Set();
    s->setTo(getId());
    ListType & sargs = s->getArgs();
    sargs.push_back(MapType());
    MapType & self_ent = sargs.back().asMap();
    self_ent["id"] = getId();
    self_ent["status"] = m_status - (consumed / m_mass);

    res.push_back(s);

    Nourish * n = new Nourish();
    n->setTo(to);
    n->setArgs(ListType(1,nour_ent));

    res.push_back(n);
}

void Thing::MoveOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Thing::move_operation" << std::endl << std::flush;);
    m_seq++;
    if (m_script->Operation("move", op, res) != 0) {
        return;
    }
    const ListType & args = op.getArgs();
    if (args.empty()) {
        error(op, "Move has no argument", res, getId());
        return;
    }
    const MapType & ent = args.front().asMap();
    MapType::const_iterator I = ent.find("id");
    MapType::const_iterator Iend = ent.end();
    if ((I == Iend) || !I->second.isString()) {
        error(op, "Move op has no id in argument", res, getId());
        return;
    }
    I = ent.find("loc");
    if ((I == Iend) || !I->second.isString()) {
        error(op, "Move op has no loc", res, getId());
        return;
    }
    const std::string & new_loc_id = I->second.asString();
    EntityDict::const_iterator J = m_world->getEntities().find(new_loc_id);
    if (J == m_world->getEntities().end()) {
        error(op, "Move op loc invalid", res, getId());
        return;
    }
    debug(std::cout << "{" << new_loc_id << "}" << std::endl << std::flush;);
    Entity * new_loc = J->second;
    Entity * test_loc = new_loc;
    for (; test_loc != 0; test_loc = test_loc->m_location.m_loc) {
        if (test_loc == this) {
            error(op, "Attempt by entity to move into itself", res, getId());
            return;
        }
    }
    I = ent.find("pos");
    if ((I == Iend) || !I->second.isList()) {
        error(op, "Move op has no pos", res, getId());
        return;
    }

    // Up until this point nothing should have changed, but the changes
    // have all now been checked for validity.

    if (m_location.m_loc != new_loc) {
        // Update loc
        m_location.m_loc->m_contains.erase(this);
        if (m_location.m_loc->m_contains.empty()) {
            m_location.m_loc->m_update_flags |= a_cont;
            m_location.m_loc->updated.emit();
        }
        bool was_empty = new_loc->m_contains.empty();
        new_loc->m_contains.insert(this);
        if (was_empty) {
            new_loc->m_update_flags |= a_cont;
            new_loc->updated.emit();
        }
        m_location.m_loc->decRef();
        m_location.m_loc = new_loc;
        m_location.m_loc->incRef();
        m_update_flags |= a_loc;
    }

    Point3D oldpos = m_location.m_pos;

    // Update pos
    m_location.m_pos.fromAtlas(I->second.asList());
    // FIXME Quick height hack
    m_location.m_pos.z() = m_world->constrainHeight(m_location.m_loc,
                                                    m_location.m_pos);
    m_update_flags |= a_pos;
    I = ent.find("velocity");
    if (I != Iend) {
        // Update velocity
        m_location.m_velocity.fromAtlas(I->second.asList());
        // Velocity is not persistent so has no flag
    }
    I = ent.find("orientation");
    if (I != Iend) {
        // Update orientation
        m_location.m_orientation.fromAtlas(I->second.asList());
        m_update_flags |= a_orient;
    }
    // Move ops often include a mode change, so we handle it here, even
    // though it is not a special attribute for efficiency. Otherwise
    // an additional Set op would be required.
    I = ent.find("mode");
    if (I != Iend) {
        // Update the mode
        set(I->first, I->second);
        // FIXME
        if (!I->second.isString()) {
            log(ERROR, "Non string mode set in Thing::MoveOperation");
        } else {
            m_motion->setMode(I->second.asString());
        }
    }

    // At this point the Location data for this entity has been updated.

    // Take into account terrain following etc.
    // Take into account mode also.
    // m_motion->adjustNewPostion();



    Operation m(op);
    m_location.addToMessage(m.getArgs().front().asMap());

    Operation * s = new Sight();
    s->setArgs(ListType(1,m.asObject()));

    res.push_back(s);

    if (m_location.m_velocity.isValid() &&
        m_location.m_velocity.sqrMag() > WFMATH_EPSILON) {
        // m_motion->genUpdateOperation(); ??
        Operation * u = new Update();
        u->setFutureSeconds(consts::basic_tick);
        u->setTo(getId());

        res.push_back(u);
    }

    // I think it might be wise to send a set indicating we have changed
    // modes, but this would probably be wasteful

    // This code handles sending Appearance and Disappearance operations
    // to this entity and others to indicate if one has gained or lost
    // sight of the other because of this movement
    if (consts::enable_ranges && isPerceptive()) {
        debug(std::cout << "testing range" << std::endl;);
        float fromSquSize = boxSquareSize(m_location.m_bBox);
        ListType appear, disappear;
        MapType this_ent;
        this_ent["id"] = getId();
        this_ent["stamp"] = (double)m_seq;
        ListType this_as_args(1,this_ent);
        EntitySet::const_iterator I = m_location.m_loc->m_contains.begin();
        EntitySet::const_iterator Iend = m_location.m_loc->m_contains.end();
        for(; I != Iend; ++I) {
            float oldDist = squareDistance((*I)->m_location.m_pos, oldpos),
                  newDist = squareDistance((*I)->m_location.m_pos, m_location.m_pos),
                  oSquSize = boxSquareSize((*I)->m_location.m_bBox);

            // Build appear and disappear lists, and send operations
            // Also so operations to (dis)appearing perceptive
            // entities saying that we are (dis)appearing
            if ((*I)->isPerceptive()) {
                bool wasInRange = ((fromSquSize / oldDist) > consts::square_sight_factor),
                     isInRange = ((fromSquSize / newDist) > consts::square_sight_factor);
                if (wasInRange ^ isInRange) {
                    if (wasInRange) {
                        // Send operation to the entity in question so it
                        // knows it is losing sight of us.
                        Disappearance * d = new Disappearance();
                        d->setArgs(this_as_args);
                        d->setTo((*I)->getId());
                        res.push_back(d);
                    } else /*if (isInRange)*/ {
                        // Send operation to the entity in question so it
                        // knows it is gaining sight of us.
                        // FIXME We don't need to do this, cos its about
                        // to get our Sight(Move)
                        Appearance * a = new Appearance();
                        a->setArgs(this_as_args);
                        a->setTo((*I)->getId());
                        res.push_back(a);
                    }
                }
            }
            
            bool couldSee = ((oSquSize / oldDist) > consts::square_sight_factor),
                 canSee = ((oSquSize / newDist) > consts::square_sight_factor);
            if (couldSee ^ canSee) {
                MapType that_ent;
                that_ent["id"] = (*I)->getId();
                that_ent["stamp"] = (double)(*I)->getSeq();
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
            Appearance * a = new Appearance();
            a->setArgs(appear);
            a->setTo(getId());
            res.push_back(a);
        }
        if (!disappear.empty()) {
            // Send an operation to ourselves with a list of entities
            // we are gaining sight of
            Disappearance * d = new Disappearance();
            d->setArgs(disappear);
            d->setTo(getId());
            res.push_back(d);
        }
    }
    updated.emit();
}

void Thing::SetOperation(const Operation & op, OpVector & res)
{
    m_seq++;
    if (m_script->Operation("set", op, res) != 0) {
        return;
    }
    const ListType & args = op.getArgs();
    if (args.empty() || !args.front().isMap()) {
        return;
    }
    const MapType & ent = args.front().asMap();
    merge(ent);
    Operation * s = new Sight();
    s->setArgs(ListType(1,op.asObject()));
    res.push_back(s);
    if (m_status < 0) {
        Delete * d = new Delete();
        ListType & dargs = d->getArgs();
        dargs.push_back(MapType());
        // FIXME Is it necessary to include a full description?
        addToMessage(dargs.front().asMap());
        d->setTo(getId());
        res.push_back(d);
    }
    if (m_update_flags != 0) {
        updated.emit();
    }
}

void Thing::UpdateOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Update" << std::endl << std::flush;);
    // This is where we will handle movement simulation from now on, rather
    // than in the mind interface. The details will be sorted by a new type
    // of object which will handle the specifics.
}
