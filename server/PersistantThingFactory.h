// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_THING_FACTORY_H
#define SERVER_THING_FACTORY_H

#include <Atlas/Message/Element.h>

class Entity;

template <class T>
class Persistor;

class ScriptFactory;

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
    Atlas::Message::MapType m_attributes;

    virtual ~FactoryBase();

    virtual Entity * newThing(const std::string & id) = 0;
    virtual Entity * newPersistantThing(const std::string & id, PersistorBase **) = 0;
    virtual FactoryBase * duplicateFactory() = 0;
};

// How do we make sure the peristance hooks are put in place in a typesafe way
// but after all the initial attribute have been set.

/// \brief Class template for factories for creating instances of the give
/// entity class
template <class T>
class PersistantThingFactory : public FactoryBase {
  protected:
    PersistantThingFactory(PersistantThingFactory<T> & p) : m_p(p.m_p),
                                                            m_master(false) { }
  public:
    Persistor<T> & m_p;
    const bool m_master;

    PersistantThingFactory() : m_p(* new Persistor<T>()), m_master(true) { }
    virtual ~PersistantThingFactory();
 
    virtual T * newThing(const std::string & id);
    virtual T * newPersistantThing(const std::string & id, PersistorBase ** p);
    virtual FactoryBase * duplicateFactory();
};

template <class T>
class ForbiddenThingFactory : public FactoryBase {
  public:
    Persistor<T> & m_p;
    ForbiddenThingFactory() : m_p(* new Persistor<T>()) { }

    virtual ~ForbiddenThingFactory();
 
    virtual T * newThing(const std::string & id);
    virtual T * newPersistantThing(const std::string & id, PersistorBase ** p);
    virtual FactoryBase * duplicateFactory();
};

#endif // SERVER_THING_FACTORY_H
