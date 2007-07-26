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

// $Id: EntityFactory.h,v 1.48 2007-07-26 00:15:53 alriddoch Exp $

#ifndef SERVER_ENTITY_FACTORY_H
#define SERVER_ENTITY_FACTORY_H

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/ObjectsFwd.h>

class Entity;
class Character;
class Task;
class FactoryBase;
class TaskFactory;
class ArithmeticFactory;
class BaseWorld;

template <class T>
class PersistantThingFactory;

typedef std::map<std::string, FactoryBase *> FactoryDict;
typedef std::map<std::string, TaskFactory *> TaskFactoryDict;
typedef std::multimap<std::string, TaskFactory *> TaskFactoryMultimap;
typedef std::map<std::string, TaskFactoryMultimap> TaskFactoryActivationDict;
typedef std::map<std::string, ArithmeticFactory *> StatisticsFactoryDict;


/// \brief Class to handle rules that cannot yet be installed, and the reason
class RuleWaiting {
  public:
    std::string name;
    Atlas::Message::MapType desc;
    std::string reason;
};

typedef std::multimap<std::string, RuleWaiting> RuleWaitList;

/// \brief Class to handle the creation of all entities for the world.
///
/// Uses PersistantThingFactory to store information about entity types, and
/// create them. Handles connecting entities to their persistor as required.
class EntityFactory {
  protected:
    explicit EntityFactory(BaseWorld & w);
    ~EntityFactory();
    static EntityFactory * m_instance;

    FactoryDict m_factories;
    TaskFactoryDict m_taskFactories;
    TaskFactoryActivationDict m_taskActivations;
    StatisticsFactoryDict m_statisticsFactories;

    BaseWorld & m_world;
    RuleWaitList m_waitingRules;

    void getRulesFromFiles(Atlas::Message::MapType&);
    void installRules();
    void installFactory(const std::string &, const std::string &, FactoryBase*);
    void populateFactory(const std::string & className,
                         FactoryBase * factory,
                         const Atlas::Message::MapType & classDesc);
    FactoryBase * getNewFactory(const std::string &);
    bool isTask(const std::string &);
    int installTaskClass(const std::string &, const std::string &,
                           const Atlas::Message::MapType&);
    int installEntityClass(const std::string &, const std::string &,
                           const Atlas::Message::MapType&);
    int installOpDefinition(const std::string &, const std::string &,
                            const Atlas::Message::MapType&);

    int modifyTaskClass(const std::string &, const Atlas::Objects::Root &);
    int modifyEntityClass(const std::string &, const Atlas::Objects::Root &);
    int modifyOpDefinition(const std::string &, const Atlas::Objects::Root &);

    void waitForRule(const std::string &,
                     const Atlas::Message::MapType &,
                     const std::string &,
                     const std::string &);
  public:
    static void init(BaseWorld & w) {
        m_instance = new EntityFactory(w);
        m_instance->installRules();
    }
    static EntityFactory * instance() {
        return m_instance;
    }
    static void del() {
        if (m_instance != 0) {
            delete m_instance;
            m_instance = 0;
        }
    }
    void initWorld();

    Entity * newEntity(const std::string &, long, const std::string &,
                       const Atlas::Objects::Entity::RootEntity &) const;
    void flushFactories();

    Task * newTask(const std::string &, Character &) const;
    Task * activateTask(const std::string &, const std::string &,
                        const std::string &, Character &) const;
    int addStatisticsScript(Character &) const;

    int installRule(const std::string &, const Atlas::Message::MapType&);
    int modifyRule(const std::string &, const Atlas::Objects::Root &);
};

#endif // SERVER_ENTITY_FACTORY_H
