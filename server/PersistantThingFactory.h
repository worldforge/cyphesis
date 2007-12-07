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

// $Id: PersistantThingFactory.h,v 1.26 2007-12-07 01:19:16 alriddoch Exp $

#ifndef SERVER_THING_FACTORY_H
#define SERVER_THING_FACTORY_H

#include <Atlas/Message/Element.h>

#include <set>

class Entity;
class ScriptFactory;
class TypeNode;

template <class T>
class Persistor;

/// \brief Interface class for connecting a newly created entity to its
/// persistor
class PersistorBase {
  public:
    virtual ~PersistorBase() { }

    virtual void persist() = 0;
};

/// \brief Class template for connecting a newly created entity to its
/// persistor
template <class T>
class PersistorConnection : public PersistorBase {
  private:
    T & m_t;
    Persistor<T> & m_p;
  public:
    PersistorConnection(T & t, Persistor<T> & p) : m_t(t), m_p(p) { }

    /// Use p to hook up t into the persistance code
    void persist();
};

/// \brief Base class for for factories for creating entities
///
/// An Entity consists of an instance of one of a number of C++ classes
/// optionally with a script. Stores information about default attributes,
/// script language and class name.
class FactoryBase {
  protected:
    FactoryBase();
  public:
    ScriptFactory * m_scriptFactory;
    /// Default attribute values for this class
    Atlas::Message::MapType m_classAttributes;
    /// Default attribute values for instances of this class, including
    /// defaults inherited from parent classes.
    Atlas::Message::MapType m_attributes;
    /// Factory for class from which the class handled by this factory
    /// inherits.
    FactoryBase * m_parent;
    /// Set of factories for classes which inherit from the class handled
    /// by this factory.
    std::set<FactoryBase *> m_children;
    /// Inheritance type of this class.
    TypeNode * m_type;

    virtual ~FactoryBase();

    /// \brief Create a new Entity and make it persistent.
    ///
    /// @param id a string giving the identifier of the Entity.
    /// @param intId an integer giving the identifier of the Entity.
    /// @param pb a pointer to the persistor object for the Entity.
    virtual Entity * newPersistantThing(const std::string & id, long intId, PersistorBase ** pb) = 0;
    /// \brief Add anything required to the entity after it has been created.
    virtual int populate(Entity &) = 0;
    /// \brief Create a copy of this factory.
    virtual FactoryBase * duplicateFactory() = 0;
};

template <class T>
class ThingFactory : public FactoryBase {
  protected:
    ThingFactory(ThingFactory<T> & o);
  public:
    ThingFactory();
    virtual ~ThingFactory();

    virtual T * newPersistantThing(const std::string & id, long intId,
                                   PersistorBase ** p);
    virtual int populate(Entity &);
    virtual FactoryBase * duplicateFactory();
};

// How do we make sure the peristance hooks are put in place in a typesafe way
// but after all the initial attribute have been set.

/// \brief Class template for factories for creating instances of the give
/// entity class
template <class T>
class PersistantThingFactory : public ThingFactory<T> {
  protected:
    PersistantThingFactory(PersistantThingFactory<T> & p) : m_p(p.m_p),
                                                            m_master(false) { }
  public:
    Persistor<T> & m_p;
    const bool m_master;

    PersistantThingFactory() : m_p(* new Persistor<T>()), m_master(true) { }
    virtual ~PersistantThingFactory();
 
    virtual T * newPersistantThing(const std::string & id, long intId, PersistorBase ** p);
    virtual FactoryBase * duplicateFactory();
};

/// \brief Class template for factories for entity classes which cannot or
/// should not be instanced
template <class T>
class ForbiddenThingFactory : public FactoryBase {
  public:
    Persistor<T> & m_p;
    ForbiddenThingFactory() : m_p(* new Persistor<T>()) { }

    virtual ~ForbiddenThingFactory();
 
    virtual T * newPersistantThing(const std::string & id, long intId, PersistorBase ** p);
    virtual int populate(Entity &);
    virtual FactoryBase * duplicateFactory();
};

#endif // SERVER_THING_FACTORY_H
