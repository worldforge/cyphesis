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


#include "EntityKit.h"

#include "common/ScriptKit.h"
#include "common/TypeNode.h"

#include <cassert>

using Atlas::Message::MapType;

EntityKit::EntityKit() : m_scriptFactory(0),
                         m_parent(0),
                         m_type(0),
                         m_createdCount(0)
{
}

EntityKit::~EntityKit()
{
    delete m_scriptFactory;
}

void EntityKit::addProperties()
{
    assert(m_type != 0);
    m_type->addProperties(m_attributes);
}

void EntityKit::updateProperties()
{
    assert(m_type != 0);
    m_type->updateProperties(m_attributes);

    std::set<EntityKit *>::const_iterator I = m_children.begin();
    std::set<EntityKit *>::const_iterator Iend = m_children.end();
    for (; I != Iend; ++I) {
        EntityKit * child_factory = *I;
        child_factory->m_attributes = m_attributes;
        MapType::const_iterator J = child_factory->m_classAttributes.begin();
        MapType::const_iterator Jend = child_factory->m_classAttributes.end();
        for (; J != Jend; ++J) {
            child_factory->m_attributes[J->first] = J->second;
        }
        child_factory->updateProperties();
    }
}
