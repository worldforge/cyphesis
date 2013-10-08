// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
// Copyright (C) 2013 Erik Ogenvik
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


#ifndef SERVER_ARCHETYPE_FACTORY_H
#define SERVER_ARCHETYPE_FACTORY_H

#include "common/EntityKit.h"

#include <vector>

/// \brief Concrete factory template for creating in-game entity objects through archetypes.
class ArchetypeFactory : public EntityKit {
  protected:
    explicit ArchetypeFactory(ArchetypeFactory & o);

    LocatedEntity * createEntity(const std::string & id,
                                    long intId,
                                    const Atlas::Objects::Entity::RootEntity & attributes,
                                    LocatedEntity* location,
                                    std::map<std::string, Atlas::Objects::Entity::RootEntity>& entities,
                                    std::map<std::string, LocatedEntity*>& resolvedEntities);

    /**
     * @brief Sends any thoughts to the entity.
     * @param entity
     */
    void sendThoughts(LocatedEntity& entity);

  public:
    explicit ArchetypeFactory();
    virtual ~ArchetypeFactory();

    virtual LocatedEntity * newEntity(const std::string & id, long intId,
                const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location);
    virtual ArchetypeFactory * duplicateFactory();

    virtual void addProperties();

    virtual void updateProperties();

    /// Factory for class from which the class handled by this factory
    /// inherits.
    ArchetypeFactory * m_parent;
    /// Set of factories for classes which inherit from the class handled
    /// by this factory.
    std::set<ArchetypeFactory *> m_children;

    std::vector<Atlas::Message::Element> m_entities;
    std::vector<Atlas::Message::Element> m_thoughts;

};

#endif // SERVER_ARCHETYPE_FACTORY_H
