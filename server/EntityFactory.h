// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_ENTITY_FACTORY_H
#define RULESETS_ENTITY_FACTORY_H

#include <common/types.h>

#include <sigc++/object.h>

namespace Atlas { namespace Message {
  class Object;
} }

class Thing;
class FactoryBase;
class BaseWorld;

typedef std::map<std::string, FactoryBase *> FactoryDict;

class EntityFactory {
    EntityFactory(BaseWorld & w);
    static EntityFactory * m_instance;

    FactoryDict factories;
    BaseWorld & m_world;

    void installBaseClasses();
  public:
    static void init(BaseWorld & w) {
        m_instance = new EntityFactory(w);
        m_instance->installBaseClasses();
    }
    static EntityFactory * instance() {
        return m_instance;
    }
    static void del() {
        if (m_instance != NULL) {
            delete m_instance;
        }
    }
    Entity * newEntity(const std::string &, const std::string &,
                       const Atlas::Message::Object::MapType &);
    void flushFactories();

    void installFactory(const std::string &, const std::string &, FactoryBase*);
    void installClass(const std::string &, const std::string&);
    FactoryBase * getFactory(const std::string &);
};

#endif // ENTITY_FACTORY_H
