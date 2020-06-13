/*
 Copyright (C) 2014 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef PROVIDERS_H_
#define PROVIDERS_H_

#include "Predicates.h"

#include "rules/LocatedEntity.h"
#include <Atlas/Message/Element.h>
#include <list>

#include <map>
#include <functional>
#include <string>

class TypeNode;

class BBoxProperty;

namespace EntityFilter {

    class Predicate;

    /**
     * A simple struct for storing both an entity and a position.
     */
    struct QueryEntityLocation
    {
        LocatedEntity& entity;

        const WFMath::Point<3>* pos = nullptr;

        QueryEntityLocation() = delete;

        QueryEntityLocation(LocatedEntity& entity_in) : entity(entity_in)
        {}

        QueryEntityLocation(LocatedEntity& entity_in, const WFMath::Point<3>* pos_in) : entity(entity_in), pos(pos_in)
        {}
    };

    struct QueryContext
    {
        /**
         * The main entity (with an optional position) which the filter is applied on. This is required, all other parameters are optional.
         */
        QueryEntityLocation entityLoc;
        /**
         * Represents an actor, mainly used together with the "tool" field. The "actor" is the entity which uses the "tool".
         */
        LocatedEntity* actor = nullptr;

        /**
         * Represents something being used, often along with an "actor".
         */
        LocatedEntity* tool = nullptr;

        /**
         * Do not set this field yourself. This is used when iterating through Entity sets, and is set automatically by the Providers.
         */
        LocatedEntity* child = nullptr;

        /**
         * A memory, using when filters are attached to Minds.
         */
        std::function<const Atlas::Message::MapType&(const std::string&)> memory_lookup_fn;

        //This field can be used by client code to specify entity for "self.*" query
        LocatedEntity* self_entity = nullptr;

        /**
         * Gets entities by their id. Used by the get_entity function. Optional.
         */
        std::function<Ref<LocatedEntity>(const std::string&)> entity_lookup_fn;

        /**
         * Looks up types by name. Used by the DynamicTypeNodeProvider. Optional.
         */
        std::function<const TypeNode*(const std::string&)> type_lookup_fn;

        /**
         * If provided allows predicates to report on failures to match. This can be used to provide better messages to clients.
         * Used by the DescribePredicate.
         */
        std::function<void (const std::string&)> report_error_fn;
    };

    class TypedProvider
    {
        public:
            virtual ~TypedProvider() = default;

            /**
             * @brief Gets the type of the pointer provided by this instance, if that can be determined.
             *
             * This will only return a valid value if
             * 1) An Element containing a pointer will always be returned.
             * 2) The exact same type always will be returned.
             * @return Null, or a type info instance defining the type returned as a pointer Element.
             */
            virtual const std::type_info* getType() const = 0;
    };


    template<typename T>
    class Consumer : public TypedProvider, public std::enable_shared_from_this<Consumer<T>>
    {
        public:
            ~Consumer() override = default;

            virtual void value(Atlas::Message::Element& value, const T& parent_value) const = 0;

            const std::type_info* getType() const override;
    };

    template<typename T>
    const std::type_info* Consumer<T>::getType() const
    {
        return nullptr;
    }


    template<typename T>
    class ProviderBase
    {
        public:
            explicit ProviderBase(std::shared_ptr<Consumer<T>> consumer);

            virtual ~ProviderBase();

        protected:
            std::shared_ptr<Consumer<T>> m_consumer;
    };

    template<typename T>
    inline ProviderBase<T>::ProviderBase(std::shared_ptr<Consumer<T>> consumer)
            : m_consumer(std::move(consumer))
    {
    }

    template<typename T>
    inline ProviderBase<T>::~ProviderBase() = default;

    template<typename TProviding, typename TConsuming>
    class ConsumingProviderBase : public ProviderBase<TProviding>, public Consumer<TConsuming>
    {
        public:
            explicit ConsumingProviderBase(std::shared_ptr<Consumer<TProviding>> consumer);

            ~ConsumingProviderBase() override = default;;

            const std::type_info* getType() const override;
    };

    template<typename TProviding, typename TConsuming>
    inline ConsumingProviderBase<TProviding, TConsuming>::ConsumingProviderBase(std::shared_ptr<Consumer<TProviding>> consumer)
            : ProviderBase<TProviding>(consumer)
    {
    }

    template<typename TProviding, typename TConsuming>
    inline const std::type_info* ConsumingProviderBase<TProviding, TConsuming>::getType() const
    {
        if (this->m_consumer) {
            return this->m_consumer->getType();
        }
        return nullptr;
    }


    template<typename T>
    class NamedAttributeProviderBase : public ProviderBase<T>
    {
        public:
            NamedAttributeProviderBase(std::shared_ptr<Consumer<T>> consumer, std::string attribute_name);

        protected:
            const std::string m_attribute_name;
    };

    template<typename T>
    inline NamedAttributeProviderBase<T>::NamedAttributeProviderBase(std::shared_ptr<Consumer<T>> consumer, std::string attribute_name)
            : ProviderBase<T>(consumer), m_attribute_name(std::move(attribute_name))
    {
    }

    template<typename TProviding, typename TConsuming>
    class ConsumingNamedAttributeProviderBase : public NamedAttributeProviderBase<TProviding>, public Consumer<TConsuming>
    {
        public:
            ConsumingNamedAttributeProviderBase(std::shared_ptr<Consumer<TProviding>> consumer, const std::string& attribute_name);

            ~ConsumingNamedAttributeProviderBase() override = default;;

            const std::type_info* getType() const override;
    };

    template<typename TProviding, typename TConsuming>
    inline ConsumingNamedAttributeProviderBase<TProviding, TConsuming>::ConsumingNamedAttributeProviderBase(std::shared_ptr<Consumer<TProviding>> consumer, const std::string& attribute_name)
            : NamedAttributeProviderBase<TProviding>(std::move(consumer), attribute_name)
    {
    }

    template<typename TProviding, typename TConsuming>
    inline const std::type_info* ConsumingNamedAttributeProviderBase<TProviding, TConsuming>::getType() const
    {
        if (this->m_consumer) {
            return this->m_consumer->getType();
        }
        return nullptr;
    }


    class FixedElementProvider : public Consumer<QueryContext>
    {
        public:
            explicit FixedElementProvider(Atlas::Message::Element element);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const Atlas::Message::Element m_element;
    };

    class DynamicTypeNodeProvider : public ConsumingProviderBase<TypeNode, QueryContext>
    {
        public:
            DynamicTypeNodeProvider(std::shared_ptr<Consumer<TypeNode>> consumer, std::string type);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const std::type_info* getType() const override;

            const std::string m_type;
    };

    class FixedTypeNodeProvider : public ConsumingProviderBase<TypeNode, QueryContext>
    {
        public:
            FixedTypeNodeProvider(std::shared_ptr<Consumer<TypeNode>> consumer, const TypeNode& type);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const std::type_info* getType() const override;

            const TypeNode& m_type;
    };

    class MemoryProvider : public ConsumingProviderBase<Atlas::Message::Element, QueryContext>
    {
        public:
            explicit MemoryProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer);

            void value(Atlas::Message::Element& value, const QueryContext&) const override;
    };

    class EntityProvider : public ConsumingProviderBase<LocatedEntity, QueryContext>
    {
        public:
            explicit EntityProvider(std::shared_ptr<Consumer<LocatedEntity>> consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const std::type_info* getType() const override;
    };

    class EntityLocationProvider : public ConsumingProviderBase<LocatedEntity, QueryContext>
    {
        public:
            explicit EntityLocationProvider(std::shared_ptr<Consumer<LocatedEntity>> consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const std::type_info* getType() const override;
    };

    class ActorProvider : public EntityProvider
    {
        public:
            explicit ActorProvider(std::shared_ptr<Consumer<LocatedEntity>> consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

    };

    class ToolProvider : public EntityProvider
    {
        public:
            explicit ToolProvider(std::shared_ptr<Consumer<LocatedEntity>> consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

    };

    class ChildProvider : public EntityProvider
    {
        public:
            explicit ChildProvider(std::shared_ptr<Consumer<LocatedEntity>> consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

    };

    class SelfEntityProvider : public ConsumingProviderBase<LocatedEntity, QueryContext>
    {
        public:
            explicit SelfEntityProvider(std::shared_ptr<Consumer<LocatedEntity>> consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const std::type_info* getType() const override;
    };

    class EntityTypeProvider : public ConsumingProviderBase<TypeNode, LocatedEntity>
    {
        public:
            explicit EntityTypeProvider(std::shared_ptr<Consumer<TypeNode>> consumer);

            void value(Atlas::Message::Element& value, const LocatedEntity& entity) const override;

            const std::type_info* getType() const override;
    };

    //Provides integer ID of an entity
    class EntityIdProvider : public Consumer<LocatedEntity>
    {
        public:
            void value(Atlas::Message::Element& value, const LocatedEntity& entity) const override;
    };

    class TypeNodeProvider : public Consumer<TypeNode>
    {
        public:
            explicit TypeNodeProvider(std::string attribute_name);

            void value(Atlas::Message::Element& value, const TypeNode& type) const override;

            const std::string m_attribute_name;
    };

    class BBoxProvider : public ConsumingProviderBase<Atlas::Message::Element, LocatedEntity>
    {
        public:
            enum class Measurement
            {
                    HEIGHT, WIDTH, DEPTH, VOLUME, AREA
            };

            BBoxProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer, Measurement measurement);

            void value(Atlas::Message::Element& value, const LocatedEntity& prop) const override;

        protected:
            Measurement m_measurement;
    };

    template<typename TProperty>
    class PropertyProvider : public ConsumingNamedAttributeProviderBase<TProperty, LocatedEntity>
    {
        public:
            PropertyProvider(std::shared_ptr<Consumer<TProperty>> consumer, const std::string& attribute_name);

            virtual ~PropertyProvider() = default;

            virtual void value(Atlas::Message::Element& value, const LocatedEntity& entity) const;
    };

    template<typename TProperty>
    inline PropertyProvider<TProperty>::PropertyProvider(std::shared_ptr<Consumer<TProperty>> consumer, const std::string& attribute_name)
            : ConsumingNamedAttributeProviderBase<TProperty, LocatedEntity>(consumer, attribute_name)
    {
    }

    template<typename TProperty>
    inline void PropertyProvider<TProperty>::value(Atlas::Message::Element& value, const LocatedEntity& entity) const
    {
        const TProperty* prop = entity.getPropertyClass<TProperty>(this->m_attribute_name);
        if (!prop) {
            return;
        }

        if (this->m_consumer) {
            this->m_consumer->value(value, *prop);
        } else {
            value = (void*) prop;
        }
    }


    class SoftPropertyProvider : public ConsumingNamedAttributeProviderBase<Atlas::Message::Element, LocatedEntity>
    {
        public:
            SoftPropertyProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer, const std::string& attribute_name);

            void value(Atlas::Message::Element& value, const LocatedEntity& entity) const override;
    };

    class MapProvider : public ConsumingNamedAttributeProviderBase<Atlas::Message::Element, Atlas::Message::Element>
    {
        public:
            MapProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer, const std::string& attribute_name);

            void value(Atlas::Message::Element& value, const Atlas::Message::Element& parent_element) const override;
    };


    ///\brief This class retrieves a pointer to the m_contains property of a given entity
    class ContainsProvider : public Consumer<LocatedEntity>
    {
        public:
            void value(Atlas::Message::Element& value, const LocatedEntity& entity) const override;

            const std::type_info* getType() const override;
    };

    ///\brief This class uses container Consumer to retrieve a container and condition Predicate
    ///to check whether there exists an entity within the container that matches the given condition
    class ContainsRecursiveFunctionProvider : public Consumer<QueryContext>
    {
        public:
            ContainsRecursiveFunctionProvider(std::shared_ptr<Consumer<QueryContext>> container,
                                              std::shared_ptr<Predicate> condition,
                                              bool recursive);

            void value(Atlas::Message::Element& value,
                       const QueryContext& context) const override;

        private:
            ///\brief Condition used to match entities within the container
            std::shared_ptr<Predicate> m_condition;
            ///\brief A Consumer which must return LocatedEntitySet* i.e. entity.contains
            std::shared_ptr<Consumer<QueryContext>> m_consumer;

            bool m_recursive;

            bool checkContainer(LocatedEntitySet* container,
                                const QueryContext& context) const;
    };


    class GetEntityFunctionProvider : public ConsumingProviderBase<LocatedEntity, QueryContext>
    {
        public:
            explicit GetEntityFunctionProvider(std::shared_ptr<Consumer<QueryContext>> entity_provider, std::shared_ptr<Consumer<LocatedEntity>> consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const std::type_info* getType() const override;

        private:
            std::shared_ptr<Consumer<QueryContext>> m_entity_provider;
    };



}

#endif /* PROVIDERS_H_ */
