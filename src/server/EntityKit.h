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


#ifndef COMMON_ENTITY_KIT_H
#define COMMON_ENTITY_KIT_H

#include "common/TypeNode.h"

#include <Atlas/Objects/ObjectsFwd.h>

#include <set>
#include <map>
#include <string>
#include "modules/Ref.h"
#include "rules/simulation/Entity.h"

namespace Atlas {
    namespace Message {
        class Element;

        typedef std::map<std::string, Element> MapType;
    }
}

class Entity;

template<class T>
class ScriptKit;

/// \brief Abstract factory for creating in-game entity objects.
///
/// An Entity consists of an instance of one of a number of C++ classes
/// optionally with a script. Stores information about default attributes,
/// script language and class name.
class EntityKit
{
    protected:
        EntityKit() : m_type(nullptr),
                      m_createdCount(0)
        {
        }

    public:
        /// Inheritance type of this class.
        TypeNode* m_type;
        /// Number of times this factory has created an entity
        int m_createdCount;

        virtual ~EntityKit() = default;

        /// \brief Create a new Entity and make it persistent.
        ///
        /// @param id an identifier of the Entity.
        /// @param attributes custom attributes set for the new instance
        /// @param attributes the location of the entity
        virtual Ref<Entity> newEntity(RouterId id,
                                      const Atlas::Objects::Entity::RootEntity& attributes) = 0;

        virtual void addProperties(const PropertyManager& propertyManager) = 0;

        virtual void updateProperties(std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes, const PropertyManager& propertyManager) = 0;

};

#endif // COMMON_ENTITY_KIT_H
