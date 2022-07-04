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


#ifndef SERVER_ENTITY_FACTORY_H
#define SERVER_ENTITY_FACTORY_H

#include "EntityKit.h"

/**
 * An attribute of a class.
 */
struct ClassAttribute
{
    /**
     * A default value will be directly applied.
     */
    Atlas::Message::Element defaultValue;

    /**
     * This value will be appended.
     * For numerical values it will result in an addition.
     * For strings it will result in the value being added to the end of the existing string.
     * For maps it will result in the values being inserted.
     * For lists it will result in the values being added to the back of the list.
     */
    Atlas::Message::Element append;

    /**
     * This value will be prepended.
     * For numerical values it will result in an addition.
     * For strings it will result in the value being added to the start of the existing string.
     * For maps it will result in the values being inserted.
     * For lists it will result in the values being added to the start of the list.
     */
    Atlas::Message::Element prepend;

    /**
     * This value will be subtracted.
     * For numerical values it will result in a subtraction.
     * For strings nothing will happen, since it's not obvious how one subtracts one string from another.
     * For maps it will result in the keys being removed (any values are ignored).
     * For lists it will result in the values being removed to the start of the list.
     */
    Atlas::Message::Element subtract;

    /**
     * This value will be multiplied with the base value, and then added.
     * For numerical values it will result in a multiplication and then an addition.
     * For strings nothing will happen.
     * For maps nothing will happen.
     * For lists nothing will happen.
     */
    Atlas::Message::Element add_fraction;

    /**
     * Apply the values on an existing element.
     * @param existing An existing element, which will be altered.
     */
    void combine(Atlas::Message::Element& existing) const;
};

class EntityFactoryBase : public EntityKit
{
    protected:

        void initializeEntity(Entity& thing,
                              const Atlas::Objects::Entity::RootEntity& attributes);

    public:

        /// Default attribute values for this class
        std::map<std::string, ClassAttribute> m_classAttributes;
        /// Default attribute values for instances of this class, including
        /// defaults inherited from parent classes.
        Atlas::Message::MapType m_attributes;
        /// Factory for class from which the class handled by this factory
        /// inherits.
        EntityFactoryBase* m_parent;
        /// Set of factories for classes which inherit from the class handled
        /// by this factory.
        std::set<EntityFactoryBase*> m_children;

        EntityFactoryBase();

        ~EntityFactoryBase() override;

        virtual std::unique_ptr<EntityFactoryBase> duplicateFactory() = 0;

        void addProperties(const PropertyManager& propertyManager) override;

        void updateProperties(std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes, const PropertyManager& propertyManager) override;

};

/// \brief Concrete factory template for creating in-game entity objects.
template<class T>
class EntityFactory : public EntityFactoryBase
{
    protected:
        EntityFactory(EntityFactory<T>& o);

    public:

        EntityFactory();

        ~EntityFactory() override;

        Ref<Entity> newEntity(RouterId id,
                              const Atlas::Objects::Entity::RootEntity& attributes) override;

        std::unique_ptr<EntityFactoryBase> duplicateFactory() override;

};

#endif // SERVER_ENTITY_FACTORY_H
