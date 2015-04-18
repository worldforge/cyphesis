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

#include "../LocatedEntity.h"
#include <Atlas/Message/Element.h>
#include <list>

class OutfitProperty;
class TypeNode;
class BBoxProperty;

namespace EntityFilter
{

class Predicate;

struct QueryContext {
        LocatedEntity& entity;
        std::map<std::string, Atlas::Message::Element> memory;

        //This field can be used by client code to specify entity for "self.*" query
        LocatedEntity* self_entity;
};

class TypedProvider {
    public:
        virtual ~TypedProvider(){}

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


template <typename T>
class Consumer : public TypedProvider
{
    public:
        virtual ~Consumer(){}
        virtual void value(Atlas::Message::Element& value, const T& parent_value) const = 0;
        virtual const std::type_info* getType() const;
};

template <typename T>
const std::type_info* Consumer<T>::getType() const
{
    return nullptr;
}


template <typename T>
class ProviderBase {
    public:
        ProviderBase(Consumer<T>* consumer);
        virtual ~ProviderBase();
    protected:
        Consumer<T>* m_consumer;
};

template <typename T>
inline ProviderBase<T>::ProviderBase(Consumer<T>* consumer)
: m_consumer(consumer)
{
}

template <typename T>
inline ProviderBase<T>::~ProviderBase()
{
    delete m_consumer;
}

template <typename TProviding, typename TConsuming>
class ConsumingProviderBase : public ProviderBase<TProviding>, public Consumer<TConsuming>
{
    public:
        ConsumingProviderBase(Consumer<TProviding>* consumer);
        virtual ~ConsumingProviderBase(){};
        virtual const std::type_info* getType() const;
};

template <typename TProviding, typename TConsuming>
inline ConsumingProviderBase<TProviding, TConsuming>::ConsumingProviderBase(Consumer<TProviding>* consumer)
: ProviderBase<TProviding>(consumer)
{
}

template <typename TProviding, typename TConsuming>
inline const std::type_info* ConsumingProviderBase<TProviding, TConsuming>::getType() const
{
    if (this->m_consumer) {
        return this->m_consumer->getType();
    }
    return nullptr;
}


template <typename T>
class NamedAttributeProviderBase : public ProviderBase<T> {
    public:
        NamedAttributeProviderBase(Consumer<T>* consumer, const std::string& attribute_name);
    protected:
        const std::string m_attribute_name;
};

template <typename T>
inline NamedAttributeProviderBase<T>::NamedAttributeProviderBase(Consumer<T>* consumer, const std::string& attribute_name)
: ProviderBase<T>(consumer), m_attribute_name(attribute_name)
{
}

template <typename TProviding, typename TConsuming>
class ConsumingNamedAttributeProviderBase : public NamedAttributeProviderBase<TProviding>, public Consumer<TConsuming>
{
    public:
        ConsumingNamedAttributeProviderBase(Consumer<TProviding>* consumer, const std::string& attribute_name);
        virtual ~ConsumingNamedAttributeProviderBase(){};
        virtual const std::type_info* getType() const;
};

template <typename TProviding, typename TConsuming>
inline ConsumingNamedAttributeProviderBase<TProviding, TConsuming>::ConsumingNamedAttributeProviderBase(Consumer<TProviding>* consumer, const std::string& attribute_name)
: NamedAttributeProviderBase<TProviding>(consumer, attribute_name)
{
}

template <typename TProviding, typename TConsuming>
inline const std::type_info* ConsumingNamedAttributeProviderBase<TProviding, TConsuming>::getType() const
{
    if (this->m_consumer) {
        return this->m_consumer->getType();
    }
    return nullptr;
}


class FixedElementProvider : public Consumer<QueryContext> {
    public:
        FixedElementProvider(const Atlas::Message::Element& element);
        virtual void value(Atlas::Message::Element& value, const QueryContext& context) const;
    protected:
        const Atlas::Message::Element m_element;
};

class FixedTypeNodeProvider : public ConsumingProviderBase<TypeNode, QueryContext> {
    public:
        FixedTypeNodeProvider(Consumer<TypeNode>* consumer, const TypeNode& type);
        virtual void value(Atlas::Message::Element& value, const QueryContext& context) const;
        virtual const std::type_info* getType() const;
    protected:
        const TypeNode& m_type;
};
class MemoryProvider: public ConsumingProviderBase<Atlas::Message::Element, QueryContext>{
    public:
        MemoryProvider(Consumer<Atlas::Message::Element>* consumer);
        virtual void value(Atlas::Message::Element& value, const QueryContext&) const;
};

class EntityProvider : public ConsumingProviderBase<LocatedEntity, QueryContext> {
    public:
        EntityProvider(Consumer<LocatedEntity>* consumer);
        virtual void value(Atlas::Message::Element& value, const QueryContext& context) const;
        virtual const std::type_info* getType() const;
};

class SelfEntityProvider : public ConsumingProviderBase<LocatedEntity, QueryContext> {
    public:
        SelfEntityProvider(Consumer<LocatedEntity>* consumer);
        virtual void value(Atlas::Message::Element& value, const QueryContext& context) const;
        virtual const std::type_info* getType() const;
};

class EntityTypeProvider : public ConsumingProviderBase<TypeNode, LocatedEntity> {
    public:
        EntityTypeProvider(Consumer<TypeNode>* consumer);
        virtual void value(Atlas::Message::Element& value, const LocatedEntity& entity) const;
        virtual const std::type_info* getType() const;
};

//Provides integer ID of an entity
class EntityIdProvider : public Consumer<LocatedEntity>{
    public:
        virtual void value(Atlas::Message::Element& value, const LocatedEntity& entity) const;
};

class TypeNodeProvider : public Consumer<TypeNode>
{
    public:
        TypeNodeProvider(const std::string& attribute_name);
        virtual void value(Atlas::Message::Element& value, const TypeNode& type) const;
    private:
        const std::string m_attribute_name;
};

class OutfitEntityProvider : public ConsumingNamedAttributeProviderBase<LocatedEntity, OutfitProperty>
{
    public:
        OutfitEntityProvider(Consumer<LocatedEntity>* consumer, const std::string& attribute_name);
        virtual void value(Atlas::Message::Element& value, const OutfitProperty& prop) const;
};

class BBoxProvider : public ConsumingProviderBase<Atlas::Message::Element, BBoxProperty>
{
    public:
        enum class Measurement {
            HEIGHT, WIDTH, DEPTH, VOLUME, AREA
        };

        BBoxProvider(Consumer<Atlas::Message::Element>* consumer, Measurement measurement);
        virtual void value(Atlas::Message::Element& value, const BBoxProperty& prop) const;
    protected:
        Measurement m_measurement;
};

template <typename TProperty>
class PropertyProvider : public ConsumingNamedAttributeProviderBase<TProperty, LocatedEntity>
{
    public:
        PropertyProvider(Consumer<TProperty>* consumer, const std::string& attribute_name);
        virtual ~PropertyProvider(){}
        virtual void value(Atlas::Message::Element& value, const LocatedEntity& entity) const;
};

template <typename TProperty>
inline PropertyProvider<TProperty>::PropertyProvider(Consumer<TProperty>* consumer, const std::string& attribute_name)
: ConsumingNamedAttributeProviderBase<TProperty, LocatedEntity>(consumer, attribute_name)
{
}

template <typename TProperty>
inline void PropertyProvider<TProperty>::value(Atlas::Message::Element& value, const LocatedEntity& entity) const
{
    const TProperty* prop = entity.getPropertyClass<TProperty>(this->m_attribute_name);
    if (!prop) {
        return;
    }

    if (this->m_consumer) {
        this->m_consumer->value(value, *prop);
    } else {
        value = (void*)prop;
    }
}


class SoftPropertyProvider : public ConsumingNamedAttributeProviderBase<Atlas::Message::Element, LocatedEntity>
{
    public:
        SoftPropertyProvider(Consumer<Atlas::Message::Element>* consumer, const std::string& attribute_name);
        virtual void value(Atlas::Message::Element& value, const LocatedEntity& entity) const;
};

class MapProvider : public ConsumingNamedAttributeProviderBase<Atlas::Message::Element, Atlas::Message::Element>
{
    public:
        MapProvider(Consumer<Atlas::Message::Element>* consumer, const std::string& attribute_name);
        virtual void value(Atlas::Message::Element& value, const Atlas::Message::Element& parent_element) const;
};

class EntityRefProvider : public ConsumingNamedAttributeProviderBase<LocatedEntity, LocatedEntity>
{
    public:
        EntityRefProvider(Consumer<LocatedEntity>* consumer, const std::string& attribute_name);
        virtual void value(Atlas::Message::Element& value, const LocatedEntity& entity) const;
        virtual const std::type_info* getType() const;
};

///\brief This class retrieves a pointer to the m_contains property of a given entity
class ContainsProvider : public Consumer<LocatedEntity> {
    public:
        virtual void value(Atlas::Message::Element& value, const LocatedEntity& entity) const;
        virtual const std::type_info* getType() const;
};

///\brief This class uses container Consumer to retrieve a container and condition Predicate
///to check whether there exists an entity within the container that matches the given condition
class ContainsRecursiveFunctionProvider : public Consumer<QueryContext> {
    public:
        ContainsRecursiveFunctionProvider(Consumer<QueryContext>* container,
                                          Predicate* condition);
        virtual void value(Atlas::Message::Element& value,
                           const QueryContext& context) const;
    private:
        ///\brief Condition used to match entities within the container
        Predicate* m_condition;
        ///\brief A Consumer which must return LocatedEntitySet* i.e. entity.contains
        Consumer<QueryContext>* m_consumer;

        bool checkContainer(LocatedEntitySet* container) const;
};

class ProviderFactory {
    public:
        struct Segment {
            std::string delimiter;
            std::string attribute;
        };
        typedef std::list<Segment> SegmentsList;
        virtual ~ProviderFactory() {}
        virtual Consumer<QueryContext>* createProviders(SegmentsList segments) const;

    protected:
        FixedTypeNodeProvider* createFixedTypeNodeProvider(SegmentsList segments) const;
        EntityProvider* createEntityProvider(SegmentsList segments) const;
        SelfEntityProvider* createSelfEntityProvider(SegmentsList segments) const;
        OutfitEntityProvider* createOutfitEntityProvider(SegmentsList segments) const;
        BBoxProvider* createBBoxProvider(SegmentsList segments) const;
        Consumer<LocatedEntity>* createPropertyProvider(SegmentsList segments) const;
        MapProvider* createMapProvider(SegmentsList segments) const;
        TypeNodeProvider* createTypeNodeProvider(SegmentsList segments) const;
};

class MindProviderFactory: public ProviderFactory{
    public:
        virtual ~MindProviderFactory() {}
        virtual Consumer<QueryContext>* createProviders(SegmentsList segments) const;
    protected:
        MemoryProvider* createMemoryProvider(SegmentsList segments) const;
};

}

#endif /* PROVIDERS_H_ */
