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
#include "common/Router.h"
#include "common/debug.h"
#include "common/op_switch.h"
#include "common/custom.h"
#include "common/Thought.h"
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <wfmath/atlasconv.h>

#include <iostream>

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
    if (world2mind(op)) {
        debug_print("MindsProperty::operation(" << op->getParent() << ") passed to mind");
        OpVector mres;
        for (auto& mind : m_data) {
            mind->operation(op, res);
        }
        for (auto& resOp: mres) {
            //Wrap any returning ops in thoughts and send them to our entity
            Atlas::Objects::Operation::Thought thought;
            thought->setArgs1(resOp);
            thought->setTo(ent->getId());
            ent->sendWorld(thought);
        }
    }

    return OPERATION_HANDLED;
}


/// \brief Filter operations from the world to the mind
///
/// Operations from the world are checked here to see if they are suitable
/// to send to the mind. Some operations should not go to the mind as they
/// leak information. Others are just not necessary as they provide no
/// useful information.
bool MindsProperty::world2mind(const Operation& op)
{
    auto otype = op->getClassNo();
    POLL_OP_SWITCH(op, otype, w2m)
    return false;
}


/// \brief Filter a Appearance operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mAppearanceOperation(const Operation& op)
{
    return true;
}

/// \brief Filter a Disappearance operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mDisappearanceOperation(const Operation& op)
{
    return true;
}

/// \brief Filter a Error operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mErrorOperation(const Operation& op)
{
    return true;
}

/// \brief Filter a Setup operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mSetupOperation(const Operation& op)
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
bool MindsProperty::w2mTickOperation(const Operation& op)
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
bool MindsProperty::w2mUnseenOperation(const Operation& op)
{
    return true;
}

/// \brief Filter a Sight operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mSightOperation(const Operation& op)
{
    return true;
}

/// \brief Filter a Sound operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mSoundOperation(const Operation& op)
{
    return true;
}

/// \brief Filter a Thought operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mThoughtOperation(const Operation& op)
{
    return true;
    //Only allow thoughts which are sent from the mind
//    return op->getFrom() == getId();
}

bool MindsProperty::w2mThinkOperation(const Operation& op)
{
    return true;
}

bool MindsProperty::w2mCommuneOperation(const Operation& op)
{
    return true;
}

/// \brief Filter a Touch operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mTouchOperation(const Operation& op)
{
    return true;
}

/// \brief Filter a Relay operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool MindsProperty::w2mRelayOperation(const Operation& op)
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

