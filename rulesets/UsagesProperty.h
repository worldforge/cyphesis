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

#include "external/pycxx/CXX/Objects.hxx"
#include "common/Property.h"
#include "UsageInstance.h"

#include <memory>


class UsagesProperty : public Property<Atlas::Message::MapType>
{
    public:

        static constexpr const char* property_name = "usages";


        void set(const Atlas::Message::Element& val) override;

        void install(LocatedEntity* owner, const std::string& name) override;

        void remove(LocatedEntity* owner, const std::string& name) override;

        HandlerResult operation(LocatedEntity* e, const Operation& op, OpVector& res) override;

    private:

        std::map<std::string, Usage> m_usages;

        HandlerResult use_handler(LocatedEntity* e, const Operation& op, OpVector& res);



};


#endif //CYPHESIS_USAGESPROPERTY_H
