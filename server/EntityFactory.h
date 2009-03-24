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

// $Id$

#ifndef SERVER_ENTITY_FACTORY_H
#define SERVER_ENTITY_FACTORY_H

#include <Atlas/Message/Element.h>

#include <set>

class Entity;
class ScriptFactory;
class TypeNode;

/// \brief Base class for for factories for creating entities
///
/// An Entity consists of an instance of one of a number of C++ classes
/// optionally with a script. Stores information about default attributes,
/// script language and class name.
class EntityKit {
  protected:
    EntityKit();
  public:
    ScriptFactory * m_scriptFactory;
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

    /// \brief Create a new Entity and make it persistent.
    ///
    /// @param id a string giving the identifier of the Entity.
    /// @param intId an integer giving the identifier of the Entity.
    /// @param pb a pointer to the persistor object for the Entity.
    virtual Entity * newEntity(const std::string & id,
                                        long intId) = 0;
    /// \brief Add anything required to the entity after it has been created.
    virtual int populate(Entity &) = 0;
    /// \brief Create a copy of this factory.
    virtual EntityKit * duplicateFactory() = 0;
};

template <class T>
class EntityFactory : public EntityKit {
  protected:
    EntityFactory(EntityFactory<T> & o);
  public:
    EntityFactory();
    virtual ~EntityFactory();

    virtual T * newEntity(const std::string & id, long intId);
    virtual int populate(Entity &);
    virtual EntityKit * duplicateFactory();
};

#endif // SERVER_ENTITY_FACTORY_H
