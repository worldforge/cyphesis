/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifndef CYPHESIS_USAGESPROPERTY_H
#define CYPHESIS_USAGESPROPERTY_H

#include "common/Property.h"
#include "entityfilter/Filter.h"

#include <memory>

namespace EntityFilter {
    class Filter;
}
class UsagesProperty : public Property<Atlas::Message::ListType>
{
    public:

        static constexpr const char* property_name = "usages";

        struct Usage {
            std::string operation;
            std::string task;
            std::unique_ptr<EntityFilter::Filter> filter;
            //Atlas::Message::MapType parameters;
        };

        void set(const Atlas::Message::Element & val) override;

        void install(TypeNode *, const std::string &) override;

        std::string findMatchingTask(const std::string& operation, LocatedEntity* target) const;

    private:

        std::map<std::string, std::vector<std::string>> m_targetsAndTheirOperations;
        std::vector<Usage> m_usages;





};


#endif //CYPHESIS_USAGESPROPERTY_H
