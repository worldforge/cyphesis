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

typedef std::map<std::string, FactoryBase *> FactoryDict;

class EntityFactory {
    EntityFactory();
    static EntityFactory * m_instance;

    FactoryDict factories;
  public:
    static EntityFactory * instance() {
        if (m_instance == NULL) {
            m_instance = new EntityFactory();
        }
        return m_instance;
    }
    static void del() {
        if (m_instance != NULL) {
            delete m_instance;
        }
    }
    Entity * newEntity(const std::string &,
                      const Atlas::Message::Object::MapType &,
                      const EntityDict &);
    void flushFactories();

    void installBaseClasses();
    void installFactory(const std::string &, const std::string &, FactoryBase*);
    void installClass(const std::string &, const std::string&);
    FactoryBase * getFactory(const std::string &);
};

#endif // ENTITY_FACTORY_H
