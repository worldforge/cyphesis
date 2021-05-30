// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Erik Hjortsberg
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


#include "SuspendedProperty.h"
#include "rules/simulation/BaseWorld.h"
#include "common/custom.h"
#include "Entity.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

SuspendedProperty* SuspendedProperty::copy() const
{
    return new SuspendedProperty(*this);
}

void SuspendedProperty::apply(LocatedEntity& ent)
{
    //If this property is applied to the world entity, it's a special case.
    if (ent.getIntId() == 0) {
        BaseWorld::instance().setIsSuspended(isTrue());
    } else {
        if (!isTrue()) {
            //suspension is disabled; we should send any stored ops
            for (auto& op : m_suspendedOps) {
                BaseWorld::instance().message(op, ent);
            }
            m_suspendedOps.clear();
        }
    }
}

void SuspendedProperty::install(LocatedEntity& owner, const std::string& name)
{
    //Regard the world as a special case.
    if (owner.getIntId() != 0) {
        owner.installDelegate(Atlas::Objects::Operation::TICK_NO, name);
    }
}

void SuspendedProperty::remove(LocatedEntity& owner, const std::string& name)
{
    //Regard the world as a special case.
    if (owner.getIntId() != 0) {
        owner.removeDelegate(Atlas::Objects::Operation::TICK_NO, name);
    } else {
        if (!owner.isDestroyed()) {
            for (auto& op : m_suspendedOps) {
                BaseWorld::instance().message(op, owner);
            }
            m_suspendedOps.clear();
        }
    }
}

HandlerResult SuspendedProperty::operation(LocatedEntity& e,
                                           const Operation& op, OpVector& res)
{
    if (isTrue()) {
        m_suspendedOps.push_back(op);
        return OPERATION_BLOCKED;
    } else {
        return OPERATION_IGNORED;
    }
}
