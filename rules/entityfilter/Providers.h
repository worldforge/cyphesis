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
#include <utility>
#include <map>
#include <functional>
#include <string>

class OutfitProperty;

class TypeNode;

class BBoxProperty;

namespace EntityFilter {

    class Predicate;

    struct QueryContext
    {
        /**
         * The main entity which the filter is applied on. This is required, all other parameters are optional.
         */
        LocatedEntity& entity;

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

        std::function<Ref<LocatedEntity>(const std::string&)> entity_lookup_fn;

        std::function<const TypeNode*(const std::string&)> type_lookup_fn;
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
    class Consumer : public TypedProvider
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
            explicit ProviderBase(Consumer<T>* consumer);

            virtual ~ProviderBase();

        protected:
            Consumer<T>* m_consumer;
    };

    template<typename T>
    inline ProviderBase<T>::ProviderBase(Consumer<T>* consumer)
        : m_consumer(consumer)
    {
    }

    template<typename T>
    inline ProviderBase<T>::~ProviderBase()
    {
        delete m_consumer;
    }

    template<typename TProviding, typename TConsuming>
    class ConsumingProviderBase : public ProviderBase<TProviding>, public Consumer<TConsuming>
    {
        public:
            explicit ConsumingProviderBase(Consumer<TProviding>* consumer);

            ~ConsumingProviderBase() override = default;;

            const std::type_info* getType() const override;
    };

    template<typename TProviding, typename TConsuming>
    inline ConsumingProviderBase<TProviding, TConsuming>::ConsumingProviderBase(Consumer<TProviding>* consumer)
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
            NamedAttributeProviderBase(Consumer<T>* consumer, std::string attribute_name);

        protected:
            const std::string m_attribute_name;
    };

    template<typename T>
    inline NamedAttributeProviderBase<T>::NamedAttributeProviderBase(Consumer<T>* consumer, std::string attribute_name)
        : ProviderBase<T>(consumer), m_attribute_name(std::move(attribute_name))
    {
    }

    template<typename TProviding, typename TConsuming>
    class ConsumingNamedAttributeProviderBase : public NamedAttributeProviderBase<TProviding>, public Consumer<TConsuming>
    {
        public:
            ConsumingNamedAttributeProviderBase(Consumer<TProviding>* consumer, const std::string& attribute_name);

            ~ConsumingNamedAttributeProviderBase() override = default;;

            const std::type_info* getType() const override;
    };

    template<typename TProviding, typename TConsuming>
    inline ConsumingNamedAttributeProviderBase<TProviding, TConsuming>::ConsumingNamedAttributeProviderBase(Consumer<TProviding>* consumer, const std::string& attribute_name)
        : NamedAttributeProviderBase<TProviding>(consumer, attribute_name)
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
            explicit FixedElementProvider(const Atlas::Message::Element& element);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const Atlas::Message::Element m_element;
    };

    class DynamicTypeNodeProvider : public ConsumingProviderBase<TypeNode, QueryContext>
    {
        public:
            DynamicTypeNodeProvider(Consumer<TypeNode>* consumer, const std::string& type);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const std::type_info* getType() const override;

            const std::string m_type;
    };

    class FixedTypeNodeProvider : public ConsumingProviderBase<TypeNode, QueryContext>
    {
        public:
            FixedTypeNodeProvider(Consumer<TypeNode>* consumer, const TypeNode& type);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const std::type_info* getType() const override;

            const TypeNode& m_type;
    };

    class MemoryProvider : public ConsumingProviderBase<Atlas::Message::Element, QueryContext>
    {
        public:
            explicit MemoryProvider(Consumer<Atlas::Message::Element>* consumer);

            void value(Atlas::Message::Element& value, const QueryContext&) const override;
    };

    class EntityProvider : public ConsumingProviderBase<LocatedEntity, QueryContext>
    {
        public:
            explicit EntityProvider(Consumer<LocatedEntity>* consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const std::type_info* getType() const override;
    };

    class ActorProvider : public EntityProvider
    {
        public:
            explicit ActorProvider(Consumer<LocatedEntity>* consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

    };

    class ToolProvider : public EntityProvider
    {
        public:
            explicit ToolProvider(Consumer<LocatedEntity>* consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

    };

    class ChildProvider : public EntityProvider
    {
        public:
            explicit ChildProvider(Consumer<LocatedEntity>* consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

    };

    class SelfEntityProvider : public ConsumingProviderBase<LocatedEntity, QueryContext>
    {
        public:
            explicit SelfEntityProvider(Consumer<LocatedEntity>* consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const std::type_info* getType() const override;
    };

    class EntityTypeProvider : public ConsumingProviderBase<TypeNode, LocatedEntity>
    {
        public:
            explicit EntityTypeProvider(Consumer<TypeNode>* consumer);

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
            explicit TypeNodeProvider(const std::string& attribute_name);

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

            BBoxProvider(Consumer<Atlas::Message::Element>* consumer, Measurement measurement);

            void value(Atlas::Message::Element& value, const LocatedEntity& prop) const override;

        protected:
            Measurement m_measurement;
    };

    template<typename TProperty>
    class PropertyProvider : public ConsumingNamedAttributeProviderBase<TProperty, LocatedEntity>
    {
        public:
            PropertyProvider(Consumer<TProperty>* consumer, const std::string& attribute_name);

            virtual ~PropertyProvider() = default;

            virtual void value(Atlas::Message::Element& value, const LocatedEntity& entity) const;
    };

    template<typename TProperty>
    inline PropertyProvider<TProperty>::PropertyProvider(Consumer<TProperty>* consumer, const std::string& attribute_name)
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
            SoftPropertyProvider(Consumer<Atlas::Message::Element>* consumer, const std::string& attribute_name);

            void value(Atlas::Message::Element& value, const LocatedEntity& entity) const override;
    };

    class MapProvider : public ConsumingNamedAttributeProviderBase<Atlas::Message::Element, Atlas::Message::Element>
    {
        public:
            MapProvider(Consumer<Atlas::Message::Element>* consumer, const std::string& attribute_name);

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
            ContainsRecursiveFunctionProvider(Consumer<QueryContext>* container,
                                              Predicate* condition);

            void value(Atlas::Message::Element& value,
                       const QueryContext& context) const override;

        private:
            ///\brief Condition used to match entities within the container
            Predicate* m_condition;
            ///\brief A Consumer which must return LocatedEntitySet* i.e. entity.contains
            Consumer<QueryContext>* m_consumer;

            bool checkContainer(LocatedEntitySet* container,
                                const QueryContext& context) const;
    };


    class GetEntityFunctionProvider : public ConsumingProviderBase<LocatedEntity, QueryContext>
    {
        public:
            explicit GetEntityFunctionProvider(Consumer<QueryContext>* entity_provider, Consumer<LocatedEntity>* consumer);

            void value(Atlas::Message::Element& value, const QueryContext& context) const override;

            const std::type_info* getType() const override;

        private:
            Consumer<QueryContext>* m_entity_provider;
    };


    class ProviderFactory
    {
        public:
            struct Segment
            {
                std::string delimiter;
                std::string attribute;
            };
            typedef std::list<Segment> SegmentsList;

            virtual ~ProviderFactory() = default;

            virtual Consumer<QueryContext>* createProviders(SegmentsList segments) const;

            virtual Consumer<QueryContext>* createProvider(Segment segment) const;

            virtual Consumer<QueryContext>* createSimpleGetEntityFunctionProvider(Consumer<QueryContext>* entity_provider) const;

            virtual Consumer<QueryContext>* createGetEntityFunctionProvider(Consumer<QueryContext>* entity_provider, SegmentsList segments) const;

        protected:

            DynamicTypeNodeProvider* createDynamicTypeNodeProvider(SegmentsList segments) const;

            template<typename T>
            T* createEntityProvider(SegmentsList segments) const;

            SelfEntityProvider* createSelfEntityProvider(SegmentsList segments) const;

            BBoxProvider* createBBoxProvider(SegmentsList segments) const;

            Consumer<LocatedEntity>* createPropertyProvider(SegmentsList segments) const;

            MapProvider* createMapProvider(SegmentsList segments) const;

            TypeNodeProvider* createTypeNodeProvider(SegmentsList segments) const;

            MemoryProvider* createMemoryProvider(SegmentsList segments) const;
    };

}

#endif /* PROVIDERS_H_ */
