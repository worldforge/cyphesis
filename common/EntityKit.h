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

#include <set>
#include <map>
#include <string>

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::map<std::string, Element> MapType;
    }
}

class LocatedEntity;
class TypeNode;

template<class T>
class ScriptKit;

/// \brief Abstract factory for creating in-game entity objects.
///
/// An Entity consists of an instance of one of a number of C++ classes
/// optionally with a script. Stores information about default attributes,
/// script language and class name.
class EntityKit {
  protected:
    EntityKit();
  public:
    ScriptKit<LocatedEntity> * m_scriptFactory;
    /// Default attribute values for this class
    Atlas::Message::MapType m_classAttributes;
    /// Default attribute values for instances of this class, including
    /// defaults inherited from parent classes.
    Atlas::Message::MapType m_attributes;
    /// Factory for class from which the class handled by this factory
    /// inherits.
    EntityKit * m_parent;
    /// Set of factories for classes which inherit from the class handled
    /// by this factory.
    std::set<EntityKit *> m_children;
    /// Inheritance type of this class.
    TypeNode * m_type;
    /// Number of times this factory has created an entity
    int m_createdCount;

    virtual ~EntityKit();

    void addProperties();

    void updateProperties();

    /// \brief Create a new Entity and make it persistent.
    ///
    /// @param id a string giving the identifier of the Entity.
    /// @param intId an integer giving the identifier of the Entity.
    /// @param pb a pointer to the persistor object for the Entity.
    virtual LocatedEntity * newEntity(const std::string & id,
                                      long intId) = 0;
    /// \brief Create a copy of this factory.
    virtual EntityKit * duplicateFactory() = 0;
};

#endif // COMMON_ENTITY_KIT_H
