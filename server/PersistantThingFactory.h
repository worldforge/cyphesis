// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_THING_FACTORY_H
#define RULESETS_THING_FACTORY_H

#include <Atlas/Message/Object.h>

class Thing;

class PersistorBase {
  public:
    virtual ~PersistorBase() { }

    virtual void persist();
};

template <class T, class P>
class PersistorConnection : public PersistorBase {
  private:
    T & m_t;
    P & m_p;
  public:
    PersistorConnection(T & t, P & p) : m_t(t), m_p(p) { }
    ~PersistorConnection() { }

    /// Use p to hook up t into the persistance code
    void persist() { m_p.persist(m_t); }
};

class FactoryBase {
  public:
    std::string script;
    std::string language;
    Atlas::Message::Object::MapType attributes;

    virtual ~FactoryBase() { }

    virtual Thing * newThing() = 0;
    virtual PersistorBase * newPersistantThing(Thing **) = 0;
    virtual FactoryBase * dupFactory() = 0;
};

// How do we make sure the peristance hooks are put in place in a typesafe way
// but after all the initial attribute have been set.

template <class T, class P>
class PersistantThingFactory : public FactoryBase {
  private:
    P & m_p;
  public:
    PersistantThingFactory(P & p) : m_p(p) { }

    Thing * newThing() {
        return new T();
    }
    PersistorBase * newPersistantThing(Thing ** t) {
        *t = new T();
        return new PersistorConnection<T, P>(**t, m_p);
    }
    FactoryBase * dupFactory() {
        return new PersistantThingFactory<T, P>();
    }
};

#endif // RULESETS_THING_FACTORY_H
