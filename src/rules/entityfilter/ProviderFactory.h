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

#ifndef CYPHESIS_PROVIDERFACTORY_H
#define CYPHESIS_PROVIDERFACTORY_H

#include "Filter.h"
#include "Providers.h"

namespace EntityFilter {

    class ProviderFactory
    {
        public:
            struct Segment
            {
                std::string delimiter;
                std::string attribute;
            };
            typedef std::list <Segment> SegmentsList;

            virtual ~ProviderFactory() = default;

            virtual std::shared_ptr <Consumer<QueryContext>> createProviders(SegmentsList segments) const;

            virtual std::shared_ptr <Consumer<QueryContext>> createProvider(Segment segment) const;

            virtual std::shared_ptr <Consumer<QueryContext>> createSimpleGetEntityFunctionProvider(std::shared_ptr <Consumer<QueryContext>> entity_provider) const;

            virtual std::shared_ptr <Consumer<QueryContext>> createGetEntityFunctionProvider(std::shared_ptr <Consumer<QueryContext>> entity_provider, SegmentsList segments) const;

        protected:

            std::shared_ptr <DynamicTypeNodeProvider> createDynamicTypeNodeProvider(SegmentsList segments) const;

            template<typename T>
            std::shared_ptr <T> createEntityProvider(SegmentsList segments) const;

            std::shared_ptr <SelfEntityProvider> createSelfEntityProvider(SegmentsList segments) const;

            std::shared_ptr <BBoxProvider> createBBoxProvider(SegmentsList segments) const;

            std::shared_ptr <Consumer<LocatedEntity>> createPropertyProvider(SegmentsList segments) const;

            std::shared_ptr <MapProvider> createMapProvider(SegmentsList segments) const;

            std::shared_ptr <TypeNodeProvider> createTypeNodeProvider(SegmentsList segments) const;

            std::shared_ptr <MemoryProvider> createMemoryProvider(SegmentsList segments) const;
    };

}

#endif //CYPHESIS_PROVIDERFACTORY_H
