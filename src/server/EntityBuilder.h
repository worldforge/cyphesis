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

#include "EntityKit.h"
#include "rules/simulation/EntityCreator.h"

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <memory>



class LocatedEntity;
class EntityKit;
class Entity;


/// \brief Builder to handle the creation of all entities for the world.
class EntityBuilder : public EntityCreator {
  protected:

    std::map<std::string, std::unique_ptr<EntityKit>> m_entityFactories;

  public:
    explicit EntityBuilder();
    virtual ~EntityBuilder();

    int installFactory(const std::string & class_name,
                       const Atlas::Objects::Root & class_desc,
                       std::unique_ptr<EntityKit> factory);
    EntityKit * getClassFactory(const std::string & class_name) const;
    Ref<Entity> newEntity(RouterId id,
                              const std::string & type,
                              const Atlas::Objects::Entity::RootEntity & attrs) const override;
    Ref<Entity> newChildEntity(RouterId id,
                              const std::string & type,
                              const Atlas::Objects::Entity::RootEntity & attrs) const;

    void installBaseFactory(const std::string & class_name,
                            const std::string & parent,
                            std::unique_ptr<EntityKit> factory);
    void flushFactories();

    const std::map<std::string, std::unique_ptr<EntityKit>>& getFactories() const {
        return m_entityFactories;
    }

};


#endif // SERVER_ENTITY_BUILDER_H
