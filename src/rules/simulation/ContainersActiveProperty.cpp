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

#include "ContainersActiveProperty.h"
#include <Atlas/Objects/Entity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

ContainersActiveProperty::ContainersActiveProperty()
        : PropertyBase(prop_flag_persistence_ephem)
{

}

int ContainersActiveProperty::get(Atlas::Message::Element& element) const
{
    ListType list;
    for (auto& entity: m_activeContainers) {
        list.emplace_back(entity);
    }
    element = list;
    return 0;
}

void ContainersActiveProperty::set(const Atlas::Message::Element& element)
{
}

ContainersActiveProperty* ContainersActiveProperty::copy() const
{
    //Don't copy values
    return new ContainersActiveProperty();
}

void ContainersActiveProperty::apply(LocatedEntity&)
{
}


bool ContainersActiveProperty::hasContainer(const std::string& containerId) const
{
    auto I = m_activeContainers.find(containerId);
    return I != m_activeContainers.end();
}
