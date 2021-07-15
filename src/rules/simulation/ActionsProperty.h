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

#ifndef CYPHESIS_ACTIONSPROPERTY_H
#define CYPHESIS_ACTIONSPROPERTY_H

#include "common/Property.h"
#include "modules/Ref.h"
#include <boost/optional.hpp>

class ActionsProperty : public PropertyBase
{
    public:

        static constexpr const char* property_atlastype = "map";
        static constexpr const char* property_name = "actions";


        struct Action
        {
            double startTime;
            boost::optional<double> endTime;
        };

        ActionsProperty();

        void install(LocatedEntity&, const std::string&) override;

        void remove(LocatedEntity&, const std::string& name) override;

        HandlerResult operation(LocatedEntity&,
                                const Operation&,
                                OpVector&) override;

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element& val) override;

        void enqueueTickOp(const LocatedEntity& entity, OpVector& res);

        void addAction(LocatedEntity& entity, OpVector& res, std::string actionName, Action action);

        void removeAction(LocatedEntity& entity, OpVector& res, std::string actionName);

    protected:

        std::map<std::string, Action> m_data;

        boost::optional<double> m_tickOutstanding;

        ActionsProperty(const ActionsProperty& rhs) = default;

        ActionsProperty* copy() const override;

        HandlerResult TickOperation(LocatedEntity& owner, const Operation& op, OpVector&);


};

#endif //CYPHESIS_ACTIONSPROPERTY_H
