// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_ENTITY_FACTORY_H
#define SERVER_ENTITY_FACTORY_H

#include <Atlas/Message/Element.h>

#include <sigc++/object.h>

class Entity;
class FactoryBase;
class BaseWorld;

template <class T>
class PersistantThingFactory;

typedef std::map<std::string, FactoryBase *> FactoryDict;

/// \brief Class to handle the creation of all entities for the world.
///
/// Uses PersistantThingFactory to store information about entity types, and
/// create them. Handles connecting entities to their persistor as required.
class EntityFactory {
  private:
    typedef std::multimap<std::string, std::pair<std::string, Atlas::Message::MapType> > RuleWaitList;
    explicit EntityFactory(BaseWorld & w);
    static EntityFactory * m_instance;

    FactoryDict m_factories;
    BaseWorld & m_world;
    RuleWaitList m_waitingRules;
    PersistantThingFactory<Entity> * m_eft;

    void getRulesFromFiles(Atlas::Message::MapType&);
    void installRules();
    void installFactory(const std::string &, const std::string &, FactoryBase*);
    void populateFactory(const std::string & className,
                         FactoryBase * factory,
                         const Atlas::Message::MapType & classDesc);
    FactoryBase * getNewFactory(const std::string &);
  public:
    static void init(BaseWorld & w) {
        m_instance = new EntityFactory(w);
        m_instance->installRules();
    }
    static EntityFactory * instance() {
        return m_instance;
    }
    static void del() {
        if (m_instance != NULL) {
            delete m_instance;
        }
    }
    void initWorld();
    Entity * newEntity(const std::string &, const std::string &,
                       const Atlas::Message::MapType &);
    void flushFactories();

    int installRule(const std::string &, const Atlas::Message::MapType&);
    int modifyRule(const std::string &, const Atlas::Message::MapType&);
};

#endif // SERVER_ENTITY_FACTORY_H
