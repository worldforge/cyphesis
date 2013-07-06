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


#ifndef SERVER_ENTITY_BUILDER_H
#define SERVER_ENTITY_BUILDER_H

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/ObjectsFwd.h>

class BaseWorld;
class LocatedEntity;
class LocatedEntity;
class EntityKit;
class Task;
class TaskKit;

typedef std::map<std::string, EntityKit *> FactoryDict;
typedef std::map<std::string, TaskKit *> TaskFactoryDict;
typedef std::multimap<std::string, TaskKit *> TaskFactoryMultimap;
typedef std::map<std::string, TaskFactoryMultimap> TaskFactoryActivationDict;

/// \brief Builder to handle the creation of all entities for the world.
///
/// Uses PersistantThingFactory to store information about entity types, and
/// create them. Handles connecting entities to their persistor as required.
class EntityBuilder {
  protected:
    explicit EntityBuilder();
    ~EntityBuilder();
    static EntityBuilder * m_instance;

    FactoryDict m_entityFactories;
    TaskFactoryDict m_taskFactories;
    TaskFactoryActivationDict m_taskActivations;

    void installBaseFactory(const std::string & class_name,
                            const std::string & parent,
                            EntityKit * factory);
  public:
    static void init() {
        m_instance = new EntityBuilder();
    }
    static EntityBuilder * instance() {
        return m_instance;
    }
    static void del() {
        if (m_instance != 0) {
            delete m_instance;
            m_instance = 0;
        }
    }

    int installFactory(const std::string & class_name,
                       const Atlas::Objects::Root & class_desc,
                       EntityKit * factory);
    EntityKit * getClassFactory(const std::string & class_name);
    LocatedEntity * newEntity(const std::string & id,
                              long intId,
                              const std::string & type,
                              const Atlas::Objects::Entity::RootEntity & attrs,
                              const BaseWorld & world) const;
    void flushFactories();

    bool isTask(const std::string & class_name);
    bool hasTask(const std::string & class_name);
    Task * buildTask(TaskKit * factory, LocatedEntity & owner) const;
    Task * newTask(const std::string & class_name,
                   LocatedEntity & owner) const;
    void installTaskFactory(const std::string & class_name,
                            TaskKit * factory);
    TaskKit * getTaskFactory(const std::string & class_name);
    void addTaskActivation(const std::string & tool,
                           const std::string & op,
                           TaskKit * factory);
    Task * activateTask(const std::string & tool,
                        const std::string & op,
                        LocatedEntity * target,
                        LocatedEntity & owner) const;

    friend class EntityBuildertest;
};

#endif // SERVER_ENTITY_BUILDER_H
