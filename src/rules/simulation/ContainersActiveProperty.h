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

#ifndef CYPHESIS_CONTAINERSACTIVEPROPERTY_H
#define CYPHESIS_CONTAINERSACTIVEPROPERTY_H

#include "common/Property.h"
#include "rules/LocatedEntity.h"
#include <set>

/**
 * Records the containers an entity currently have opened. This works closely with ContainerAccessProperty.
 * While the ContainerAccessProperty acts on the container itself, this property is placed on the entity
 * that has it opened. Thus it's mainly meant for clients, to allow them to see which containers are open.
 */
class ContainersActiveProperty : public PropertyBase
{
    public:

        static constexpr const char* property_name = "_containers_active";
        static constexpr const char* property_atlastype = "list";

        ContainersActiveProperty();

        int get(Atlas::Message::Element&) const override;

        void set(const Atlas::Message::Element&) override;

        ContainersActiveProperty* copy() const override;

        void apply(LocatedEntity&) override;

        const std::set<std::string>& getActiveContainers() const
        {
            return m_activeContainers;
        };

        std::set<std::string>& getActiveContainers()
        {
            return m_activeContainers;
        };

        bool hasContainer(const std::string& containerId) const;
    protected:

        ContainersActiveProperty(const ContainersActiveProperty& rhs) = default;

        std::set<std::string> m_activeContainers;

};


#endif //CYPHESIS_CONTAINERSACTIVEPROPERTY_H
