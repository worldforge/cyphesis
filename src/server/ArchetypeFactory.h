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

#include "EntityKit.h"

#include <Atlas/Objects/Entity.h>

#include <vector>

class EntityBuilder;

/// \brief Concrete factory template for creating in-game entity objects through archetypes.
///
/// An archetype contains one or many entities along with optional thoughts (for NPCs etc).
/// Attributes can refer to other entities by using the entity reference format, whereby a map
/// with a string entry with the key "$eid" is used.
/// If no "objtype" is specified in the entity sent to newEntity the data therein is handled
/// as entity initialization data. The properties specified (except "parent") will all be
/// applied to the first entity created.
/// This behaviour is default in order to make sure archetypes and entities can be created
/// interchangeably.
///
/// If however the "objtype" is set to "archetype" the attributes are expected to follow the
/// format of an archetype, and will be merged into the parent archetype. This mainly means
/// that the code will look for entity definitions in the "entities" attribute, and thought
/// definitions in the "thoughts" attribute.
class ArchetypeFactory : public EntityKit
{
    protected:

        /**
         * @brief Represents one entity to be created.
         */
        struct EntityCreation
        {
            /**
             * @brief The definition of the entity, as found in the archetype.
             */
            Atlas::Objects::Entity::RootEntity definition;

            /**
             * @brief The created entity (might be null if none was created).
             */
            Ref<LocatedEntity> createdEntity;

            /**
             * Any attributes referring to unresolved entities.
             * This will be empty is there are no entity referencing attributes.
             */
            Atlas::Message::MapType unresolvedAttributes;
        };

        ArchetypeFactory(ArchetypeFactory& o);

        Ref<Entity> createEntity(RouterId id,
                                        EntityCreation& entityCreation,
                                        std::map<std::string, EntityCreation>& entities);

        /**
         * @brief Sends any thoughts to the entity.
         * @param entity
         */
        void sendThoughts(LocatedEntity& entity, std::vector<Atlas::Message::Element>& thoughts);

        /**
         * @brief Sends an initial sight of itself to the entity.
         *
         * This is required if the entity has thoughts, since if it gets thoughts before
         * it knows about itself there will be trouble.
         * @param entity
         */
        void sendInitialSight(LocatedEntity& entity);

        /**
         * @brief Checks if the attribute references an unresolved entity.
         *
         * Entities within the archetype can be referenced by using the entity reference
         * format, whereby a map with a string entry with the key "$eid" is used.
         * Such attributes however cannot be set until all of the
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
         * This is done recursively. Any Map element which a string entry with the
         * key "$eid" will be replaced with
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
        void processResolvedAttributes(std::map<std::string, EntityCreation>& entities);

        /**
         * @brief Tries to parse entity data from the map.
         * @param entitiesElement The map containing data.
         * @param entities Parsed entities will be put here.
         * @return True if parsing was successful.
         */
        bool parseEntities(const std::map<std::string, Atlas::Message::MapType>& entitiesElement,
                           std::map<std::string, EntityCreation>& entities);

        /**
         * @brief Tries to parse entity data from the list.
         * @param entitiesElement The list containing data.
         * @param entities Parsed entities will be put here.
         * @return True if parsing was successful.
         */
        bool parseEntities(const Atlas::Message::ListType& entitiesElement,
                           std::map<std::string, EntityCreation>& entities);

        /**
         * Creates a Think-Set operation which adds knowledge about the "origin" to the entity.
         * The "origin" is the position where the entity is created.
         * This only applies if the position of the entity is valid.
         * @param entity An entity.
         * @return A list containing one or zero thoughts.
         */
        std::vector<Atlas::Message::Element> createOriginLocationThought(const LocatedEntity& entity);


    public:
        explicit ArchetypeFactory(EntityBuilder& entityBuilder);

        ~ArchetypeFactory() override;

        Ref<Entity> newEntity(RouterId id,
                              const Atlas::Objects::Entity::RootEntity& attributes) override;

        virtual std::unique_ptr<ArchetypeFactory> duplicateFactory();

        void addProperties(const PropertyManager& propertyManager) override;

        void updateProperties(std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes, const PropertyManager& propertyManager) override;


        EntityBuilder& m_entityBuilder;
        /// Factory for class from which the class handled by this factory
        /// inherits.
        ArchetypeFactory* m_parent;
        /// Set of factories for classes which inherit from the class handled
        /// by this factory.
        std::set<ArchetypeFactory*> m_children;

        /// @brief Entity definitions.
        ///
        /// This is a combination of the entities defined for this instance
        /// as well as for all parents.
        std::map<std::string, Atlas::Message::MapType> m_entities;

        /// @brief Thought definitions.
        ///
        /// This is a combination of the thoughts defined for this instance
        /// as well as for all parents.
        /// Note that the thoughts, as of now, only are applied to the first
        /// defined entity.
        std::vector<Atlas::Message::Element> m_thoughts;


        /// @brief Instance specific entity definitions.
        std::map<std::string, Atlas::Message::MapType> m_classEntities;
        /// @brief Instance specific thought definitions.
        std::vector<Atlas::Message::Element> m_classThoughts;
};

#endif // SERVER_ARCHETYPE_FACTORY_H
