// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include <string>
#include <map>

class Thing;
class FactoryBase;
class WorldRouter;

typedef std::map<std::string, FactoryBase *> fdict_t;

class EntityFactory {
    EntityFactory();
    static EntityFactory * m_instance;

    fdict_t factories;
  public:
    static EntityFactory * instance() {
        if (m_instance == NULL) {
            m_instance = new EntityFactory();
        }
        return m_instance;
    }
    static EntityFactory * del() {
        if (m_instance != NULL) {
            delete m_instance;
        }
    }
    Thing * newThing(const string &, const Atlas::Message::Object &, WorldRouter *);
    void flushFactories();
};

#endif // ENTITY_FACTORY_H
