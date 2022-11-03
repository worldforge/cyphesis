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
#include "rules/PhysicalProperties.h"

using Atlas::Objects::smart_dynamic_cast;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

static const bool debug_flag = false;

/// \brief Constructor for physical or tangible entities.
Thing::Thing(RouterId id) :
        Entity(std::move(id))
{
}

void Thing::DeleteOperation(const Operation& op, OpVector& res)
{
    if (m_parent == nullptr) {
        //TODO: is this really incorrect? Why shouldn't we be allowed to delete entities that have no parent?
        log(ERROR, String::compose("Deleting %1(%2) when it is not "
                                   "in the world.", getType(), getId()));
        assert(m_parent != nullptr);
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
        sToEntity->setFrom(getId());
        operation(sToEntity, res);

        Disappearance disappearanceOp;
        Anonymous anonymous;
        anonymous->setId(getId());
        anonymous->setAttr("destroyed", 1); //Add attribute clarifying that this entity is destroyed.
        disappearanceOp->setArgs1(std::move(anonymous));
        disappearanceOp->setTo(getId());
        disappearanceOp->setFrom(getId());
        operation(disappearanceOp, res);

    }

    Sight s;
    s->setArgs1(op);
    broadcast(s, res, Visibility::PUBLIC);

    Disappearance disappearanceOp;
    Anonymous anonymous;
    anonymous->setId(getId());
    anonymous->setAttr("destroyed", 1); //Add attribute clarifying that this entity is destroyed.
    disappearanceOp->setArgs1(std::move(anonymous));
    broadcast(disappearanceOp, res, Visibility::PUBLIC);


    //    //Important to send directly before this entity is deleted, so that broadcasts gets right.
//    sendWorld(s);

    Entity::DeleteOperation(op, res);
}

void Thing::MoveOperation(const Operation& op, OpVector& res)
{
    debug_print("Thing::move_operation")

//    if (m_parent == nullptr) {
//        log(ERROR, String::compose("Moving %1(%2) when it is not in the world.",
//                                   getType(), getId()));
//        assert(m_parent != nullptr);
//        return;
//    }

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
    if (getId() == ent->getId()) {
        moveOurselves(op, ent, res);
    } else {
        moveOtherEntity(op, ent, res);
    }


//    if (getId() != ent->getId()) {
//        error(op, "Move op does not have correct id in argument", res, getId());
//        return;
//    }

}

void Thing::moveToNewLocation(Ref<LocatedEntity>& new_loc,
                              OpVector& res,
                              Domain* existingDomain,
                              const Point3D& newPos,
                              const Quaternion& newOrientation,
                              const Vector3D& newImpulseVelocity)
{
    // new_loc should only be non-null if the LOC specified is
    // different from the current LOC
    assert(m_parent != new_loc.get());
//            // Check for pickup, ie if the new LOC is the actor, and the
//            // previous LOC is the actor's LOC.
//            if (new_loc->getId() == op->getFrom() &&
//                m_parent == new_loc->m_parent) {
//
//                //Send Pickup to those entities which are currently observing
//                if (m_parent) {
//
//                    Pickup p;
//                    p->setFrom(op->getFrom());
//                    p->setTo(getId());
//
//                    Sight s;
//                    s->setArgs1(p);
//                    m_parent->broadcast(s, res, Visibility::PUBLIC);
//                }
//
//            }
//            // Check for drop, ie if the old LOC is the actor, and the
//            // new LOC is the actor's LOC.
//            if (m_parent->getId() == op->getFrom() &&
//                new_loc == m_parent->m_parent) {
//
//                Drop d;
//                d->setFrom(op->getFrom());
//                d->setTo(getId());
//                Sight s;
//                s->setArgs1(d);
//                m_parent->broadcast(s, res, Visibility::PUBLIC);
//            }

    // Update loc

    //A set of entities that were observing the entity.
    std::set<const LocatedEntity*> previousObserving;
    collectObservers(previousObserving);

    std::set<const LocatedEntity*> previousObserved;
    if (existingDomain && isPerceptive()) {
        std::list<LocatedEntity*> observedEntities;
        existingDomain->getVisibleEntitiesFor(*this, observedEntities);
        previousObserved.insert(observedEntities.begin(), observedEntities.end());
        previousObserved.insert(m_parent);
        previousObserved.erase(this); // Remove ourselves.
    }

    //TODO: move this into the domain instead
    if (newOrientation.isValid()) {
        auto& prop = requirePropertyClassFixed<OrientationProperty>();
        prop.data() = newOrientation;
        applyProperty(prop);
    }
    if (newPos.isValid()) {
        auto& prop = requirePropertyClassFixed<PositionProperty>();
        prop.data() = newPos;
        applyProperty(prop);
    }

    //TODO: supply all required location data when changing location
    changeContainer(new_loc);
    processAppearDisappear(std::move(previousObserving), res);

    //If the entity is stackable it might have been deleted as a result of changing container. If so bail out now.
    if (isDestroyed()) {
        return;
    }
    m_flags.addFlags(entity_dirty_location);

    //Since the location has changed we need to issue an Update
    enqueueUpdateOp(res);


    auto newDomain = new_loc->getDomain();
    if (!previousObserved.empty()) {
        //Get all entities that were previously observed, but aren't any more, and send Disappearence op for them.
        previousObserved.erase(m_parent); //Remove the new container; we want to be able to observe it.
        if (newDomain) {
            //If there's a new domain, remove all entities that we still can observe.
            std::list<LocatedEntity*> observedEntities;
            if (existingDomain) {
                existingDomain->getVisibleEntitiesFor(*this, observedEntities);
            }

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
        if (newImpulseVelocity.isValid()) {
            Domain::TransformData transformData{{}, {}, nullptr, newImpulseVelocity};
            std::set<LocatedEntity*> transformedEntities;
            //We can ignore the transformedEntities, since we only are applying an impulse velocity
            newDomain->applyTransform(*this, transformData, transformedEntities);
        }
    }
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

    enqueueUpdateOp(res);
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
        auto& prop = entry.second.property;
        if (prop && prop->hasFlags(prop_flag_unsent)) {
            debug_print("UPDATE:  " << prop_flag_unsent << " " << entry.first)
            if (prop->hasFlags(prop_flag_visibility_private)) {
                prop->add(entry.first, set_arg_private);
                hadPrivateChanges = true;
            } else if (prop->hasFlags(prop_flag_visibility_protected)) {
                prop->add(entry.first, set_arg_protected);
                hadProtectedChanges = true;
            } else {
                prop->add(entry.first, set_arg);
                hadPublicChanges = true;
            }
            prop->removeFlags(prop_flag_unsent | prop_flag_persistence_clean);
            hadChanges = true;
        }
    }

//    //TODO: only send changed location properties
//    if (m_flags.hasFlags(entity_dirty_location)) {
//        m_location.addToEntity(set_arg);
//        removeFlags(entity_dirty_location);
//        hadChanges = true;
//        hadPublicChanges = true;
//    }

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

    //Only change sequence number and call onUpdated if something actually changed.
    if (hadChanges) {
        m_seq++;
        if (!hasFlags(entity_clean)) {
            onUpdated();
        }
    }
    removeFlags(entity_update_broadcast_queued);
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

bool Thing::lookAtEntity(const Operation& op, OpVector& res, const LocatedEntity& watcher) const
{

    if (isVisibleForOtherEntity(watcher)) {
        generateSightOp(watcher, op, res);
        return true;
    }
    return false;
}


void Thing::generateSightOp(const LocatedEntity& observingEntity, const Operation& originalLookOp, OpVector& res) const
{
    debug_print("Thing::generateSightOp() observer " << observingEntity.describeEntity() << " observed " << this->describeEntity())

    Sight s;

    Anonymous sarg;

    //If there's a domain we should let it decide "contains, and in that case other code shouldn't send the property itself.
    bool filterOutContainsProp = true;
    if (m_contains != nullptr) {

        //If the observed entity has a domain, let it decide child visibility.
        //Otherwise, show all children.
        const Domain* observedEntityDomain = getDomain();
        if (observedEntityDomain) {
            auto& contlist = sarg->modifyContains();
            contlist.clear();
            std::list<LocatedEntity*> entityList;
            observedEntityDomain->getVisibleEntitiesFor(observingEntity, entityList);
            for (auto& entity : entityList) {
                if (entity->m_parent == this) {
                    contlist.push_back(entity->getId());
                }
            }
        } else {
            filterOutContainsProp = false;
        }
    }

    //Admin entities can see all properties
    if (observingEntity.hasFlags(entity_admin)) {
        for (auto& entry : m_properties) {
            if (filterOutContainsProp && entry.first == "contains") {
                continue;
            }
            entry.second.property->add(entry.first, sarg);
        }
    } else if (observingEntity.getIntId() == getIntId()) {
        //Our own entity can see both public and protected, but not private properties.
        for (auto& entry : m_properties) {
            if (!entry.second.property->hasFlags(prop_flag_visibility_private)) {
                if (filterOutContainsProp && entry.first == "contains") {
                    continue;
                }
                entry.second.property->add(entry.first, sarg);
            }
        }
    } else {
        //Other entities can only see public properties.
        for (auto& entry : m_properties) {
            if (!entry.second.property->hasFlags(prop_flag_visibility_non_public)) {
                if (filterOutContainsProp && entry.first == "contains") {
                    continue;
                }
                entry.second.property->add(entry.first, sarg);
            }
        }
    }

    sarg->setStamp(m_seq);
    if (m_type) {
        sarg->setParent(m_type->name());
    }
//    m_location.addToEntity(sarg);
    sarg->setObjtype("obj");

    s->setArgs1(sarg);




    if (m_parent) {
        if (!m_parent->isVisibleForOtherEntity(observingEntity)) {
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
        //The entity which sent the look can have disappeared; that's completely normal.
        return;
    }

    bool result = lookAtEntity(op, res, *from);

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
    if (!op->isDefaultTo()) {
        s->setTo(op->getTo());
    }
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

void Thing::moveOurselves(const Operation& op, const RootEntity& ent, OpVector& res)
{

    Ref<LocatedEntity> new_loc = nullptr;
    if (!ent->isDefaultLoc()) {
        const std::string& new_loc_id = ent->getLoc();
        if (new_loc_id != m_parent->getId()) {
            // If the LOC has not changed, we don't need to look it up, or do
            // any of the following checks.
            new_loc = BaseWorld::instance().getEntity(new_loc_id);
            if (new_loc == nullptr) {
                error(op, "Move op loc does not exist", res, getId());
                return;
            }
            debug_print("LOC: " << new_loc_id)
            auto test_loc = new_loc;
            for (; test_loc != nullptr; test_loc = test_loc->m_parent) {
                if (test_loc == this) {
                    error(op, "Attempt to move into itself", res, getId());
                    return;
                }
            }
            assert(new_loc != nullptr);
            assert(m_parent != new_loc.get());
        }

    }


    // Up until this point nothing should have changed, but the changes
    // have all now been checked for validity.

    // Move ops often include a mode change, so we handle it here, even
    // though it is not a special attribute for efficiency. Otherwise
    // an additional Set op would be required.
    Element attr_mode;
    if (ent->copyAttr("mode", attr_mode) == 0) {
        if (!attr_mode.isString()) {
            log(ERROR, "Non string 'mode' set in Thing::MoveOperation");
        } else {
            // Update the mode
            setAttrValue("mode", attr_mode);
        }
    }

    //If a Move op contains a mode_data prop it should be used.
    //It's expected that only admins should ever send a "mode_data" as Move ops (to build the world).
    //In all other cases we want to let regular Domain rules apply
    Element attr_modeData;
    if (ent->copyAttr("mode_data", attr_modeData) == 0) {
        setAttrValue("mode_data", attr_modeData);
    }

    //Move ops can also alter the "planted_offset" property
    Element attr_plantedOffset;
    if (ent->copyAttr("planted_offset", attr_plantedOffset) == 0) {
        setAttrValue("planted_offset", attr_plantedOffset);
    }

    Element attr_propel;
    if (ent->copyAttr("_propel", attr_propel) == 0) {
        setAttrValue("_propel", attr_propel);
    }

    double current_time = BaseWorld::instance().getTimeAsSeconds();

    //We can only move if there's a domain
    Domain* domain = nullptr;
    if (m_parent) {
        domain = m_parent->getDomain();
    }

    //Send a Sight of the Move to any current observers. Do this before we might alter location.
    Operation m = op.copy();
    RootEntity marg = smart_dynamic_cast<RootEntity>(m->getArgs().front());
    assert(marg.isValid());
//        m_location.addToEntity(marg);
//        {
//            auto modeDataProp = getPropertyClassFixed<ModeDataProperty>();
//            if (modeDataProp) {
//                if (modeDataProp->hasFlags(prop_flag_unsent)) {
//                    Element modeDataElem;
//                    if (modeDataProp->get(modeDataElem) == 0) {
//                        marg->setAttrValue(ModeDataProperty::property_name, modeDataElem);
//                    }
//                }
//            }
//        }

    if (!m->hasAttrFlag(Atlas::Objects::Operation::SECONDS_FLAG)) {
        m->setSeconds(current_time);
    }

    Sight s;
    s->setArgs1(m);
    broadcast(s, res, Visibility::PUBLIC);


    WFMath::Vector<3> newImpulseVelocity;
    WFMath::Point<3> newPos;
    WFMath::Quaternion newOrientation;

    bool updatedTransform = false;
    //It only makes sense to set positional attributes if there's a domain, or we're moving to a new location
    if (domain || new_loc) {
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
            // Update impact velocity
            if (fromStdVector(newImpulseVelocity, ent->getVelocity()) == 0) {
                if (newImpulseVelocity.isValid()) {
                    updatedTransform = true;
                }
            }
        }
    }

    std::set<LocatedEntity*> transformedEntities;


    // Check if the location has changed
    if (new_loc) {
        moveToNewLocation(new_loc, res, domain, newPos, newOrientation, newImpulseVelocity);
    } else {
        if (updatedTransform && domain) {

            auto modeDataProp = getPropertyClassFixed<ModeDataProperty>();
            LocatedEntity* plantedOnEntity = nullptr;

            if (modeDataProp && modeDataProp->getMode() == ModeProperty::Mode::Planted) {
                auto& plantedOnData = modeDataProp->getPlantedOnData();
                if (plantedOnData.entityId) {
                    auto entityRef = BaseWorld::instance().getEntity(*plantedOnData.entityId);
                    if (entityRef) {
                        plantedOnEntity = entityRef.get();
                    }
                }
            }

            Domain::TransformData transformData{newOrientation, newPos, plantedOnEntity, newImpulseVelocity};
            domain->applyTransform(*this, transformData, transformedEntities);
        }
    }


//    m_location.update(current_time);
    removeFlags(entity_clean);

    // At this point the Location data for this entity has been updated.

    //TODO: handle any transformed entities inside the domain instead
//    //Check if there are any other transformed entities, and send move ops for those.
//    if (transformedEntities.size() > 1) {
//        for (auto& transformedEntity : transformedEntities) {
//            if (transformedEntity != this) {
//
//                Atlas::Objects::Entity::Anonymous setArgs;
//                setArgs->setId(transformedEntity->getId());
//                transformedEntity->m_location.addToEntity(setArgs);
//
//                auto modeDataProp = transformedEntity->getPropertyClassFixed<ModeDataProperty>();
//                if (modeDataProp) {
//                    if (modeDataProp->hasFlags(prop_flag_unsent)) {
//                        Element modeDataElem;
//                        if (modeDataProp->get(modeDataElem) == 0) {
//                            setArgs->setAttr(ModeDataProperty::property_name, modeDataElem);
//                        }
//                    }
//                }
//
//                Set setOp;
//                setOp->setArgs1(setArgs);
//
//                Sight sight;
//                sight->setArgs1(setOp);
//                transformedEntity->broadcast(sight, res, Visibility::PUBLIC);
//
//            }
//        }
//    }


    m_seq++;

    onUpdated();
}


void Thing::moveOtherEntity(const Operation& op, const RootEntity& ent, OpVector& res)
{
    auto otherEntity = BaseWorld::instance().getEntity(ent->getId());
    if (otherEntity) {

        //All movement checks needs to know if the entity is a child of us, so we'll do that check here.
        bool isChildOfUs = m_contains && m_contains->find(otherEntity) != m_contains->end();


        //Only allow movement if the entity is being moved either into, within or out of us.
        if (ent->isDefaultLoc()) {
            //The entity is being moved within this entity.
            if (isChildOfUs) {
                otherEntity->operation(op, res);
            } else {
                log(WARNING, String::compose("Entity %1 being asked to move entity %2 which is not contained by the first.", describeEntity(), otherEntity->describeEntity()));
            }
        } else {
            //Entity is either being moved into or out of us (or within us, with "loc" being set even though it's already a child).
            auto& newLoc = ent->getLoc();
            if (newLoc == getId()) {
                //Entity is either being moved within ourselves, or being moved to us.
                if (isChildOfUs) {
                    //Entity already belongs to us, just send the op on.
                    otherEntity->operation(op, res);
                } else {
                    //Entity is being moved into us.
                    //TODO: perform checks if moving into us is allowed
                    otherEntity->operation(op, res);
                }
            } else {
                //Entity is being moved to another parent, check that it's a child of us.
                if (isChildOfUs) {
                    //TODO: perform checks if moving out of us is allowed
                    auto destinationEntity = BaseWorld::instance().getEntity(ent->getLoc());
                    if (!destinationEntity) {
                        log(WARNING,
                            String::compose("Entity %1 being asked to move entity %2 to new parent with id %3, which does not exist.", describeEntity(), otherEntity->describeEntity(), ent->getLoc()));
                    } else {
                        //Send move op on to the new destination entity.
                        destinationEntity->operation(op, res);
                    }
                }
            }
        }
    }

}