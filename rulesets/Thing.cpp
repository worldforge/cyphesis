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
#include "Domain.h"

#include "common/BaseWorld.h"
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
#include "common/Unseen.h"

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
Thing::Thing(const std::string & id, long intId) :
       Entity(id, intId)
{
}

Thing::~Thing()
{
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
    res.push_back(s);

    Entity::DeleteOperation(op, res);
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
    LocatedEntity * new_loc = 0;
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

    const Location old_loc = m_location;

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
            if (m_motion) {
                m_motion->setMode(attr_mode.String());
            }
            mode = attr_mode.String();
        }
    }

    const double & current_time = BaseWorld::instance().getTime();

    // Update pos
    fromStdVector(m_location.m_pos, ent->getPos());

    //We can only move if there's a domain
    Domain* domain = nullptr;
    if (m_location.m_loc) {
        domain = m_location.m_loc->getMovementDomain();
    }


    //Check if we've moved between domains. First check if the location has changed, and if so
    //check if the domain also has changed.
    if (new_loc != 0) {
        if (old_loc.m_loc != m_location.m_loc && old_loc.m_loc) {
            auto domain_old = old_loc.m_loc->getMovementDomain();
            if (domain_old && domain_old != domain) {
                //Everything that saw us at the old domain should get a disappear op.
                //We shouldn't need to send disappear ops to ourselves though, since the top
                //level entity will have changed.
                //TODO: We can't use a broadcast op here, since the broadcast will look at the
                //location of the entity when it's processed, not when it's created. We should
                //alter this so that any op that's to be broadcast instead should include
                //the location data in the op itself.
                domain_old->processDisappearanceOfEntity(*this, old_loc, res);
            }
        }
    }

    if (domain) {
        // FIXME Quick height hack
        m_location.m_pos.z() = domain->constrainHeight(m_location.m_loc,
                                                               m_location.pos(),
                                                               mode);
        m_location.update(current_time);
        m_flags &= ~(entity_pos_clean | entity_clean);

        if (ent->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            // Update velocity
            fromStdVector(m_location.m_velocity, ent->getVelocity());
            // Velocity is not persistent so has no flag
        }

        Element attr_orientation;
        if (ent->copyAttr("orientation", attr_orientation) == 0) {
            // Update orientation
            m_location.m_orientation.fromAtlas(attr_orientation.asList());
            m_flags &= ~entity_orient_clean;
        }

        // At this point the Location data for this entity has been updated.

        bool moving = false;

        if (m_location.velocity().isValid() &&
            m_location.velocity().sqrMag() > WFMath::numeric_constants<WFMath::CoordType>::epsilon()) {
            moving = true;
        }

        // Take into account terrain following etc.
        // Take into account mode also.
        // m_motion->adjustNewPostion();

        float update_time = consts::move_tick;

        if (moving) {
            //We've just started moving; create a motion instance.
            if (!m_motion) {
                m_motion = new Motion(*this);
            }


            // If we are moving, check for collisions
            update_time = m_motion->checkCollisions(*domain);

            if (m_motion->collision()) {
                if (update_time < WFMath::numeric_constants<WFMath::CoordType>::epsilon()) {
                    moving = m_motion->resolveCollision();
                } else {
                    m_motion->m_collisionTime = current_time + update_time;
                }
            }

            // Serial number must be changed regardless of whether we will use it
            ++m_motion->serialno();

            // If we are moving, schedule an update to track the movement
            debug(std::cout << "Move Update in " << update_time << std::endl << std::flush;);

            Update u;
            u->setFutureSeconds(update_time);
            u->setTo(getId());

            u->setRefno(m_motion->serialno());

            res.push_back(u);

        } else {
            if (m_motion) {
                //We moved previously, but have now stopped.

                delete m_motion;
                m_motion = nullptr;
            }
        }

        Operation m(op.copy());
        RootEntity marg = smart_dynamic_cast<RootEntity>(m->getArgs().front());
        assert(marg.isValid());
        m_location.addToEntity(marg);

        Sight s;
        s->setArgs1(m);

        res.push_back(s);


        // This code handles sending Appearance and Disappearance operations
        // to this entity and others to indicate if one has gained or lost
        // sight of the other because of this movement

        // FIXME Why only for a perceptive moving entity? Surely other entities
        // must gain/lose sight of this entity if it's moving?
        if (isPerceptive()) {
            checkVisibility(old_loc, res);
        }
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
void Thing::checkVisibility(const Location & old_loc, OpVector & res)
{
    if (m_location.m_loc) {
        auto domain = m_location.m_loc->getMovementDomain();
        if (domain) {
            domain->processVisibilityForMovedEntity(*this, old_loc, res);
        }
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
    if (~m_flags & entity_clean) {
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
void Thing::updateProperties(const Operation & op, OpVector & res)
{
    debug(std::cout << "Generating property update" << std::endl << std::flush;);

    Anonymous set_arg;
    set_arg->setId(getId());

    PropertyDict::const_iterator J = m_properties.begin();
    PropertyDict::const_iterator Jend = m_properties.end();

    for (; J != Jend; ++J) {
        PropertyBase * prop = J->second;
        assert(prop != 0);
        if (prop->flags() & flag_unsent) {
            debug(std::cout << "UPDATE:  " << flag_unsent << " " << J->first
                            << std::endl << std::flush;);

            prop->add(J->first, set_arg);
            prop->resetFlags(flag_unsent | per_clean);
            resetFlags(entity_clean);
            // FIXME Make sure we handle separately for private properties
        }
    }

    m_seq++;
    if (~m_flags & entity_clean) {
        onUpdated();
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
    if (m_motion == nullptr || op->getRefno() != m_motion->serialno()) {
        return;
    }

    // If somehow a movement update arrives with the correct refno, but
    // we are not moving, then something has gone wrong.
    if (!m_location.velocity().isValid() ||
        m_location.velocity().sqrMag() < WFMath::numeric_constants<WFMath::CoordType>::epsilon()) {
        log(ERROR, "Update got for entity not moving. " + describeEntity());
        return;
    }

    // This is where we will handle movement simulation from now on, rather
    // than in the mind interface. The details will be sorted by a new type
    // of object which will handle the specifics.

    const double & current_time = BaseWorld::instance().getTime();
    float time_diff = (float)(current_time - m_location.timeStamp());

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

    const Location old_loc = m_location;

    bool moving = true;

    // Check if a predicted collision is due.
    if (m_motion->collision()) {
        if (current_time >= m_motion->m_collisionTime) {
            time_diff = (float)(m_motion->m_collisionTime - m_location.timeStamp());
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

    Domain* domain = nullptr;
    // Adjust the position to world constraints - essentially fit
    // to the terrain height at this stage.
    // FIXME Get the constraints from the movement domain
    if (m_location.m_loc) {
        domain = m_location.m_loc->getMovementDomain();
        if (domain) {
            m_location.m_pos.z() = domain->constrainHeight(m_location.m_loc,
                                                        m_location.pos(),
                                                        "standing");
        } else {

        }
    }
    m_location.update(current_time);
    m_flags &= ~(entity_pos_clean | entity_clean);

    float update_time = consts::move_tick;

    if (moving && domain) {
        // If we are moving, check for collisions
        update_time = m_motion->checkCollisions(*domain);

        if (m_motion->collision()) {
            if (update_time < WFMath::numeric_constants<WFMath::CoordType>::epsilon()) {
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
    m->setSeconds(current_time);

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
    } else {
        delete m_motion;
        m_motion = nullptr;
    }

    // This code handles sending Appearance and Disappearance operations
    // to this entity and others to indicate if one has gained or lost
    // sight of the other because of this movement

    // FIXME Why only for a perceptive moving entity? Surely other entities
    // must gain/lose sight of this entity if it's moving?
    if (isPerceptive()) {
        checkVisibility(old_loc, res);
    }
    onUpdated();
}

void Thing::LookOperation(const Operation & op, OpVector & res)
{
    LocatedEntity * from = BaseWorld::instance().getEntity(op->getFrom());
    if (from == nullptr) {
        log(ERROR, "Look op has invalid from." + describeEntity());
        return;
    }
    // Register the entity with the world router as perceptive.
    BaseWorld::instance().addPerceptive(from);

    //If the entity doing looking is the parent of this entity, allow it
    if (m_location.m_loc == from) {
        //TODO: generate op
        return;
    } else {
        //A common case is child entities looking at parents, so check for that
        if (from->m_location.m_loc == this) {
            //Sight is allowed
            //TODO: generate op
            return;
        } else {
            //Another common case is entities with the same parent. Check for that
            if (from->m_location.m_loc && from->m_location.m_loc == m_location.m_loc) {
                Domain* domain = getMovementDomain();
                domain->lookAtEntity(from, *this, op, res);
                return;
            }

            Domain* parentDomain = nullptr;
            LocatedEntity* parentEntityWithDomain = from->m_location.m_loc;
            while (parentEntityWithDomain != nullptr) {
                parentDomain = parentEntityWithDomain->getMovementDomain();
                if (parentDomain) {
                    break;
                }
            }

            //Now we need to walk upwards from the entity being looked at ("this") and see if any ancestor is either
            //the observing entity ("from") or the parent domain entity.
            std::vector<LocatedEntity*> toAncestors(4);
            LocatedEntity* toAncestor = this->m_location.m_loc;
            while (toAncestor != nullptr) {
                if (toAncestor == from) {
                    //The entity being looked at ("this") is a direct child of the entity doing the looking ("from").
                    return;
                } else if (toAncestor == parentEntityWithDomain) {
                    //The entity being looked at ("this") belongs to the same domain as the entity doing the looking ("from").
                    return;
                }
                toAncestors.push_back(toAncestor);
            }













            //Now we need to find a common ancestor and work with that.

            //First build a list of all ancestors of the entity being looked at
            std::vector<LocatedEntity*> toAncestors(10);
            LocatedEntity* toAncestor = m_location.m_loc;
            while (toAncestor != nullptr) {
                if (toAncestor == from) {
                    //A special case; the entity doing the looking is one of the direct ancestors.
                    //We now just have to walk through the list of toAncestors and verify that we can watch them.
                    return;
                }
                toAncestors.push_back(toAncestor);
                toAncestor = toAncestor->m_location.m_loc;
            }

            //Now walk upwards from the entity doing the looking, looking for a common ancestor
            //And checking on the way if it's allowed to see the ancestor entity
            LocatedEntity* fromAncestor = from->m_location.m_loc;
            while (fromAncestor != nullptr) {
                LocatedEntity* parent = from->m_location.m_loc;
                if (parent) {
                    Domain* domain = parent->getMovementDomain();

                    auto I = std::find(toAncestors.begin(), toAncestors.end(), parent);
                    if (I != toAncestors.end()) {
                        //Found an ancestor

                        LocatedEntity* childEntity = nullptr;
                        do {
                            I--;
                            if (I != toAncestors.begin()) {
                                childEntity = *I;
                            } else {
                                childEntity = this;
                            }
                            if (domain && !domain->isEntityVisibleFor(fromAncestor, childEntity)) {
                                return false;
                            }
                        } while (childEntity != nullptr);
                    }

                    //If the parent has a domain we stop here. If the parent is an ancestor we can go back down the toAncestors chain.
                    //But we don't allow visibility up to the parent of a domain entity.
                    if (domain) {
                        return false;
                    }
                }
                fromAncestor = parent;
            }





//            LocatedEntity* fromAncestor = &from;
//            toAncestors.push_back(toAncestor);
//            while (fromAncestor != nullptr) {
//                while (toAncestor != nullptr) {
//                    if (toAncestor == fromAncestor) {
//
//                    }
//                    toAncestor = toAncestor->m_location.m_loc;
//                    toAncestors.push_back(toAncestor);
//                }
//                toAncestor = m_location.m_loc;
//                toAncestors.clear();
//            }
        }
    }



    //Let the domain handle the Look op.
    Domain* domain = nullptr;

    //If the entity doing the looking is a direct child, use our domain.
    //Otherwise use the domain of our parent.
    if (from->m_location.m_loc == this) {
        domain = getMovementDomain();
    } else {
        if (m_location.m_loc) {
            domain = m_location.m_loc->getMovementDomain();
        }
    }
    if (domain) {
        domain->lookAtEntity(*from, *this, op, res);
    } else {
        Unseen u;
        u->setTo(op->getFrom());
        u->setArgs(op->getArgs());
        if (!op->isDefaultSerialno()) {
            u->setRefno(op->getSerialno());
        }
        res.push_back(u);
        log(WARNING, "Entity being looked at don't belong to any Domain, so sights cannot be determined. " + describeEntity());
    }
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

        LocatedEntity * obj = BaseWorld::instance().addNewEntity(type,ent);

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
        res.push_back(s);
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "EXCEPTION: Malformed object to be created");
        error(op, "Malformed object to be created", res, getId());
        return;
    }
}
