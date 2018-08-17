/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "MindsProperty.h"
#include "LocatedEntity.h"
#include "TransientProperty.h"
#include "UsagesProperty.h"
#include "BaseWorld.h"


#include "common/Router.h"
#include "common/debug.h"
#include "common/op_switch.h"
#include "common/custom.h"

#include "common/Actuate.h"
#include "common/Attack.h"
#include "common/Eat.h"
#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Unseen.h"
#include "common/Update.h"
#include "common/Think.h"
#include "common/Thought.h"

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <wfmath/atlasconv.h>

#include <iostream>


using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

MindsProperty::MindsProperty()
    : PropertyBase(per_ephem)
{
}

MindsProperty::~MindsProperty() = default;

void MindsProperty::set(const Atlas::Message::Element& val)
{
    //Don't allow setting.
}

int MindsProperty::get(Atlas::Message::Element& val) const
{
    Atlas::Message::ListType list;
    for (auto& mind : m_data) {
        list.push_back(mind->getId());
    }
    val = list;
    return 0;
}

void MindsProperty::add(const std::string& val, Atlas::Message::MapType& map) const
{
    Atlas::Message::ListType list;
    for (auto& mind : m_data) {
        list.push_back(mind->getId());
    }
    map[val] = list;
}

void MindsProperty::add(const std::string& val, const Atlas::Objects::Entity::RootEntity& ent) const
{
    Atlas::Message::ListType list;
    for (auto& mind : m_data) {
        list.push_back(mind->getId());
    }
    ent->setAttr(val, list);
}

MindsProperty* MindsProperty::copy() const
{
    //Don't copy any values
    return new MindsProperty();
}

void MindsProperty::install(LocatedEntity* entity, const std::string& name)
{
    entity->addListener(this);
}

void MindsProperty::remove(LocatedEntity* entity, const std::string& name)
{
    entity->removeListener(this);
}


HandlerResult MindsProperty::operation(LocatedEntity* ent, const Operation& op, OpVector& res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::THOUGHT_NO) {
        return ThoughtOperation(ent, op, res);
    } else if (op->getClassNo() == Atlas::Objects::Operation::RELAY_NO) {
        return RelayOperation(ent, op, res);
    } else {
        if (world2mind(op)) {
            debug_print("MindsProperty::operation(" << op->getParent() << ") passed to mind");
            OpVector mres;
            sendToMinds(op, mres);
            for (auto& resOp: mres) {
                //Wrap any returning ops in thoughts and send them to our entity
                Atlas::Objects::Operation::Thought thought;
                thought->setArgs1(resOp);
                thought->setTo(ent->getId());
                ent->sendWorld(thought);
            }
        }
    }

    return OPERATION_HANDLED;
}

void MindsProperty::sendToMinds(const Operation& op, OpVector& res) const
{
    for (auto& mind : m_data) {
        mind->operation(op, res);
    }
}


HandlerResult MindsProperty::RelayOperation(LocatedEntity* ent, const Operation& op, OpVector& res)
{
    if (op->isDefaultTo()) {
        ent->error(op, "A relay op must have a 'to'.", res, ent->getId());
        return OPERATION_BLOCKED;
    }
    if (op->isDefaultFrom()) {
        ent->error(op, "A relay op must have a 'from'.", res, ent->getId());
        return OPERATION_BLOCKED;
    }
    if (op->isDefaultId()) {
        ent->error(op, "A relay op must have an 'id'.", res, ent->getId());
        return OPERATION_BLOCKED;
    }

    //Get the mind to which the relay is directed.
    auto mindId = op->getId();

    for (auto& mind : m_data) {
        if (mind->getId() == mindId) {
            OpVector mres;
            mind->operation(op, res);
            for (auto& resOp: mres) {
                //Wrap any returning ops in thoughts and send them to our entity
                Atlas::Objects::Operation::Thought thought;
                thought->setArgs1(resOp);
                thought->setTo(ent->getId());
                res.push_back(std::move(resOp));
            }
            return OPERATION_BLOCKED;
        }
    }

    return OPERATION_BLOCKED;
}

HandlerResult MindsProperty::ThoughtOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    for (auto& arg : op->getArgs()) {
        auto innerOp = smart_dynamic_cast<Operation>(arg);
        if (innerOp) {
            OpVector mres;
            mind2body(ent, innerOp, mres);

            // If the original op had a serial no, we assume the first consequence
            // of that is effectively the same operation.
            // FIXME Can this be guaranteed by the mind2body phase?
            if (!mres.empty() && mres.front()->isDefaultSerialno()) {
                mres.front()->setSerialno(op->getSerialno());
            }

            for (auto& resOp : mres) {
                resOp->setFrom(ent->getId());
                res.push_back(std::move(resOp));
            }
        }
    }
    return OPERATION_BLOCKED;
}


/// \brief Filter an Actuate operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindActuateOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    debug(std::cout << "Got Actuate op from mind" << std::endl << std::flush;);

    //Distance filtering etc. happens in ActuateOperation
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter a Setup operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindSetupOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    Anonymous setup_arg;
    setup_arg->setName("mind");
    op->setArgs1(setup_arg);
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter a Use operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindUseOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    debug(std::cout << "Got Use op from mind" << std::endl << std::flush;);

    //Make sure that the first contained arg is another Use operation,
    // which is then sent to the actual tool.

    auto& args = op->getArgs();
    if (args.size() < 2) {
        log(ERROR, "mindUseOperation: use op has no argument, should have two. " + ent->describeEntity());
        return;
    }
    auto toolEnt = smart_dynamic_cast<Atlas::Objects::Entity::RootEntity>(args.front());
    if (!toolEnt) {
        log(ERROR, "mindUseOperation: First arg is not an entity. " + ent->describeEntity());
        return;
    }
    if (!toolEnt->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindMoveOperation: First arg has no ID. " + ent->describeEntity());
        return;
    }

    //TODO: should we perhaps check that this only can be Action ops?
    auto innerOp = smart_dynamic_cast<Atlas::Objects::Operation::RootOperation>(args[1]);
    if (!innerOp) {
        log(ERROR, "mindUseOperation: Second arg is not an operation. " + ent->describeEntity());
        return;
    }

    Atlas::Objects::Operation::Use useOp;
    useOp->setTo(toolEnt->getId());
    useOp->setFrom(ent->getId());
    useOp->setArgs1(innerOp);

    res.push_back(useOp);
}

/// \brief Filter a Update operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindUpdateOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
}

/// \brief Filter a Wield operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindWieldOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    debug(std::cout << "Got Wield op from mind" << std::endl << std::flush;);
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter a Tick operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindTickOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    Anonymous tick_arg;
    tick_arg->setName("mind");
    op->setArgs1(tick_arg);
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter a Move operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindMoveOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    debug(std::cout << "MindsProperty::mind_move_op" << std::endl << std::flush;);
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindMoveOperation: move op has no argument. " + ent->describeEntity());
        return;
    }
    const RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());
    if (!arg.isValid()) {
        log(ERROR, "mindMoveOperation: Arg is not an entity. " + ent->describeEntity());
        return;
    }
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindMoveOperation: Arg has no ID. " + ent->describeEntity());
        return;
    }
//    Element stamina_attr;
//    if (getAttrType(STAMINA, stamina_attr, Element::TYPE_FLOAT) == 0) {
//        if (stamina_attr.Float() <= 0.f) {
//            // Character is immobilised.
//            return;
//        }
//    }
    const std::string& other_id = arg->getId();
    if (other_id != ent->getId()) {
        debug(std::cout << "Moving something else. " << other_id << std::endl << std::flush;);
        auto other = BaseWorld::instance().getEntity(other_id);
        if (!other) {
            Unseen u;

            Anonymous unseen_arg;
            unseen_arg->setId(other_id);
            u->setArgs1(unseen_arg);

            u->setTo(ent->getId());
            res.push_back(u);
            return;
        }

        //Check that we actually can reach the other entity.
        if (ent->canReach({other, {}})) {
            //Now also check that we can reach wherever we're trying to move the entity.

            auto targetLoc = other->m_location.m_parent;

            //Only allow some things to be set when moving another entity.
            RootEntity newArgs1;
            //We've already checked that the id exists
            newArgs1->setId(arg->getId());

            if (!arg->isDefaultLoc()) {
                newArgs1->setLoc(arg->getLoc());
                targetLoc = BaseWorld::instance().getEntity(arg->getLoc());
            }
            if (!targetLoc) {
                ent->clientError(op, "Target parent entity doesn't exist.", res, op->getFrom());
                return;
            }
            WFMath::Point<3> targetPos;
            if (!arg->isDefaultPos()) {
                newArgs1->setPos(arg->getPos());
                targetPos.fromAtlas(arg->getPosAsList());
            }
            //Check that we can reach the edge of the entity if it's placed in its new location.
            if (!ent->canReach({targetLoc, targetPos}, other->m_location.radius())) {
                ent->clientError(op, "Target is too far away.", res, op->getFrom());
                return;
            }
            if (arg->hasAttr("orientation")) {
                newArgs1->setAttr("orientation", arg->getAttr("orientation"));
            }
            //Replace first arg with our sanitized arg.
            op->setArgs1(newArgs1);
            op->setFrom(ent->getId());
            op->setTo(other_id);

            res.push_back(op);

        } else {
            ent->clientError(op, "Entity is too far away.", res, op->getFrom());
        }

        //TODO: add checks for the things that we can reach, and that we can move.
        //Probably involve the domain in this.
//        Element mass;
//        if (other->getAttr(MASS, mass) != 0 || !mass.isFloat()) {
//            // FIXME Check against strength
//            // || mass.Float() > m_statistics.get("strength"));
//            debug(std::cout << "We can't move this. Just too heavy" << std::endl << std::flush
//            ;);
//            //TODO: send op back to the mind informing it that it was too heavy to move.
//            return;
//        }

        return;
    }
    std::string new_loc;
    if (arg->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        new_loc = arg->getLoc();
    } else {
        debug(std::cout << "Parent not set" << std::endl << std::flush;);
    }
    Point3D new_pos;
    Vector3D new_propel;
    Quaternion new_orientation;
    try {
        //If there's a position specified, that takes precedence (i.e. move as quickly straight to the position).
        //Note that we still look for a propel attribute, since that can be used to determine the speed of movement.
        if (arg->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
            fromStdVector(new_pos, arg->getPos());
            debug(std::cout << "pos set to " << new_pos << std::endl << std::flush;);
        }

        //First look for the "propel" attribute; if that's not there look for the legacy "velocity" attribute.
        //Note that we differ between "propel", which is how an entity propels itself forward, and "velocity"
        //which is the resulting velocity of the entity, taking all other entities as well as gravity into consideration.
        Element attr_propel;
        if (arg->copyAttr("propel", attr_propel) == 0) {
            try {
                new_propel.fromAtlas(attr_propel);
            } catch (...) {
                //just ignore malformed data
            }
        } else {
            if (arg->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
                fromStdVector(new_propel, arg->getVelocity());
                debug(std::cout << "propel set to " << new_propel << std::endl << std::flush;);
            }
        }

        Element orientation_attr;
        if (arg->copyAttr("orientation", orientation_attr) == 0) {
            new_orientation.fromAtlas(orientation_attr);
            debug(std::cout << "ori set to " << new_orientation << std::endl << std::flush;);
            if (!new_orientation.isValid()) {
                log(ERROR, "Ignoring invalid orientation from client " + ent->describeEntity() + ".");
            }
        }
    } catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "EXCEPTION: mindMoveOperation: Malformed move operation. " + ent->describeEntity());
        return;
    } catch (...) {
        log(ERROR, "EXCEPTION: mindMoveOperation: Unknown exception thrown. " + ent->describeEntity());
        return;
    }

    debug(std::cout << ":" << new_loc << ":" << ent->m_location.m_parent->getId() << ":" << std::endl << std::flush;);
    if (!new_loc.empty() && (new_loc != ent->m_location.m_parent->getId())) {
        debug(std::cout << "Changing loc" << std::endl << std::flush;);
        auto target_loc = BaseWorld::instance().getEntity(new_loc);
        if (!target_loc) {
            //TODO: what use case is this? Moving the entity to a null location?
            Unseen u;

            Anonymous unseen_arg;
            unseen_arg->setId(new_loc);
            u->setArgs1(unseen_arg);

            u->setTo(ent->getId());
            res.push_back(u);
            return;
        }

        if (new_pos.isValid()) {
            Location target(target_loc, new_pos);
            Vector3D distance = distanceTo(ent->m_location, target);
            assert(distance.isValid());
            // Convert target into our current frame of reference.
            new_pos = ent->m_location.pos() + distance;
        } else {
            log(WARNING, "mindMoveOperation: Argument changes LOC, but no POS specified. Not sure this makes any sense. " + ent->describeEntity());
        }
    }
    // Movement within current loc. Work out the speed and stuff and
    // use movement object to track movement.

    Location ret_location = ent->m_location;

    //If there's a position set, we'll use that to determine the propel value. However, we'll also check if there also was a propel value set,
    //since the magnitude of that indicates the speed to use (otherwise we'll use full speed).
    if (new_pos.isValid()) {
        if (new_propel.isValid()) {
            auto mag = new_propel.mag();
            new_propel = new_pos - ent->m_location.pos();
            new_propel.normalize();
            new_propel *= mag;
        } else {
            new_propel = new_pos - ent->m_location.pos();
            new_propel.normalize();
        }
    }
    // Set up argument for operation
    Anonymous move_arg;
    move_arg->setId(ent->getId());

    // Need to add the arguments to this op before we return it
    // direction is already a unit vector
//    if (new_pos.isValid()) {
//        m_movement.setTarget(new_pos);
//        debug(std::cout << "Target" << new_pos << std::endl << std::flush;);
//    }
    if (new_propel.isValid()) {
        auto mag = new_propel.mag();
        if (mag == 0) {
            move_arg->setAttr("propel", new_propel.toAtlas());
        } else {
            //We don't allow the mind to set any speed greater than a normalized value.
            if (mag > 1.0) {
                new_propel.normalize();
            }
            move_arg->setAttr("propel", new_propel.toAtlas());
        }
    }

    if (new_orientation.isValid()) {
        move_arg->setAttr("orientation", new_orientation.toAtlas());
    }

    // Create move operation
    Move moveOp;
    moveOp->setTo(ent->getId());
    moveOp->setSeconds(BaseWorld::instance().getTime());
    moveOp->setArgs1(move_arg);

    res.push_back(moveOp);

}

/// \brief Filter a Set operation coming from the mind
///
/// Currently any Set op is permitted. In the future this will be locked
/// down to only allow mutable things to be changed. For example, for
/// inventory items with no name can have their name set from the client.
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindSetOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    log(WARNING, "Set op from mind");
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindSetOperation: set op has no argument. " + ent->describeEntity());
        return;
    }
    const Root& arg = args.front();
    if (arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        op->setTo(arg->getId());
    } else {
        op->setTo(ent->getId());
    }
    res.push_back(op);
}

/// \brief Filter a Combine operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindCombineOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    debug(std::cout << "mindCombineOperation" << std::endl << std::flush;);
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindCombineOperation: combine op has no argument. " + ent->describeEntity());
        return;
    }
    auto I = args.begin();
    const Root& arg1 = *I;
    op->setTo(arg1->getId());
    auto Iend = args.end();
    for (; I != Iend; ++I) {
        const Root& arg = *I;
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            ent->error(op, "MindsProperty::mindCombineOp No ID.", res, ent->getId());
            return;
        }
        // FIXME Check item to be combined is in inventory
        // and then also check stackable and the same type.
    }
    res.push_back(op);
}

/// \brief Filter a Create operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindCreateOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter a Delete operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindDeleteOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter a Divide operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindDivideOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindDivideOperation: op has no argument. " + ent->describeEntity());
        return;
    }
    auto I = args.begin();
    const Root& arg1 = *I;
    if (!arg1->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        ent->error(op, "MindsProperty::mindDivideOp arg 1 has no ID.", res, ent->getId());
        return;
    }
    // FIXME Check entity to be divided is in inventory
    op->setTo(arg1->getId());
    ++I;
    auto Iend = args.end();
    for (; I != Iend; ++I) {
        const Root& arg = *I;
        if (arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            ent->error(op, "MindsProperty::mindDivideOp arg has ID.", res, ent->getId());
            return;
        }
        // Check the same type?
    }
    res.push_back(op);
}

/// \brief Filter a Imaginary operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindImaginaryOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter a Talk operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindTalkOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    debug(std::cout << "MindsProperty::mindTalkOperation" << std::endl << std::flush;);
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter a Look operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindLookOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    debug(std::cout << "Got look up from mind from [" << op->getFrom() << "] to [" << op->getTo() << "]" << std::endl << std::flush;);

    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        //If nothing is specified, send to parent, if available.
        if (ent->m_location.m_parent) {
            op->setTo(ent->m_location.m_parent->getId());
        } else {
            return;
        }
    } else {
        const Root& arg = args.front();
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            log(ERROR, ent->describeEntity() + " mindLookOperation: Op has no ID");
            return;
        }
        op->setTo(arg->getId());
    }
    debug(std::cout << "  now to [" << op->getTo() << "]" << std::endl << std::flush;);
    res.push_back(op);
}

/// \brief Filter a Eat operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindEatOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        log(ERROR, ent->describeEntity() + " mindEatOperation: Op has no ARGS");
        return;
    }
    const Root& arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, ent->describeEntity() + " mindEatOperation: Arg has no ID");
        return;
    }

    auto target = BaseWorld::instance().getEntity(arg->getId());
    if (!target) {
        log(NOTICE, ent->describeEntity() + " mindEatOperation: Target does not exist");
        return;
    }

    if (!ent->canReach({target, {}})) {
        ent->clientError(op, "Target is too far away.", res, op->getFrom());
        return;
    }
    op->setTo(arg->getId());
    res.push_back(op);
}

/// \brief Filter a GoalInfo operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindGoalInfoOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter a Touch operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindTouchOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    // Work out what is being touched.
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindTouchOperation: Op has no ARGS");
        return;
    }
    auto arg = smart_dynamic_cast<Atlas::Objects::Entity::Anonymous>(args.front());
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, ent->describeEntity() + " mindTouchOperation: Op has no ID");
        return;
    }

    WFMath::Point<3> pos;
    if (arg->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
        pos.fromAtlas(arg->getPosAsList());
    }

    auto other = BaseWorld::instance().getEntity(arg->getId());

    //Check that we actually can reach the other entity.
    if (ent->canReach({std::move(other), pos})) {
        // Pass the modified touch operation on to target.
        op->setTo(arg->getId());
        res.push_back(op);
        // Send sight of touch
        Sight s;
        s->setArgs1(op);
        res.push_back(s);
    } else {
        ent->clientError(op, "Entity is too far away.", res, op->getFrom());
    }


}

/// \brief Filter any other operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindOtherOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    log(WARNING, String::compose("Passing '%1' op from mind through to world. %2", op->getParent(), ent->describeEntity()));
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter a Thought operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindThoughtOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter a Think operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void MindsProperty::mindThinkOperation(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    op->setTo(ent->getId());
    res.push_back(op);
}

/// \brief Filter operations from the mind destined for the body.
///
/// Operations from the character's mind which is either an NPC mind,
/// or a remote client are passed in here for pre-processing and filtering
/// before they are valid to be processed as internal ops. The operation
/// may be modified and re-used so operations passed to this function have
/// their ownership passed in, and caller should not modify the operation,
/// make assumptions that it has not been modified after calling mind2body.
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void MindsProperty::mind2body(LocatedEntity* ent, const Operation& op, OpVector& res) const
{
    debug_print("MindsProperty::mind2body(" << op->getParent() << ") " << ent->describeEntity());

    if (!op->isDefaultTo()) {
        log(ERROR, String::compose("Operation \"%1\" from mind with TO set. %2", op->getParent(), ent->describeEntity()));
        return;
    }
    if (!op->isDefaultFutureSeconds() && op->getClassNo() != Atlas::Objects::Operation::TICK_NO) {
        log(ERROR, String::compose("Operation \"%1\" from mind with "
                                   "FUTURE_SECONDS set. %2", op->getParent(), ent->describeEntity()));
    }
    auto op_no = op->getClassNo();
    switch (op_no) {
        case Atlas::Objects::Operation::COMBINE_NO:
            mindCombineOperation(ent, op, res);
            break;
        case Atlas::Objects::Operation::CREATE_NO:
            mindCreateOperation(ent, op, res);
            break;
        case Atlas::Objects::Operation::DELETE_NO:
            mindDeleteOperation(ent, op, res);
            break;
        case Atlas::Objects::Operation::DIVIDE_NO:
            mindDivideOperation(ent, op, res);
            break;
        case Atlas::Objects::Operation::IMAGINARY_NO:
            mindImaginaryOperation(ent, op, res);
            break;
        case Atlas::Objects::Operation::LOOK_NO:
            mindLookOperation(ent, op, res);
            break;
        case Atlas::Objects::Operation::MOVE_NO:
            mindMoveOperation(ent, op, res);
            break;
        case Atlas::Objects::Operation::SET_NO:
            mindSetOperation(ent, op, res);
            break;
        case Atlas::Objects::Operation::TALK_NO:
            mindTalkOperation(ent, op, res);
            break;
        case Atlas::Objects::Operation::TOUCH_NO:
            mindTouchOperation(ent, op, res);
            break;
        case Atlas::Objects::Operation::USE_NO:
            mindUseOperation(ent, op, res);
            break;
        case Atlas::Objects::Operation::WIELD_NO:
            mindWieldOperation(ent, op, res);
            break;
        default:
            if (op_no == Atlas::Objects::Operation::ACTUATE_NO) {
                mindActuateOperation(ent, op, res);
            } else if (op_no == Atlas::Objects::Operation::EAT_NO) {
                mindEatOperation(ent, op, res);
            } else if (op_no == Atlas::Objects::Operation::SETUP_NO) {
                mindSetupOperation(ent, op, res);
            } else if (op_no == Atlas::Objects::Operation::TICK_NO) {
                mindTickOperation(ent, op, res);
            } else if (op_no == Atlas::Objects::Operation::UPDATE_NO) {
                mindUpdateOperation(ent, op, res);
            } else if (op_no == Atlas::Objects::Operation::THOUGHT_NO) {
                mindThoughtOperation(ent, op, res);
            } else if (op_no == Atlas::Objects::Operation::GOAL_INFO_NO) {
                mindGoalInfoOperation(ent, op, res);
            } else if (op_no == Atlas::Objects::Operation::THINK_NO) {
                mindThinkOperation(ent, op, res);
            } else {
                mindOtherOperation(ent, op, res);
            }
            break;
    }
}


/// \brief Filter operations from the world to the mind
///
/// Operations from the world are checked here to see if they are suitable
/// to send to the mind. Some operations should not go to the mind as they
/// leak information. Others are just not necessary as they provide no
/// useful information.
bool MindsProperty::world2mind(const Operation& op) const
{
    auto otype = op->getClassNo();
    POLL_OP_SWITCH(op, otype, w2m)
    return false;
}


/// \brief Filter a Appearance operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mAppearanceOperation(const Operation& op) const
{
    return true;
}

/// \brief Filter a Disappearance operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mDisappearanceOperation(const Operation& op) const
{
    return true;
}

/// \brief Filter a Error operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mErrorOperation(const Operation& op) const
{
    return true;
}

/// \brief Filter a Setup operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mSetupOperation(const Operation& op) const
{
    if (!op->getArgs().empty()) {
        if (op->getArgs().front()->getName() == "mind") {
            return true;
        }
    }
    return false;
}


/// \brief Filter a Tick operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mTickOperation(const Operation& op) const
{
    if (!op->getArgs().empty()) {
        if (op->getArgs().front()->getName() == "mind") {
            return true;
        }
    }
    return false;
}

/// \brief Filter a Unseen operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mUnseenOperation(const Operation& op) const
{
    return true;
}

/// \brief Filter a Sight operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mSightOperation(const Operation& op) const
{
    return true;
}

/// \brief Filter a Sound operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mSoundOperation(const Operation& op) const
{
    return true;
}

/// \brief Filter a Thought operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mThoughtOperation(const Operation& op) const
{
    return true;
    //Only allow thoughts which are sent from the mind
//    return op->getFrom() == ent->getId();
}

bool MindsProperty::w2mThinkOperation(const Operation& op) const
{
    return true;
}

bool MindsProperty::w2mCommuneOperation(const Operation& op) const
{
    return true;
}

/// \brief Filter a Touch operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mTouchOperation(const Operation& op) const
{
    return true;
}

/// \brief Filter a Relay operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mRelayOperation(const Operation& op) const
{
    //Relay is an internal op.
    return false;
}

void MindsProperty::addMind(Router* mind)
{
    m_data.push_back(mind);
}

void MindsProperty::removeMind(Router* mind, LocatedEntity* entity)
{
    auto I = std::find(m_data.begin(), m_data.end(), mind);
    if (I != m_data.end()) {
        m_data.erase(I);
    }

    //If there are no more minds controlling we should either remove a transient entity, or stop a moving one.
    if (m_data.empty()) {
        //If the entity is marked as "transient" we should remove it from the world once it's not controlled anymore.
        if (entity->getProperty(TransientProperty::property_name)) {
            log(INFO, "Removing entity marked as transient when mind disconnected. " + entity->describeEntity());

            Atlas::Objects::Operation::Delete delOp;
            delOp->setTo(entity->getId());
            Atlas::Objects::Entity::Anonymous anon;
            anon->setId(entity->getId());
            delOp->setArgs1(anon);

            entity->sendWorld(delOp);
        } else {
            // Send a move op stopping the current movement
            Atlas::Objects::Entity::Anonymous move_arg;
            move_arg->setId(entity->getId());
            move_arg->setAttr("propel", Vector3D::ZERO().toAtlas());
            ::addToEntity(Vector3D::ZERO(), move_arg->modifyVelocity());

            Atlas::Objects::Operation::Move move;
            move->setFrom(entity->getId());
            move->setTo(entity->getId());
            move->setArgs1(move_arg);
            entity->sendWorld(move);
        }
    }
}

const std::vector<Router*>& MindsProperty::getMinds() const
{
    return m_data;
}

