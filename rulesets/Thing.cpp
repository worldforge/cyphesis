// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Thing.h"
#include "Script.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/inheritance.h"
#include "common/BaseWorld.h"

#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Nourish.h"
#include "common/Burn.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

static const bool debug_flag = false;

Thing::Thing(const std::string & id) : Entity(id)
{
    subscribe("setup", OP_SETUP);
    subscribe("action", OP_ACTION);
    subscribe("create", OP_CREATE);
    subscribe("delete", OP_DELETE);
    subscribe("burn", OP_BURN);
    subscribe("move", OP_MOVE);
    subscribe("set", OP_SET);
    subscribe("look", OP_LOOK);
}

Thing::~Thing() { }

void Thing::SetupOperation(const Setup & op, OpVector & res)
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

    RootOperation * tick = new Tick();
    tick->setTo(getId());

    res.push_back(tick);
}

void Thing::ActionOperation(const Action & op, OpVector & res)
{
    if (m_script->Operation("action", op, res) != 0) {
        return;
    }
    RootOperation * s = new Sight();
    s->setArgs(ListType(1,op.asObject()));
    res.push_back(s);
}

void Thing::CreateOperation(const Create & op, OpVector & res)
{
    if (m_script->Operation("create", op, res) != 0) {
        return;
    }
    const ListType & args = op.getArgs();
    if (args.empty()) {
       return;
    }
    try {
        MapType ent = args.front().asMap();
        MapType::const_iterator I = ent.find("parents");
        if ((I == ent.end()) || !I->second.isList()) {
            error(op, "Entity to be created has no type", res, getId());
            return;
        }
        const ListType & parents = I->second.asList();
        if (parents.empty() || !parents.front().isString()) {
            error(op, "Entity to be created has invalid type", res, getId());
            return;
        }
        if ((ent.find("loc") == ent.end()) && (m_location.m_loc != 0)) {
            ent["loc"] = m_location.m_loc->getId();
            if (ent.find("pos") == ent.end()) {
                ent["pos"] = m_location.m_pos.toAtlas();
            }
        }
        const std::string & type = parents.front().asString();
        debug( std::cout << getId() << " creating " << type;);

        Entity * obj = m_world->addNewObject(type,ent);

        if (obj == 0) {
            error(op, "Create op failed.", res, op.getFrom());
            return;
        }

        Create c(op);
        ListType & args = c.getArgs();
        args.push_back(MapType());
        obj->addToMessage(args.front().asMap());
        RootOperation * s = new Sight();
        s->setArgs(ListType(1,c.asObject()));
        // This should no longer be required as it is now handled centrally
        // s->setRefno(op.getSerialno());
        res.push_back(s);
    }
    catch (Atlas::Message::WrongTypeException) {
        log(ERROR, "EXCEPTION: Malformed object to be created");
        error(op, "Malformed object to be created", res, getId());
        return;
    }
}

void Thing::DeleteOperation(const Delete & op, OpVector & res)
{
    if (m_script->Operation("delete", op, res) != 0) {
        return;
    }
    // The actual destruction and removal of this entity will be handled
    // by the WorldRouter
    RootOperation * s = new Sight();
    s->setArgs(ListType(1,op.asObject()));
    res.push_back(s);
}

void Thing::BurnOperation(const Burn & op, OpVector & res)
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

void Thing::MoveOperation(const Move & op, OpVector & res)
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
    Point3D oldpos = m_location.m_pos;
    const MapType & ent = args.front().asMap();
    MapType::const_iterator I = ent.find("loc");
    if ((I == ent.end()) || !I->second.isString()) {
        error(op, "Move op has no loc", res, getId());
        return;
    }
    const std::string & ref = I->second.asString();
    EntityDict::const_iterator J = m_world->getObjects().find(ref);
    if (J == m_world->getObjects().end()) {
        error(op, "Move op loc invalid", res, getId());
        return;
    }
    debug(std::cout << "{" << ref << "}" << std::endl << std::flush;);
    Entity * newref = J->second;
    if (newref == this) {
        error(op, "Attempt by entity to move into itself", res, getId());
        return;
    }
    I = ent.find("pos");
    if ((I == ent.end()) || !I->second.isList()) {
        error(op, "Move op has no pos", res, getId());
        return;
    }

    // Up until this point nothing should have changed, but the changes
    // have all now been checked for validity.

    if (m_location.m_loc != newref) {
    // Update loc
        m_location.m_loc->m_contains.erase(this);
        if (m_location.m_loc->m_contains.empty()) {
            m_location.m_loc->m_update_flags |= a_cont;
            m_location.m_loc->updated.emit();
        }
        bool was_empty = newref->m_contains.empty();
        newref->m_contains.insert(this);
        if (was_empty) {
            newref->m_update_flags |= a_cont;
            newref->updated.emit();
        }
        m_location.m_loc = newref;
        m_update_flags |= a_loc;
    }

    // Update pos
    m_location.m_pos.fromAtlas(I->second.asList());
    // FIXME Quick height hack
    m_location.m_pos.z() = m_world->constrainHeight(m_location.m_loc,
                                                    m_location.m_pos);
    m_update_flags |= a_pos;
    I = ent.find("velocity");
    if (I != ent.end()) {
        // Update velocity
        m_location.m_velocity.fromAtlas(I->second.asList());
        // Velocity is not persistent so has no flag
    }
    I = ent.find("orientation");
    if (I != ent.end()) {
        // Update orientation
        m_location.m_orientation.fromAtlas(I->second.asList());
        m_update_flags |= a_orient;
    }

    Move m(op);
    m_location.addToMessage(m.getArgs().front().asMap());

    RootOperation * s = new Sight();
    s->setArgs(ListType(1,m.asObject()));

    res.push_back(s);

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

void Thing::SetOperation(const Set & op, OpVector & res)
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
    RootOperation * s = new Sight();
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
