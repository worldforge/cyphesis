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

#include "ContainerAccessProperty.h"
#include "ContainerDomain.h"
#include <Atlas/Objects/Entity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;


ContainerAccessProperty::ContainerAccessProperty(ContainerDomain& container) :
        PropertyBase(prop_flag_persistence_ephem),
        m_container(container)
{
}


int ContainerAccessProperty::get(Element& ent) const
{
    ListType list;
    for (auto& entity: m_container.getEntries()) {
        list.emplace_back(entity.first);
    }
    ent = list;

    return 0;
}

void ContainerAccessProperty::set(const Element& ent)
{

    std::vector<std::string> ids;
    if (ent.isList()) {
        for (auto entry: ent.List()) {
            if (entry.isString()) {
                ids.emplace_back(entry.String());
            }
        }
    }
    m_container.setObservers(std::move(ids));

}

ContainerAccessProperty* ContainerAccessProperty::copy() const
{
    return nullptr;
    //return new ContainerAccessProperty(*this);
}

