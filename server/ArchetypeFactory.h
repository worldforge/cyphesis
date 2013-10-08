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

#include <Atlas/Objects/Entity.h>

#include <vector>

/// \brief Concrete factory template for creating in-game entity objects through archetypes.
///
/// An archetype contains one or many entities along with optional thoughts (for NPCs etc).
/// Attributes can refer to other entities by prefixing their id with a "@" sign.
class ArchetypeFactory : public EntityKit {
  protected:

    /**
     * @brief Represents one entity to be created.
     */
    struct EntityCreation {
        /**
         * @brief The definition of the entity, as found in the archetype.
         */
        Atlas::Objects::Entity::RootEntity definition;

        /**
         * @brief The created entity (might be null if none was created).
         */
        LocatedEntity* createdEntity;

        /**
         * Any attributes referring to unresolved entities.
         * This will be empty is there are no entity referencing attributes.
         */
        Atlas::Message::MapType unresolvedAttributes;
    };

    explicit ArchetypeFactory(ArchetypeFactory & o);

    LocatedEntity * createEntity(const std::string & id,
                                    long intId,
                                    EntityCreation& entityCreation,
                                    LocatedEntity* location,
                                    std::map<std::string, EntityCreation>& entities);

    /**
     * @brief Sends any thoughts to the entity.
     * @param entity
     */
    void sendThoughts(LocatedEntity& entity);

    /**
     * @brief Checks if the attribute references an unresolved entity.
     *
     * Entities within the archetype can be referenced through prefixing their local
     * id with a "@" character. Such attributes however cannot be set until all of the
     * entities have been created, and their final ids have been resolved. This method
     * checks if any attribute is referring to an unresolved entity, and thus needs to
     * be resolved first.
     * @param attr The attribute to check.
     * @return True if the attribute contained an unresolved entity ref
     */
    bool isEntityRefAttribute(const Atlas::Message::Element& attr) const;

    /**
     * @brief Resolves references to unresolved entities.
     *
     * This is done recursively. Any String element which refers to an
     * unresolved entity (i.e. is prefixed with "@") will be replaced with
     * a Ptr element wrapping a LocatedEntity instance (given that the
     * reference is correct).
     *
     * @param entities All processed entities.
     * @param attr The attribute which will be resolved.
     */
    void resolveEntityReference(
            std::map<std::string, EntityCreation>& entities,
            Atlas::Message::Element& attr);

    /**
     * @brief Processes all unresolved attributes for all entities.
     *
     * Any unresolved reference will be resolved, and the attribute set
     * on the entity.
     * @param entities All processed entities.
     */
    void processResolvedAttributes(
            std::map<std::string, EntityCreation>& entities);


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
