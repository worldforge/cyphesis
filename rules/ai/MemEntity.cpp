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
#include "../SolidProperty.h"
#include "common/TypeNode.h"

static const bool debug_flag = false;

MemEntity::MemEntity(const std::string & id, long intId) :
           LocatedEntity(id, intId), m_lastSeen(0.)
{
}


void MemEntity::externalOperation(const Operation & op, Link &)
{
}

void MemEntity::operation(const Operation &, OpVector &)
{
}

const PropertyBase * MemEntity::getProperty(const std::string & name) const
{
    auto I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second;
    }
    if (m_type != nullptr) {
        I = m_type->defaults().find(name);
        if (I != m_type->defaults().end()) {
            return I->second;
        }
    }
    return nullptr;
}

void MemEntity::destroy()
{
     // Handling re-parenting is done very similarly to Entity::destroy,
     // but is slightly different as we tolerate LOC being null.
     auto ent_loc = this->m_location.m_parent;
     if (ent_loc) {
         // Remove deleted entity from its parents contains
         assert(ent_loc->m_contains != nullptr);
         ent_loc->m_contains->erase(this);
     }
     // FIXME This is required until MemMap uses parent refcounting
     this->m_location.m_parent = nullptr;

     if (this->m_contains) {
         // Add deleted entity's children into its parents contains
         for (auto& child_ent : *this->m_contains) {
             child_ent->m_location.m_parent = ent_loc;
             // FIXME adjust pos and:
             // FIXME take account of orientation
             if (ent_loc != nullptr) {
                 ent_loc->m_contains->insert(child_ent);
             }
         }
     }
     m_flags.addFlags(entity_destroyed);
}

PropertyBase * MemEntity::setAttr(const std::string & name, const Atlas::Message::Element & attr)
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        I->second->set(attr);
        I->second->apply(this);
        propertyApplied(I->first, *I->second);
        return I->second;
    }
    //Check if the property changed is any one of those that will alter the location,
    //and if so use those. This makes sure that the m_location data is correct.
    //Note that we can't use the PropertyManager for this, since it's a singleton for the whole
    //system. If MemEntity was completely decoupled from Entity it would be possible though.
    PropertyBase* prop;
    if (name == BBoxProperty::property_name) {
        prop = new BBoxProperty();
    } else if (name == SolidProperty::property_name) {
        prop = new SolidProperty();
    } else if (name == ScaleProperty::property_name) {
        prop = new ScaleProperty();
    } else {
        prop = new SoftProperty(attr);
    }
    prop->install(this, name);
    prop->set(attr);
    prop->apply(this);
    propertyApplied(name, *prop);
    return m_properties[name] = prop;
}
