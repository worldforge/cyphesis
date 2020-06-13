/*
 Copyright (C) 2020 Erik Ogenvik

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

#include "ProviderFactory.h"
#include "Providers.h"
#include "Filter.h"
namespace EntityFilter {

    std::shared_ptr<Consumer<QueryContext>> ProviderFactory::createProvider(Segment segment) const
    {
        return createProviders(SegmentsList{std::move(segment)});
    }

    std::shared_ptr<Consumer<QueryContext>> ProviderFactory::createProviders(SegmentsList segments) const
    {
        if (!segments.empty()) {
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
            } else if (first_attribute == "entity_location") {
                return createEntityProvider<EntityLocationProvider>(std::move(segments));
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

}