// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_THING_FACTORY_H
#define SERVER_THING_FACTORY_H

#include <Atlas/Message/Object.h>

class Entity;

template <class T>
class Persistor;

class PersistorBase {
  public:
    virtual ~PersistorBase() { }

    virtual void persist() = 0;
};

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

class FactoryBase {
  public:
    std::string script;
    std::string language;
    Atlas::Message::Object::MapType attributes;

    virtual ~FactoryBase() { }

    virtual Entity * newThing() = 0;
    virtual Entity * newPersistantThing(PersistorBase **) = 0;
    virtual FactoryBase * duplicateFactory() = 0;
};

// How do we make sure the peristance hooks are put in place in a typesafe way
// but after all the initial attribute have been set.

template <class T>
class PersistantThingFactory : public FactoryBase {
  private:
    Persistor<T> & m_p;
  public:
    PersistantThingFactory() : m_p(* new Persistor<T>()) { }
    PersistantThingFactory(PersistantThingFactory<T> & p) : m_p(p.m_p) { }

    T * newThing();
    T * newPersistantThing(PersistorBase ** p);
    FactoryBase * duplicateFactory();
};

#endif // SERVER_THING_FACTORY_H
