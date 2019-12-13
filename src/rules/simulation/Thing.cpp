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

#include "rules/simulation/PropelProperty.h"
#include "rules/Domain.h"

#include "rules/simulation/BaseWorld.h"
#include "common/debug.h"

#include "common/operations/Update.h"
#include "common/TypeNode.h"
#include "EntityProperty.h"
#include "ModeProperty.h"
#include "ModeDataProperty.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Objects::smart_dynamic_cast;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Move;
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

void Thing::DeleteOperation(const Operation& op, OpVector& res)
{
    if (m_location.m_parent == nullptr) {
        log(ERROR, String::compose("Deleting %1(%2) when it is not "
                                   "in the world.", getType(), getId()));
        assert(m_location.m_parent != nullptr);
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
    broadcast(s, res, Visibility::PUBLIC);

//    //Important to send directly before this entity is deleted, so that broadcasts gets right.
//    sendWorld(s);

    Entity::DeleteOperation(op, res);
}

void Thing::MoveOperation(const Operation& op, OpVector& res)
{
    debug_print("Thing::move_operation")

    if (m_location.m_parent == nullptr) {
        log(ERROR, String::compose("Moving %1(%2) when it is not in the world.",
                                   getType(), getId()));
        assert(m_location.m_parent != nullptr);
        return;
    }

    // Check the validity of the operation.
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        error(op, "Move has no argument", res, getId());
        return;
    }
    RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
    if (!ent.isValid() || ent->isDefaultId()) {
        error(op, "Move op arg is malformed", res, getId());
        return;
    }
    if (getId() != ent->getId()) {
        error(op, "Move op does not have correct id in argument", res, getId());
        return;
    }

    Ref<LocatedEntity> new_loc = nullptr;
    if (!ent->isDefaultLoc()) {
        const std::string& new_loc_id = ent->getLoc();
        if (new_loc_id != m_location.m_parent->getId()) {
            // If the LOC has not changed, we don't need to look it up, or do
            // any of the following checks.
            new_loc = BaseWorld::instance().getEntity(new_loc_id);
            if (new_loc == nullptr) {
                error(op, "Move op loc does not exist", res, getId());
                return;
            }
            debug_print("LOC: " << new_loc_id)
            auto test_loc = new_loc;
            for (; test_loc != nullptr; test_loc = test_loc->m_location.m_parent) {
                if (test_loc == this) {
                    error(op, "Attempt to move into itself", res, getId());
                    return;
                }
            }
            assert(new_loc != nullptr);
            assert(m_location.m_parent != new_loc);
        }

    }


    // Up until this point nothing should have changed, but the changes
    // have all now been checked for validity.

    const Location old_loc = m_location;

    // Move ops often include a mode change, so we handle it here, even
    // though it is not a special attribute for efficiency. Otherwise
    // an additional Set op would be required.
    Element attr_mode;
    if (ent->copyAttr("mode", attr_mode) == 0) {
        if (!attr_mode.isString()) {
            log(ERROR, "Non string 'mode' set in Thing::MoveOperation");
        } else {
            // Update the mode
            setAttr("mode", attr_mode);
        }
    }

    //If a Move op contains a mode_data prop it should be used.
    //It's expected that only admins should ever send a "mode_data" as Move ops (to build the world).
    //In all other cases we want to let regular Domain rules apply
    Element attr_modeData;
    if (ent->copyAttr("mode_data", attr_modeData) == 0) {
        setAttr("mode_data", attr_modeData);
    }

    //Move ops can also alter the "planted_offset" property
    Element attr_plantedOffset;
    if (ent->copyAttr("planted_offset", attr_plantedOffset) == 0) {
        setAttr("planted_offset", attr_plantedOffset);
    }

    const double& current_time = BaseWorld::instance().getTime();

    //We can only move if there's a domain
    Domain* domain = nullptr;
    if (m_location.m_parent) {
        domain = m_location.m_parent->getDomain();
    }


    if (domain) {

        WFMath::Vector<3> newPropel;
        WFMath::Vector<3> newImpulseVelocity;
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

        Element attr_propel;
        if (ent->copyAttr("propel", attr_propel) == 0) {
            auto propelProp = requirePropertyClassFixed<PropelProperty>();
            try {
                newPropel.fromAtlas(attr_propel);
                if (!newPropel.isEqualTo(propelProp->data())) {
                    propelProp->data() = newPropel;
                    // Velocity is not persistent so has no flag
                    updatedTransform = true;
                } else {
                    //Velocity wasn't changed, so we can make newPropel invalid and it won't be applied.
                    newPropel.setValid(false);
                }
            } catch (...) {
                //just ignore malformed data
            }
        }
        if (ent->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            // Update impact velocity
            if (fromStdVector(newImpulseVelocity, ent->getVelocity()) == 0) {
                if (newImpulseVelocity.isValid()) {
                    updatedTransform = true;
                }
            }
        }


        std::set<LocatedEntity*> transformedEntities;


        // Check if the location has changed
        if (new_loc) {
            // new_loc should only be non-null if the LOC specified is
            // different from the current LOC
            assert(m_location.m_parent != new_loc);
//            // Check for pickup, ie if the new LOC is the actor, and the
//            // previous LOC is the actor's LOC.
//            if (new_loc->getId() == op->getFrom() &&
//                m_location.m_parent == new_loc->m_location.m_parent) {
//
//                //Send Pickup to those entities which are currently observing
//                if (m_location.m_parent) {
//
//                    Pickup p;
//                    p->setFrom(op->getFrom());
//                    p->setTo(getId());
//
//                    Sight s;
//                    s->setArgs1(p);
//                    m_location.m_parent->broadcast(s, res, Visibility::PUBLIC);
//                }
//
//            }
//            // Check for drop, ie if the old LOC is the actor, and the
//            // new LOC is the actor's LOC.
//            if (m_location.m_parent->getId() == op->getFrom() &&
//                new_loc == m_location.m_parent->m_location.m_parent) {
//
//                Drop d;
//                d->setFrom(op->getFrom());
//                d->setTo(getId());
//                Sight s;
//                s->setArgs1(d);
//                m_location.m_parent->broadcast(s, res, Visibility::PUBLIC);
//            }

            // Update loc

            //A set of entities that were observing the entity.
            std::set<const LocatedEntity*> previousObserving;
            collectObservers(previousObserving);

            std::set<const LocatedEntity*> previousObserved;
            if (isPerceptive()) {
                std::list<LocatedEntity*> observedEntities;
                domain->getVisibleEntitiesFor(*this, observedEntities);
                previousObserved.insert(observedEntities.begin(), observedEntities.end());
                previousObserved.insert(m_location.m_parent.get());
            }

            if (updatedTransform) {
                if (newOrientation.isValid()) {
                    m_location.m_orientation = newOrientation;
                }
                if (newPos.isValid()) {
                    m_location.m_pos = newPos;
                }
            }

            changeContainer(new_loc);
            //If the entity is stackable it might have been deleted as a result of changing container. If so bail out now.
            if (isDestroyed()) {
                return;
            }
            if (!previousObserving.empty()) {
                processAppearDisappear(std::move(previousObserving), res);
            }
            auto newDomain = new_loc->getDomain();
            if (!previousObserved.empty()) {
                //Get all entities that were previously observed, but aren't any more, and send Disappearence op for them.
                previousObserved.erase(m_location.m_parent.get());
                if (newDomain) {
                    //If there's a new domain, remove all entities that we still can observe.
                    std::list<LocatedEntity*> observedEntities;
                    domain->getVisibleEntitiesFor(*this, observedEntities);

                    for (auto& nowObservedEntity : observedEntities) {
                        if (nowObservedEntity != this) {
                            previousObserved.erase(nowObservedEntity);
                        }
                    }
                }
                std::vector<Atlas::Objects::Root> disappearArgs;
                for (auto& notObservedAnyMoreEntity : previousObserved) {
                    Anonymous that_ent;
                    that_ent->setId(notObservedAnyMoreEntity->getId());
                    that_ent->setStamp(notObservedAnyMoreEntity->getSeq());
                    disappearArgs.push_back(that_ent);

                }
                if (!disappearArgs.empty()) {
                    Disappearance disappear;
                    disappear->setTo(getId());
                    disappear->setArgs(disappearArgs);
                    res.emplace_back(std::move(disappear));
                }
            }
            if (newDomain) {
                if (updatedTransform) {
                    Domain::TransformData transformData{newOrientation, newPos, newPropel, nullptr, newImpulseVelocity};
                    newDomain->applyTransform(*this, transformData, transformedEntities);
                }
            }
        } else {
            if (updatedTransform) {

                auto modeDataProp = getPropertyClassFixed<ModeDataProperty>();
                LocatedEntity* plantedOnEntity = nullptr;
                if (modeDataProp && modeDataProp->getMode() == ModeProperty::Mode::Planted) {
                    plantedOnEntity = modeDataProp->getPlantedOnData().entity.get();
                }

                Domain::TransformData transformData{newOrientation, newPos, newPropel, plantedOnEntity, newImpulseVelocity};
                domain->applyTransform(*this, transformData, transformedEntities);
            }
        }


        m_location.update(current_time);
        removeFlags(entity_clean);

        // At this point the Location data for this entity has been updated.

        Operation m = op.copy();
        RootEntity marg = smart_dynamic_cast<RootEntity>(m->getArgs().front());
        assert(marg.isValid());
        m_location.addToEntity(marg);
        {
            auto modeDataProp = getPropertyClassFixed<ModeDataProperty>();
            if (modeDataProp) {
                if (modeDataProp->hasFlags(flag_unsent)) {
                    Element modeDataElem;
                    if (modeDataProp->get(modeDataElem) == 0) {
                        marg->setAttr(ModeDataProperty::property_name, modeDataElem);
                    }
                }
            }
        }

        if (!m->hasAttrFlag(Atlas::Objects::Operation::SECONDS_FLAG)) {
            m->setSeconds(current_time);
        }

        Sight s;
        s->setArgs1(m);
        broadcast(s, res, Visibility::PUBLIC);

        //Check if there are any other transformed entities, and send move ops for those.
        //However, with this setup the Sight ops for the resting entities will be sent _before_ the Sight
        //op for this entity is sent. It's a bit backwards.
        //TODO: send resting sights after main sight
        if (transformedEntities.size() > 1) {
            for (auto& transformedEntity : transformedEntities) {
                if (transformedEntity != this) {

                    Atlas::Objects::Entity::Anonymous movedArg;
                    movedArg->setId(transformedEntity->getId());
                    transformedEntity->m_location.addToEntity(movedArg);

                    auto modeDataProp = transformedEntity->getPropertyClassFixed<ModeDataProperty>();
                    if (modeDataProp) {
                        if (modeDataProp->hasFlags(flag_unsent)) {
                            Element modeDataElem;
                            if (modeDataProp->get(modeDataElem) == 0) {
                                movedArg->setAttr(ModeDataProperty::property_name, modeDataElem);
                            }
                        }
                    }

                    Move movedOp;
                    movedOp->setArgs1(movedArg);

                    Sight sight;
                    sight->setArgs1(movedOp);
                    OpVector childRes;
                    transformedEntity->broadcast(sight, childRes, Visibility::PUBLIC);
                    for (auto& childOp : childRes) {
                        transformedEntity->sendWorld(std::move(childOp));
                    }
                }
            }
        }
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

    //If there's a serial number we should return a sight here already.
    if (!op->isDefaultSerialno() && !op->isDefaultFrom()) {
        Sight sight;
        sight->setTo(op->getFrom());
        sight->setRefno(op->getSerialno());
        sight->setArgs1(op);
        res.emplace_back(std::move(sight));
    }

    Update update;
    update->setTo(getId());
    res.push_back(std::move(update));
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
    debug_print("Generating property update")

    Anonymous set_arg;
    Anonymous set_arg_protected;
    Anonymous set_arg_private;

    bool hadChanges = false;
    bool hadPublicChanges = false;
    bool hadProtectedChanges = false;
    bool hadPrivateChanges = false;

    for (const auto& entry : m_properties) {
        auto& prop = entry.second;
        assert(prop != nullptr);
        if (prop->hasFlags(flag_unsent)) {
            debug(std::cout << "UPDATE:  " << flag_unsent << " " << entry.first
                            << std::endl << std::flush;);
            if (entry.second->hasFlags(visibility_private)) {
                prop->add(entry.first, set_arg_private);
                hadPrivateChanges = true;
            } else if (entry.second->hasFlags(visibility_protected)) {
                prop->add(entry.first, set_arg_protected);
                hadProtectedChanges = true;
            } else {
                prop->add(entry.first, set_arg);
                hadPublicChanges = true;
            }
            prop->removeFlags(flag_unsent | persistence_clean);
            hadChanges = true;
        }
    }

    if (hadChanges) {
        //Mark that entity needs to be written to storage.
        removeFlags(entity_clean);
    }

    if (hadPublicChanges) {

        set_arg->setId(getId());

        Set set;
        set->setTo(getId());
        set->setFrom(getId());
        set->setSeconds(op->getSeconds());
        set->setArgs1(set_arg);

        Sight sight;
        sight->setArgs1(set);
        broadcast(sight, res, Visibility::PUBLIC);
    }

    if (hadProtectedChanges) {
        set_arg_protected->setId(getId());

        Set set;
        set->setTo(getId());
        set->setFrom(getId());
        set->setSeconds(op->getSeconds());
        set->setArgs1(set_arg_protected);

        Sight sight;
        sight->setArgs1(set);
        broadcast(sight, res, Visibility::PROTECTED);
    }

    if (hadPrivateChanges) {
        set_arg_private->setId(getId());

        Set set;
        set->setTo(getId());
        set->setFrom(getId());
        set->setSeconds(op->getSeconds());
        set->setArgs1(set_arg_private);

        Sight sight;
        sight->setArgs1(set);
        broadcast(sight, res, Visibility::PRIVATE);
    }

    //Location changes must be communicated through a Move op.
    if (m_flags.hasFlags(entity_dirty_location)) {
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

        broadcast(s, res, Visibility::PUBLIC);
        removeFlags(entity_dirty_location);
        hadChanges = true;
    }


    //Only change sequence number and call onUpdated if something actually changed.
    if (hadChanges) {
        m_seq++;
        if (!hasFlags(entity_clean)) {
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

    //Admin entities can see all properties
    if (observingEntity.hasFlags(entity_admin)) {
        for (auto& entry : m_properties) {
            entry.second->add(entry.first, sarg);
        }
    } else if (observingEntity.getIntId() == getIntId()) {
        //Our own entity can see both public and protected, but not private properties.
        for (auto& entry : m_properties) {
            if (!entry.second->hasFlags(visibility_private)) {
                entry.second->add(entry.first, sarg);
            }
        }
    } else {
        //Other entities can only see public properties.
        for (auto& entry : m_properties) {
            if (!entry.second->hasFlags(visibility_non_public)) {
                entry.second->add(entry.first, sarg);
            }
        }
    }

    sarg->setStamp(m_seq);
    if (m_type) {
        sarg->setParent(m_type->name());
    }
    m_location.addToEntity(sarg);
    sarg->setObjtype("obj");

    s->setArgs1(sarg);

    if (m_contains != nullptr) {

        //If the observed entity has a domain, let it decide child visibility.
        //Otherwise show all children.
        const Domain* observedEntityDomain = getDomain();
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

    if (m_location.m_parent) {
        if (!m_location.m_parent->isVisibleForOtherEntity(&observingEntity)) {
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
    auto from = BaseWorld::instance().getEntity(op->getFrom());
    if (!from) {
        log(ERROR, String::compose("Look op has invalid from %1. %2", op->getFrom(), describeEntity()));
        return;
    }

    bool result = lookAtEntity(op, res, from.get());

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

void Thing::ImaginaryOperation(const Operation& op, OpVector& res)
{
    Sight s{};
    s->setArgs1(op);
    res.push_back(s);
}

void Thing::TalkOperation(const Operation& op, OpVector& res)
{
    Sound s{};
    s->setArgs1(op);
    res.push_back(s);
}

void Thing::CreateOperation(const Operation& op, OpVector& res)
{
    createNewEntity(op, res);
}
