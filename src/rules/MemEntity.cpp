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

MemEntity::MemEntity(const std::string& id, long intId) :
    LocatedEntity(id, intId),
    m_lastSeen(0.)
{
}


void MemEntity::externalOperation(const Operation& op, Link&)
{
}

void MemEntity::operation(const Operation&, OpVector&)
{
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
    this->m_location.m_parent.reset();

     if (this->m_contains) {
         for (auto& child_ent : *this->m_contains) {
             //Detach all child entities from parent.
             child_ent->m_location.m_parent = nullptr;
         }
     }
    m_flags.addFlags(entity_destroyed);
}

