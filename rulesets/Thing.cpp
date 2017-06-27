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

#include "Motion.h"
#include "PropelProperty.h"

#include "common/BaseWorld.h"
#include "common/debug.h"

#include "common/Burn.h"
#include "common/Nourish.h"
#include "common/Update.h"
#include "common/Pickup.h"
#include "common/Drop.h"
#include "common/Unseen.h"
#include "common/TypeNode.h"

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
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Pickup;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

static const bool debug_flag = false;

/// \brief Constructor for physical or tangible entities.
Thing::Thing(const std::string& id, long intId) :
    Entity(id, intId)
{
}

Thing::~Thing()
{
}

void Thing::DeleteOperation(const Operation& op, OpVector& res)
{
    if (m_location.m_loc == 0) {
        log(ERROR, String::compose("Deleting %1(%2) when it is not "
                                       "in the world.", getType(), getId()));
        assert(m_location.m_loc != 0);
        return;
    }
    // The actual destruction and removal of this entity will be handled
    // by the WorldRouter

    if (isPerceptive()) {
        //We need to send a sight operation directly to the entity.
        //The reason is that else the entity will be deleted before it can receive the broadcast Sight
        //of the Delete op, which will leave any external clients hanging.
        Sight sToEntity;
        sToEntity->setArgs1(op);
        sToEntity->setTo(getId());
        operation(sToEntity, res);
    }

    Sight s;
    s->setArgs1(op);
    broadcast(s, res);

//    //Important to send directly before this entity is deleted, so that broadcasts gets right.
//    sendWorld(s);

    Entity::DeleteOperation(op, res);
}

void Thing::MoveOperation(const Operation& op, OpVector& res)
{
    debug(std::cout << "Thing::move_operation" << std::endl << std::flush;);

    if (m_location.m_loc == 0) {
        log(ERROR, String::compose("Moving %1(%2) when it is not in the world.",
                                   getType(), getId()));
        assert(m_location.m_loc != 0);
        return;
    }

    // Check the validity of the operation.
    const std::vector<Root>& args = op->getArgs();
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

    LocatedEntity* new_loc = nullptr;
    if (ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        const std::string& new_loc_id = ent->getLoc();
        if (new_loc_id != m_location.m_loc->getId()) {
            // If the LOC has not changed, we don't need to look it up, or do
            // any of the following checks.
            new_loc = BaseWorld::instance().getEntity(new_loc_id);
            if (new_loc == nullptr) {
                error(op, "Move op loc does not exist", res, getId());
                return;
            }
            debug(std::cout << "LOC: " << new_loc_id << std::endl << std::flush;);
            LocatedEntity* test_loc = new_loc;
            for (; test_loc != nullptr; test_loc = test_loc->m_location.m_loc) {
                if (test_loc == this) {
                    error(op, "Attempt to move into itself", res, getId());
                    return;
                }
            }
            assert(new_loc != nullptr);
            assert(m_location.m_loc != new_loc);
        }

    }


    // Up until this point nothing should have changed, but the changes
    // have all now been checked for validity.

    const Location old_loc = m_location;


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
            mode = attr_mode.String();
        }
    }

    const double& current_time = BaseWorld::instance().getTime();

    //We can only move if there's a domain
    Domain* domain = nullptr;
    if (m_location.m_loc) {
        domain = m_location.m_loc->getMovementDomain();
    }


    if (domain) {

        WFMath::Vector<3> newVelocity;
        WFMath::Point<3> newPos;
        WFMath::Quaternion newOrientation;

        bool updatedTransform = false;

        if (ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
            // Update pos
            if (fromStdVector(newPos, ent->getPos()) == 0) {
                updatedTransform = true;
            }
        }
        Element attr_orientation;
        if (ent->copyAttr("orientation", attr_orientation) == 0) {
            // Update orientation
            newOrientation.fromAtlas(attr_orientation.asList());
            updatedTransform = true;
        }


        if (ent->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            // Update velocity
            if (fromStdVector(newVelocity, ent->getVelocity()) == 0) {
                auto propelProp = requirePropertyClassFixed<PropelProperty>();
                if (!newVelocity.isEqualTo(propelProp->data())) {
                    propelProp->data() = newVelocity;
                    // Velocity is not persistent so has no flag
                    updatedTransform = true;
                } else {
                    //Velocity wasn't changed, so we can make newVelocity invalid and it won't be applied.
                    newVelocity.setValid(false);
                }
            }
        }


        // Check if the location has changed
        if (new_loc != nullptr) {
            // new_loc should only be non-null if the LOC specified is
            // different from the current LOC
            assert(m_location.m_loc != new_loc);
            // Check for pickup, ie if the new LOC is the actor, and the
            // previous LOC is the actor's LOC.
            if (new_loc->getId() == op->getFrom() &&
                m_location.m_loc == new_loc->m_location.m_loc) {

                //Send Pickup to those entities which are currently observing
                if (m_location.m_loc) {

                    Pickup p;
                    p->setFrom(op->getFrom());
                    p->setTo(getId());

                    Sight s;
                    s->setArgs1(p);
                    m_location.m_loc->broadcast(s, res);
                }

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
                m_location.m_loc->broadcast(s, res);
            }

            // Update loc

            std::set<const LocatedEntity*> previousObserving;
            collectObservers(previousObserving);

            if (updatedTransform) {
                if (newOrientation.isValid()) {
                    m_location.m_orientation = newOrientation;
                }
                if (newPos.isValid()) {
                    m_location.m_pos = newPos;
                }
            }

            changeContainer(new_loc);
            processAppearDisappear(std::move(previousObserving), res);
        } else {
            if (updatedTransform) {
                domain->applyTransform(*this, newOrientation, newPos, newVelocity);
            }
        }


        m_location.update(current_time);
        resetFlags(entity_clean);

        // At this point the Location data for this entity has been updated.

        Operation m = op.copy();
        RootEntity marg = smart_dynamic_cast<RootEntity>(m->getArgs().front());
        assert(marg.isValid());
        m_location.addToEntity(marg);

        if (!m->hasAttrFlag(Atlas::Objects::Operation::SECONDS_FLAG)) {
            m->setSeconds(current_time);
        }

        Sight s;
        s->setArgs1(m);
        broadcast(s, res);

    }

    m_seq++;

    onUpdated();
}


void Thing::SetOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        error(op, "Set has no argument", res, getId());
        return;
    }
    const Root& ent = args.front();
    merge(ent->asMessage());
    Sight s;
    s->setArgs1(op);
    broadcast(s, res);
    //res.push_back(s);
    m_seq++;
    if (~m_flags & entity_clean) {
        onUpdated();
    }

    //Send an update in case there are properties that were updated as a side effect of the merge
    //TODO: only do this if there actually are changes
    Update update;
    update->setTo(getId());
    res.push_back(update);
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
void Thing::updateProperties(const Operation& op, OpVector& res)
{
    debug(std::cout << "Generating property update" << std::endl << std::flush;);

    Anonymous set_arg;
    set_arg->setId(getId());


    bool hadChanges = false;

    for (auto entry : m_properties) {
        PropertyBase* prop = entry.second;
        assert(prop != 0);
        if (prop->flags() & flag_unsent) {
            debug(std::cout << "UPDATE:  " << flag_unsent << " " << entry.first
                            << std::endl << std::flush;);

            prop->add(entry.first, set_arg);
            prop->resetFlags(flag_unsent | per_clean);

            hadChanges = true;
            // FIXME Make sure we handle separately for private properties
        }
    }

    if (hadChanges) {
        //Mark that entity needs to be written to storage.
        resetFlags(entity_clean);

        Set set;
        set->setTo(getId());
        set->setFrom(getId());
        set->setSeconds(op->getSeconds());
        set->setArgs1(set_arg);

        Sight sight;
        sight->setArgs1(set);
        broadcast(sight, res);
    }


    //Location changes must be communicated through a Move op.
    if (m_flags & entity_dirty_location) {
        Move m;
        Anonymous move_arg;
        move_arg->setId(getId());
        m_location.addToEntity(move_arg);
        m->setArgs1(move_arg);
        m->setFrom(getId());
        m->setTo(getId());
        m->setSeconds(op->getSeconds());

        Sight s;
        s->setArgs1(m);

        broadcast(s, res);
        resetFlags(entity_dirty_location);
        hadChanges = true;
    }


    //Only change sequence number and call onUpdated if something actually changed.
    if (hadChanges) {
        m_seq++;
        if (~m_flags & entity_clean) {
            onUpdated();
        }
    }
}

void Thing::UpdateOperation(const Operation& op, OpVector& res)
{
    // If it has no refno, then it is a generic request to broadcast
    // an update of some properties which have changed.
    if (op->isDefaultRefno()) {
        updateProperties(op, res);
        return;
    }

    onUpdated();
}

bool Thing::lookAtEntity(const Operation& op, OpVector& res, const LocatedEntity* watcher) const
{

    if (isVisibleForOtherEntity(watcher)) {
        generateSightOp(*watcher, op, res);
        return true;
    }
    return false;
}


void Thing::generateSightOp(const LocatedEntity& observingEntity, const Operation& originalLookOp, OpVector& res) const
{
    debug_print("Thing::generateSightOp() observer " << observingEntity.describeEntity() << " observed " << this->describeEntity());

    Sight s;

    Anonymous sarg;
    addToEntity(sarg);
    s->setArgs1(sarg);

    if (m_contains != nullptr) {

        //If the observed entity has a domain, let it decide child visibility.
        //Otherwise show all children.
        const Domain* observedEntityDomain = getMovementDomain();
        std::list<std::string>& contlist = sarg->modifyContains();
        if (observedEntityDomain) {
            contlist.clear();
            std::list<LocatedEntity*> entityList;
            observedEntityDomain->getVisibleEntitiesFor(observingEntity, entityList);
            for (auto& entity : entityList) {
                contlist.push_back(entity->getId());
            }
        }
//            if (contlist.empty()) {
//                sarg->removeAttr("contains");
//            }
    }

    if (m_location.m_loc) {
        if (!m_location.m_loc->isVisibleForOtherEntity(&observingEntity)) {
            sarg->removeAttr("loc");
        }
    }

    s->setTo(originalLookOp->getFrom());
    if (!originalLookOp->isDefaultSerialno()) {
        s->setRefno(originalLookOp->getSerialno());
    }
    res.push_back(s);

}

void Thing::LookOperation(const Operation& op, OpVector& res)
{
    LocatedEntity* from = BaseWorld::instance().getEntity(op->getFrom());
    if (from == nullptr) {
        log(ERROR, String::compose("Look op has invalid from %1. %2", op->getFrom(), describeEntity()));
        return;
    }
    // Register the entity with the world router as perceptive.
    BaseWorld::instance().addPerceptive(from);

    bool result = lookAtEntity(op, res, from);

    if (!result) {
        Unseen u;
        u->setTo(op->getFrom());
        u->setArgs(op->getArgs());
        if (!op->isDefaultSerialno()) {
            u->setRefno(op->getSerialno());
        }
        res.push_back(u);
    }
}

void Thing::CreateOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        return;
    }
    try {
        RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
        if (!ent.isValid()) {
            error(op, "Entity to be created is malformed", res, getId());
            return;
        }
        const std::list<std::string>& parents = ent->getParents();
        if (parents.empty()) {
            error(op, "Entity to be created has empty parents", res, getId());
            return;
        }

        //If there's no location set we'll use the same one as the current entity.
        if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG) && (m_location.m_loc != 0)) {
            ent->setLoc(m_location.m_loc->getId());
        }
        const std::string& type = parents.front();
        debug_print(getId() << " creating " << type);

        LocatedEntity* obj = BaseWorld::instance().addNewEntity(type, ent);

        if (obj == 0) {
            error(op, "Create op failed.", res, op->getFrom());
            return;
        }

        Anonymous new_ent;
        obj->addToEntity(new_ent);

        if (!op->isDefaultSerialno()) {
            log(NOTICE, "Sending create response");

            Info i;
            i->setArgs1(new_ent);
            i->setTo(op->getFrom());
            res.push_back(i);
        }

        Operation c(op.copy());
        c->setArgs1(new_ent);

        Sight s;
        s->setArgs1(c);
        broadcast(s, res);
        //res.push_back(s);
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "EXCEPTION: Malformed object to be created");
        error(op, "Malformed object to be created", res, getId());
        return;
    }
}
