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

OpVector Thing::SetupOperation(const Setup & op)
{

    // This is a bit of a hack that I am not entirely happy with.
    // We broadcast a sight of create of ourselves so that everything
    // nearby can see us. This is to get round the fact that the sight
    // of create broadcast by the entity that created us may have
    // been elsewhere on the map.
    RootOperation * sight = new Sight(Sight::Instantiate());
    Create c(Create::Instantiate());
    Element::ListType & args = c.getArgs();
    args.push_back(Element::MapType());
    addToObject(args.front().asMap());
    c.setTo(getId());
    c.setFrom(getId());
    Element::ListType & sargs = sight->getArgs();
    sargs.push_back(c.asObject());

    OpVector sres;
    if (m_script->Operation("setup", op, sres) != 0) {
        sres.push_back(sight);
        return sres;
    }

    OpVector res(2);

    res[0] = sight;

    RootOperation * tick = new Tick(Tick::Instantiate());
    tick->setTo(getId());

    res[1] = tick;

    return res;
}

OpVector Thing::ActionOperation(const Action & op)
{
    OpVector res;
    if (m_script->Operation("action", op, res) != 0) {
        return res;
    }
    RootOperation * s = new Sight(Sight::Instantiate());
    s->setArgs(Element::ListType(1,op.asObject()));
    return OpVector(1,s);
}

OpVector Thing::CreateOperation(const Create & op)
{
    OpVector res;
    if (m_script->Operation("create", op, res) != 0) {
        return res;
    }
    const Element::ListType & args = op.getArgs();
    if (args.empty()) {
       return OpVector();
    }
    try {
        Element::MapType ent = args.front().asMap();
        Element::MapType::const_iterator I = ent.find("parents");
        if ((I == ent.end()) || !I->second.isList()) {
            return error(op, "Entity to be created has no type", getId());
        }
        const Element::ListType & parents = I->second.asList();
        if (parents.empty()) {
            return error(op, "Entity to be create has empty type list", getId());
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
            return error(op, "Create op failed.", op.getFrom());
        }

        Create c(op);
        Element::ListType & args = c.getArgs();
        args.push_back(Element::MapType());
        obj->addToObject(args.front().asMap());
        RootOperation * s = new Sight(Sight::Instantiate());
        s->setArgs(Element::ListType(1,c.asObject()));
        // This should no longer be required as it is now handled centrally
        // s->setRefno(op.getSerialno());
        return OpVector(1,s);
    }
    catch (Atlas::Message::WrongTypeException) {
        log(ERROR, "EXCEPTION: Malformed object to be created");
        return error(op, "Malformed object to be created", getId());
    }
    return OpVector();
}

OpVector Thing::DeleteOperation(const Delete & op)
{
    OpVector res;
    if (m_script->Operation("delete", op, res) != 0) {
        return res;
    }
    // The actual destruction and removal of this entity will be handled
    // by the WorldRouter
    RootOperation * s = new Sight(Sight::Instantiate());
    s->setArgs(Element::ListType(1,op.asObject()));
    return OpVector(1,s);
}

OpVector Thing::BurnOperation(const Burn & op)
{
    OpVector res;
    if (m_script->Operation("burn", op, res) != 0) {
        return res;
    }
    if (op.getArgs().empty() || !op.getArgs().front().isMap()) {
        return error(op, "Fire op has no argument", getId());
    }
    Element::MapType::const_iterator I = m_attributes.find("burn_speed");
    if ((I == m_attributes.end()) || !I->second.isNum()) {
        return res;
    }
    double bspeed = I->second.asNum();
    const Element::MapType & fire_ent = op.getArgs().front().asMap();
    double consumed = bspeed * fire_ent.find("status")->second.asNum();

    const std::string & to = fire_ent.find("id")->second.asString();
    Element::MapType nour_ent;
    nour_ent["id"] = to;
    nour_ent["mass"] = consumed;

    Set * s = new Set(Set::Instantiate());
    s->setTo(getId());
    Element::ListType & sargs = s->getArgs();
    sargs.push_back(Element::MapType());
    Element::MapType & self_ent = sargs.back().asMap();
    self_ent["id"] = getId();
    self_ent["status"] = m_status - (consumed / m_mass);

    Nourish * n = new Nourish(Nourish::Instantiate());
    n->setTo(to);
    n->setArgs(Element::ListType(1,nour_ent));

    OpVector res2(2);
    res2[0] = s;
    res2[1] = n;
    return res2;
}

OpVector Thing::MoveOperation(const Move & op)
{
    debug( std::cout << "Thing::move_operation" << std::endl << std::flush;);
    m_seq++;
    OpVector res;
    if (m_script->Operation("move", op, res) != 0) {
        return res;
    }
    const Element::ListType & args = op.getArgs();
    if (args.empty()) {
        debug( std::cout << "ERROR: move op has no argument" << std::endl << std::flush;);
        return OpVector();
    }
    try {
        Vector3D oldpos = m_location.m_pos;
        const Element::MapType & ent = args.front().asMap();
        Element::MapType::const_iterator I = ent.find("loc");
        if ((I == ent.end()) || !I->second.isString()) {
            return error(op, "Move op has no loc", getId());
        }
        const std::string & ref = I->second.asString();
        EntityDict::const_iterator J = m_world->getObjects().find(ref);
        if (J == m_world->getObjects().end()) {
            return error(op, "Move op loc invalid", getId());
        }
        debug(std::cout << "{" << ref << "}" << std::endl << std::flush;);
        Entity * newref = J->second;
        if (newref == this) {
            return error(op, "Attempt by entity to move into itself", getId());
        }
        I = ent.find("pos");
        if ((I == ent.end()) || !I->second.isList()) {
            return error(op, "Move op has no pos", getId());
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

        RootOperation * s = new Sight(Sight::Instantiate());
        s->setArgs(Element::ListType(1,op.asObject()));
        OpVector res2(1,s);
        // I think it might be wise to send a set indicating we have changed
        // modes, but this would probably be wasteful

        // This code handles sending Appearance and Disappearance operations
        // to this entity and others to indicate if one has gained or lost
        // sight of the other because of this movement
        if (consts::enable_ranges && isPerceptive()) {
            debug(std::cout << "testing range" << std::endl;);
            EntitySet::const_iterator I = m_location.m_loc->m_contains.begin();
            Element::ListType appear, disappear;
            Element::MapType this_ent;
            this_ent["id"] = getId();
            this_ent["stamp"] = (double)m_seq;
            Element::ListType this_as_args(1,this_ent);
            for(;I != m_location.m_loc->m_contains.end(); I++) {
                const bool wasInRange = (*I)->m_location.inRange(oldpos,
                                                          consts::sight_range);
                const bool isInRange = (*I)->m_location.inRange(m_location.m_pos,
                                                          consts::sight_range);
                // Build appear and disappear lists, and send operations
                // Also so operations to (dis)appearing perceptive
                // entities saying that we are (dis)appearing
                if (wasInRange ^ isInRange) {
                    Element::MapType that_ent;
                    that_ent["id"] = (*I)->getId();
                    that_ent["stamp"] = (double)(*I)->getSeq();
                    if (wasInRange) {
                        // We are losing sight of that object
                        disappear.push_back(that_ent);
                        debug(std::cout << getId() << ": losing site of " <<(*I)->getId() << std::endl;);
                        if ((*I)->isPerceptive()) {
                            // Send operation to the entity in question so it
                            // knows it is losing sight of us.
                            Disappearance * d = new Disappearance(Disappearance::Instantiate());
                            d->setArgs(this_as_args);
                            d->setTo((*I)->getId());
                            res2.push_back(d);
                        }
                    } else /*if (isInRange)*/ {
                        // We are gaining sight of that object
                        appear.push_back(that_ent);
                        debug(std::cout << getId() << ": gaining site of " <<(*I)->getId() << std::endl;);
                        if ((*I)->isPerceptive()) {
                            // Send operation to the entity in question so it
                            // knows it is gaining sight of us.
                            Appearance * a = new Appearance(Appearance::Instantiate());
                            a->setArgs(this_as_args);
                            a->setTo((*I)->getId());
                            res2.push_back(a);
                        }
                    }
                }
            }
            if (!appear.empty()) {
                // Send an operation to ourselves with a list of entities
                // we are losing sight of
                Appearance * a = new Appearance(Appearance::Instantiate());
                a->setArgs(appear);
                a->setTo(getId());
                res2.push_back(a);
            }
            if (!disappear.empty()) {
                // Send an operation to ourselves with a list of entities
                // we are gaining sight of
                Disappearance * d = new Disappearance(Disappearance::Instantiate());
                d->setArgs(disappear);
                d->setTo(getId());
                res2.push_back(d);
            }
        }
    updated.emit();
        return res2;
    }
    catch (Atlas::Message::WrongTypeException) {
        log(ERROR, "EXCEPTION: Malformed object to be moved");
        return error(op, "Malformed object to be moved", getId());
    }
    return OpVector();
}

OpVector Thing::SetOperation(const Set & op)
{
    m_seq++;
    OpVector res;
    if (m_script->Operation("set", op, res) != 0) {
        return res;
    }
    const Element::ListType & args = op.getArgs();
    if (args.empty()) {
       return OpVector();
    }
    try {
        const Element::MapType & ent = args.front().asMap();
        Element::MapType::const_iterator I;
        for (I = ent.begin(); I != ent.end(); I++) {
            set(I->first, I->second);
        }
        RootOperation * s = new Sight(Sight::Instantiate());
        s->setArgs(Element::ListType(1,op.asObject()));
        OpVector res2(1,s);
        if (m_status < 0) {
            RootOperation * d = new Delete(Delete::Instantiate());
            Element::ListType & dargs = d->getArgs();
            dargs.push_back(Element::MapType());
            // FIXME Is it necessary to include a full description?
            addToObject(dargs.front().asMap());
            d->setTo(getId());
            res2.push_back(d);
        }
        if (m_update_flags != 0) {
            updated.emit();
        }
        return res2;
    }
    catch (Atlas::Message::WrongTypeException) {
        log(ERROR, "EXCEPTION: Malformed set operation");
        return error(op, "Malformed set operationn", getId());
    }
    return OpVector();
}
