#include <utility>

#include <utility>

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

#ifdef HAVE_CONFIG_H
#endif

#include "Providers.h"

#include "common/TypeNode.h"

#include <algorithm>
#include <wfmath/atlasconv.h>

namespace EntityFilter {


    FixedElementProvider::FixedElementProvider(const Atlas::Message::Element& element)
        : m_element(element)
    {
    }

    void FixedElementProvider::value(Atlas::Message::Element& value, const QueryContext& context) const
    {
        value = m_element;
    }

    FixedTypeNodeProvider::FixedTypeNodeProvider(std::shared_ptr<Consumer<TypeNode>> consumer, const TypeNode& type)
        : ConsumingProviderBase<TypeNode, QueryContext>(std::move(consumer)), m_type(type)
    {
    }

    void FixedTypeNodeProvider::value(Atlas::Message::Element& value, const QueryContext& context) const
    {
        if (m_consumer) {
            m_consumer->value(value, m_type);
        } else {
            value = (void*) (&m_type);
        }
    }

    const std::type_info* FixedTypeNodeProvider::getType() const
    {
        if (m_consumer) {
            return m_consumer->getType();
        } else {
            return &typeid(const TypeNode*);
        }
    }

    DynamicTypeNodeProvider::DynamicTypeNodeProvider(std::shared_ptr<Consumer<TypeNode>> consumer, const std::string& type)
        : ConsumingProviderBase<TypeNode, QueryContext>(std::move(consumer)), m_type(type)
    {
    }

    void DynamicTypeNodeProvider::value(Atlas::Message::Element& value, const QueryContext& context) const
    {
        if (context.type_lookup_fn) {
            auto type = context.type_lookup_fn(m_type);

            if (m_consumer && type) {
                m_consumer->value(value, *type);
            } else {
                value = (void*) type;
            }
        }
    }

    const std::type_info* DynamicTypeNodeProvider::getType() const
    {
        if (m_consumer) {
            return m_consumer->getType();
        } else {
            return &typeid(const TypeNode*);
        }
    }

    MemoryProvider::MemoryProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer)
        : ConsumingProviderBase<Atlas::Message::Element, QueryContext>(std::move(consumer))
    {

    }

    void MemoryProvider::value(Atlas::Message::Element& value, const QueryContext& context) const
    {
        if (context.memory_lookup_fn) {
            const Atlas::Message::MapType& memory = context.memory_lookup_fn(context.entity.getId());

            if (m_consumer) {
                m_consumer->value(value, memory);
                return;
            } else {
                value = memory;
            }
        }
    }

    EntityProvider::EntityProvider(std::shared_ptr<Consumer<LocatedEntity>> consumer)
        : ConsumingProviderBase<LocatedEntity, QueryContext>(std::move(consumer))
    {
    }

    void EntityProvider::value(Atlas::Message::Element& value, const QueryContext& context) const
    {
        if (m_consumer) {
            m_consumer->value(value, context.entity);
        } else {
            value = (void*) (&context.entity);
        }
    }

    const std::type_info* EntityProvider::getType() const
    {
        if (m_consumer) {
            return m_consumer->getType();
        } else {
            return &typeid(const LocatedEntity*);
        }
    }

    ActorProvider::ActorProvider(std::shared_ptr<Consumer<LocatedEntity>> consumer)
        : EntityProvider(std::move(consumer))
    {
    }

    void ActorProvider::value(Atlas::Message::Element& value, const QueryContext& context) const
    {
        if (m_consumer && context.actor) {
            m_consumer->value(value, *context.actor);
        } else {
            value = (void*) (context.actor);
        }
    }

    ToolProvider::ToolProvider(std::shared_ptr<Consumer<LocatedEntity>> consumer)
        : EntityProvider(std::move(consumer))
    {
    }

    void ToolProvider::value(Atlas::Message::Element& value, const QueryContext& context) const
    {
        if (m_consumer && context.tool) {
            m_consumer->value(value, *context.tool);
        } else {
            value = (void*) (context.tool);
        }
    }

    ChildProvider::ChildProvider(std::shared_ptr<Consumer<LocatedEntity>> consumer)
        : EntityProvider(std::move(consumer))
    {
    }

    void ChildProvider::value(Atlas::Message::Element& value, const QueryContext& context) const
    {
        if (m_consumer && context.child) {
            m_consumer->value(value, *context.child);
        } else {
            value = (void*) (context.child);
        }
    }

    SelfEntityProvider::SelfEntityProvider(std::shared_ptr<Consumer<LocatedEntity>> consumer) :
        ConsumingProviderBase<LocatedEntity, QueryContext>(std::move(consumer))
    {
    }

    void SelfEntityProvider::value(Atlas::Message::Element& value,
                                   const QueryContext& context) const
    {
        if (!context.self_entity) {
            return;
        } else if (m_consumer) {
            return m_consumer->value(value, *context.self_entity);
        } else {
            value = (void*) (context.self_entity);
        }
    }

    const std::type_info* SelfEntityProvider::getType() const
    {
        if (m_consumer) {
            return m_consumer->getType();
        } else {
            return &typeid(const LocatedEntity*);
        }
    }

    EntityTypeProvider::EntityTypeProvider(std::shared_ptr<Consumer<TypeNode>> consumer)
        : ConsumingProviderBase<TypeNode, LocatedEntity>(std::move(consumer))
    {

    }

    void EntityTypeProvider::value(Atlas::Message::Element& value, const LocatedEntity& entity) const
    {
        if (!entity.getType()) {
            return;
        }

        if (m_consumer) {
            m_consumer->value(value, *entity.getType());
        } else {
            value = (void*) (entity.getType());
        }
    }

    const std::type_info* EntityTypeProvider::getType() const
    {
        if (m_consumer) {
            return m_consumer->getType();
        } else {
            return &typeid(const TypeNode*);
        }
    }

    void EntityIdProvider::value(Atlas::Message::Element& value,
                                 const LocatedEntity& entity) const
    {
        value = Atlas::Message::Element(entity.getIntId());
    }

    TypeNodeProvider::TypeNodeProvider(const std::string& attribute_name)
        : m_attribute_name(attribute_name)
    {

    }

    void TypeNodeProvider::value(Atlas::Message::Element& value, const TypeNode& type) const
    {
        if (m_attribute_name == "name") {
            value = type.name();
        }
    }

    BBoxProvider::BBoxProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer, Measurement measurement)
        : ConsumingProviderBase<Atlas::Message::Element, LocatedEntity>(std::move(consumer)), m_measurement(measurement)
    {

    }

    void BBoxProvider::value(Atlas::Message::Element& value, const LocatedEntity& entity) const
    {
        Atlas::Message::Element data;
        if (entity.getAttr("bbox", data) == 0) {
            BBox bbox;
            bbox.fromAtlas(data);
            switch (m_measurement) {
                case Measurement::WIDTH:
                    value = bbox.highCorner().x() - bbox.lowCorner().x();
                    break;
                case Measurement::DEPTH:
                    value = bbox.highCorner().z() - bbox.lowCorner().z();
                    break;
                case Measurement::HEIGHT:
                    value = bbox.highCorner().y() - bbox.lowCorner().y();
                    break;
                case Measurement::VOLUME:
                    value = (bbox.highCorner().x() - bbox.lowCorner().x()) * (bbox.highCorner().y() - bbox.lowCorner().y()) * (bbox.highCorner().z() - bbox.lowCorner().z());
                    break;
                case Measurement::AREA:
                    value = (bbox.highCorner().x() - bbox.lowCorner().x()) * (bbox.highCorner().z() - bbox.lowCorner().z());
                    break;
            }
        }
    }


    SoftPropertyProvider::SoftPropertyProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer, const std::string& attribute_name) :
        ConsumingNamedAttributeProviderBase<Atlas::Message::Element, LocatedEntity>(std::move(consumer), attribute_name)
    {
    }

    void SoftPropertyProvider::value(Atlas::Message::Element& value, const LocatedEntity& entity) const
    {
        auto prop = entity.getProperty(m_attribute_name);
        if (!prop) {
            return;
        }
        if (m_consumer) {
            Atlas::Message::Element propElem;
            prop->get(propElem);
            m_consumer->value(value, propElem);
        } else {
            prop->get(value);
        }
    }

    MapProvider::MapProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer, const std::string& attribute_name) :
        ConsumingNamedAttributeProviderBase<Atlas::Message::Element, Atlas::Message::Element>(std::move(consumer), attribute_name)
    {
    }

    void MapProvider::value(Atlas::Message::Element& value, const Atlas::Message::Element& parent_element) const
    {
        if (!parent_element.isMap()) {
            return;
        }
        auto I = parent_element.Map().find(m_attribute_name);
        if (I == parent_element.Map().end()) {
            return;
        }
        if (m_consumer) {
            m_consumer->value(value, I->second);
        } else {
            value = I->second;
        }
    }

    void ContainsProvider::value(Atlas::Message::Element& value,
                                 const LocatedEntity& entity) const
    {
        auto container = entity.m_contains;
        if (container) {
            value = container;
        }
    }

    const std::type_info* ContainsProvider::getType() const
    {
        return &typeid(const LocatedEntitySet*);
    }

    ContainsRecursiveFunctionProvider::ContainsRecursiveFunctionProvider(std::shared_ptr<Consumer<QueryContext>> container,
                                                                         std::shared_ptr<Predicate> condition) :
        m_condition(std::move(condition)), m_consumer(std::move(container))
    {
        if (m_consumer->getType() != &typeid(const LocatedEntitySet*)) {
            throw std::invalid_argument(
                "first argument of contains_recursive must return a valid entity container");
        }
    }

    void ContainsRecursiveFunctionProvider::value(Atlas::Message::Element& value,
                                                  const QueryContext& context) const
    {
        Atlas::Message::Element container;
        m_consumer->value(container, context);
        if (container.isPtr()) {
            value = checkContainer((LocatedEntitySet*) container.Ptr(), context);
        } else {
            value = false;
        }
    }

    bool ContainsRecursiveFunctionProvider::checkContainer(LocatedEntitySet* container,
                                                           const QueryContext& context) const
    {
        QueryContext childContext = context;
        for (auto& item : *container) {
            childContext.child = item.get();
            if (m_condition->isMatch(childContext)) {
                return true;
            } else {
                //If an item we're looking at also contains other items - check them too using recursion
                if (item->m_contains && !item->m_contains->empty()) {
                    if (this->checkContainer(item->m_contains, context)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    std::shared_ptr<Consumer<QueryContext>> ProviderFactory::createProvider(Segment segment) const
    {
        return createProviders(SegmentsList{std::move(segment)});
    }

    std::shared_ptr<Consumer<QueryContext>> ProviderFactory::createProviders(SegmentsList segments) const
    {
        if (!segments.empty()) {
            auto front = segments.front();
            auto& first_attribute = segments.front().attribute;
            if (first_attribute == "entity") {
                return createEntityProvider<EntityProvider>(std::move(segments));
            } else if (first_attribute == "self") {
                return createSelfEntityProvider(std::move(segments));
            } else if (first_attribute == "types") {
                return createDynamicTypeNodeProvider(std::move(segments));
            } else if (first_attribute == "actor") {
                return createEntityProvider<ActorProvider>(std::move(segments));
            } else if (first_attribute == "tool") {
                return createEntityProvider<ToolProvider>(std::move(segments));
            } else if (first_attribute == "child") {
                return createEntityProvider<ChildProvider>(std::move(segments));
            } else if (first_attribute == "memory") {
                return createMemoryProvider(std::move(segments));
            }
        }
        return nullptr;
    }


    std::shared_ptr<MemoryProvider> ProviderFactory::createMemoryProvider(SegmentsList segments) const
    {
        if (segments.empty()) {
            return nullptr;
        }
        segments.pop_front();
        return std::make_shared<MemoryProvider>(createMapProvider(std::move(segments)));
    }

    std::shared_ptr<Consumer<QueryContext>> ProviderFactory::createGetEntityFunctionProvider(std::shared_ptr<Consumer<QueryContext>> entity_provider, SegmentsList segments) const
    {
        if (segments.empty()) {
            return std::make_shared<GetEntityFunctionProvider>(std::move(entity_provider), nullptr);
        }
        return std::make_shared<GetEntityFunctionProvider>(std::move(entity_provider), createPropertyProvider(std::move(segments)));
    }

    std::shared_ptr<Consumer<QueryContext>> ProviderFactory::createSimpleGetEntityFunctionProvider(std::shared_ptr<Consumer<QueryContext>> entity_provider) const
    {
        return std::make_shared<GetEntityFunctionProvider>(std::move(entity_provider), nullptr);
    }

    std::shared_ptr<DynamicTypeNodeProvider> ProviderFactory::createDynamicTypeNodeProvider(SegmentsList segments) const
    {
        if (segments.empty()) {
            return nullptr;
        }
        segments.pop_front();
        //A little hack here to avoid calling yet another method.
        if (segments.empty()) {
            return nullptr;
        }
        auto type = segments.front().attribute;
        segments.pop_front();
        return std::make_shared<DynamicTypeNodeProvider>(createTypeNodeProvider(std::move(segments)), type);
    }

    template<typename T>
    std::shared_ptr<T> ProviderFactory::createEntityProvider(SegmentsList segments) const
    {
        if (segments.empty()) {
            return nullptr;
        }
        segments.pop_front();
        if (segments.empty()) {
            return std::make_shared<T>(nullptr);
        }
        return std::make_shared<T>(createPropertyProvider(std::move(segments)));
    }

    std::shared_ptr<SelfEntityProvider> ProviderFactory::createSelfEntityProvider(SegmentsList segments) const
    {
        if (segments.empty()) {
            return nullptr;
        }
        segments.pop_front();
        return std::make_shared<SelfEntityProvider>(createPropertyProvider(std::move(segments)));
    }

    std::shared_ptr<Consumer<LocatedEntity>> ProviderFactory::createPropertyProvider(SegmentsList segments) const
    {
        if (segments.empty()) {
            return nullptr;
        }

        auto segment = std::move(segments.front());
        auto attr = segment.attribute;

        segments.pop_front();

        if (segment.delimiter == ":") {
            return std::make_shared<SoftPropertyProvider>(createMapProvider(std::move(segments)), attr);
        } else {

            if (attr == "type") {
                return std::make_shared<EntityTypeProvider>(createTypeNodeProvider(std::move(segments)));
            } else if (attr == "id") {
                return std::make_shared<EntityIdProvider>();
            } else if (attr == "bbox") {
                return createBBoxProvider(std::move(segments));
            } else if (attr == "contains") {
                return std::make_shared<ContainsProvider>();
            } else {
                return std::make_shared<SoftPropertyProvider>(createMapProvider(std::move(segments)), attr);
            }
        }
    }

    std::shared_ptr<BBoxProvider> ProviderFactory::createBBoxProvider(SegmentsList segments) const
    {
        if (segments.empty()) {
            return nullptr;
        }

        auto& segment = segments.front();
        auto attr = segment.attribute;

        auto measurement_extractor = [&]() -> BBoxProvider::Measurement {
            if (attr == "width") {
                return BBoxProvider::Measurement::WIDTH;
            } else if (attr == "depth") {
                return BBoxProvider::Measurement::DEPTH;
            } else if (attr == "height") {
                return BBoxProvider::Measurement::HEIGHT;
            } else if (attr == "volume") {
                return BBoxProvider::Measurement::VOLUME;
            } else if (attr == "area") {
                return BBoxProvider::Measurement::AREA;
            }
            throw std::invalid_argument(String::compose("Could not compile query as '%1' isn't a valid measurement for a Bounding Box.", attr));
        };

        segments.pop_front();

        return std::make_shared<BBoxProvider>(createMapProvider(std::move(segments)), measurement_extractor());

    }

    std::shared_ptr<MapProvider> ProviderFactory::createMapProvider(SegmentsList segments) const
    {
        if (segments.empty()) {
            return nullptr;
        }

        auto segment = std::move(segments.front());
        auto attr = segment.attribute;

        segments.pop_front();

        return std::make_shared<MapProvider>(createMapProvider(std::move(segments)), attr);
    }

    std::shared_ptr<TypeNodeProvider> ProviderFactory::createTypeNodeProvider(SegmentsList segments) const
    {
        if (segments.empty()) {
            return nullptr;
        }

        auto& segment = segments.front();
        auto attr = segment.attribute;

        return std::make_shared<TypeNodeProvider>(attr);
    }


    GetEntityFunctionProvider::GetEntityFunctionProvider(std::shared_ptr<Consumer<QueryContext>> entity_provider, std::shared_ptr<Consumer<LocatedEntity>> consumer)
        : ConsumingProviderBase(std::move(consumer)),
          m_entity_provider(std::move(entity_provider))
    {

    }

    void GetEntityFunctionProvider::value(Atlas::Message::Element& value,
                                          const QueryContext& context) const
    {

        if (m_entity_provider && context.entity_lookup_fn) {
            Atlas::Message::Element innerValue;
            m_entity_provider->value(innerValue, context);
            if (innerValue.isMap()) {
                auto I = innerValue.Map().find("$eid");
                if (I != innerValue.Map().end() && I->second.isString()) {
                    auto entity = context.entity_lookup_fn(I->second.String());
                    if (entity) {
                        if (m_consumer) {
                            return m_consumer->value(value, *entity);
                        } else {
                            value = entity.get();
                        }
                    }
                }
            }
        }
    }

    const std::type_info* GetEntityFunctionProvider::getType() const
    {
        if (m_consumer) {
            return m_consumer->getType();
        } else {
            return &typeid(const LocatedEntity*);
        }
    }

}
