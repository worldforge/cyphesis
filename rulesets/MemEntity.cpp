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


#include "MemEntity.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/SolidProperty.h"
#include "rulesets/InternalProperties.h"


static const bool debug_flag = false;

MemEntity::MemEntity(const std::string & id, long intId) :
           LocatedEntity(id, intId), m_lastSeen(0.)
{
}

MemEntity::~MemEntity()
{
}

void MemEntity::externalOperation(const Operation & op, Link &)
{
}

void MemEntity::operation(const Operation &, OpVector &)
{
}

void MemEntity::destroy()
{
     // Handling re-parenting is done very similarly to Entity::destroy,
     // but is slightly different as we tolerate LOC being null.
     LocatedEntity * ent_loc = this->m_location.m_loc;
     if (ent_loc != 0) {
         // Remove deleted entity from its parents contains
         assert(ent_loc->m_contains != 0);
         ent_loc->m_contains->erase(this);
     }
     // FIXME This is required until MemMap uses parent refcounting
     this->m_location.m_loc = 0;

     if (this->m_contains != 0) {
         // Add deleted entity's children into its parents contains
         LocatedEntitySet::const_iterator K = this->m_contains->begin();
         LocatedEntitySet::const_iterator Kend = this->m_contains->end();
         for (; K != Kend; ++K) {
             LocatedEntity * child_ent = *K;
             child_ent->m_location.m_loc = ent_loc;
             // FIXME adjust pos and:
             // FIXME take account of orientation
             if (ent_loc != 0) {
                 ent_loc->m_contains->insert(child_ent);
             }
         }
     }
     m_flags |= entity_destroyed;
}

PropertyBase * MemEntity::setAttr(const std::string & name, const Atlas::Message::Element & attr)
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        I->second->set(attr);
        I->second->apply(this);
        return I->second;
    }
    //Check if the property changed is any one of those that will alter the location,
    //and if so use those. This makes sure that the m_location data is correct.
    //Note that we can't use the PropertyManager for this, since it's a singleton for the whole
    //system. If MemEntity was completely decoupled from Entity it would be possible though.
    PropertyBase* prop;
    if (name == "bbox") {
        prop = new BBoxProperty();
    } else if (name == "solid") {
        prop = new SolidProperty();
    } else if (name == "simple") {
        prop = new SimpleProperty();
    } else {
        prop = new SoftProperty(attr);
    }
    prop->install(this, name);
    prop->set(attr);
    prop->apply(this);
    return m_properties[name] = prop;
}
