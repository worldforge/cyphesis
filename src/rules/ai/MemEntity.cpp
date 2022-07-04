// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#include <rules/BBoxProperty.h>
#include <rules/ScaleProperty.h>
#include "MemEntity.h"
#include "rules/SolidProperty.h"
#include "common/TypeNode.h"
#include "common/PropertyManager.h"

static const bool debug_flag = false;

MemEntity::MemEntity(RouterId id) :
        LocatedEntity(id),
        m_lastSeen(0.),
        m_lastUpdated(0)
{
}

std::unique_ptr<PropertyBase> MemEntity::createProperty(const std::string& propertyName) const
{
//    if (propertyName == ReadPositionProperty::property_name) {
//        return std::make_unique<ReadPositionProperty>(const_cast<WFMath::Point<3>&>(m_transform.pos));
//    }
//    if (propertyName == ReadBboxProperty::property_name) {
//        return std::make_unique<ReadBboxProperty>(const_cast<WFMath::AxisBox<3>&>(m_bbox));
//    }
//    if (propertyName == ReadOrientationProperty::property_name) {
//        return std::make_unique<ReadOrientationProperty>(const_cast<WFMath::Quaternion&>(m_transform.orientation));
//    }
//    if (propertyName == ReadVelocityProperty::property_name) {
//        return std::make_unique<ReadVelocityProperty>(const_cast<MemEntity&>(*this));
//    }
//    if (propertyName == ReadAngularProperty::property_name) {
//        return std::make_unique<ReadAngularProperty>(const_cast<MemEntity&>(*this));
//    }
    return PropertyManager::instance().addProperty(propertyName);
}


void MemEntity::externalOperation(const Operation& op, Link&)
{
}

void MemEntity::operation(const Operation&, OpVector&)
{
}

void MemEntity::destroy()
{
    LocatedEntity::destroy();

    if (m_contains) {
        auto containsCopy = *m_contains;
        for (auto& child_ent : containsCopy) {
            child_ent->destroy();
        }
    }
    m_flags.addFlags(entity_destroyed);
}

