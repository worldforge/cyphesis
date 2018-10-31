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

#include "rules/simulation/BaseWorld.h"
#include "PlantedOnProperty.h"
#include "rules/LocatedEntity.h"

#include <Atlas/Objects/Entity.h>

int PlantedOnProperty::get(Atlas::Message::Element& val) const
{
    Atlas::Message::MapType map;
    if (m_data.entity) {
        map["$eid"] = m_data.entity->getId();
    }
    if (m_data.attachment) {
        map["attachment"] = *m_data.attachment;
    }
    val = map;
    return 0;
}

void PlantedOnProperty::set(const Atlas::Message::Element& val)
{
    m_data = parse(val);
}

void PlantedOnProperty::add(const std::string& val, Atlas::Message::MapType& mapValue) const
{
    Atlas::Message::MapType map;
    if (m_data.entity) {
        map["$eid"] = m_data.entity->getId();
    }
    if (m_data.attachment) {
        map["attachment"] = *m_data.attachment;
    }

    mapValue[val] = map;
}

void PlantedOnProperty::add(const std::string& val, const Atlas::Objects::Entity::RootEntity& ent) const
{
    Atlas::Message::MapType map;
    if (m_data.entity) {
        map["$eid"] = m_data.entity->getId();
    }
    if (m_data.attachment) {
        map["attachment"] = *m_data.attachment;
    }

    ent->setAttr(val, map);
}

PlantedOnProperty* PlantedOnProperty::copy() const
{
    return new PlantedOnProperty();
}

const PlantedOnProperty::Data& PlantedOnProperty::data() const
{
    return m_data;
}

PlantedOnProperty::Data& PlantedOnProperty::data()
{
    return m_data;
}

PlantedOnProperty::Data PlantedOnProperty::parse(const Atlas::Message::Element& val)
{
    Data data{};
    if (val.isMap()) {
        auto eidI = val.Map().find("$eid");
        if (eidI != val.Map().end() && eidI->second.isString()) {
            data.entity = WeakEntityRef(BaseWorld::instance().getEntity(eidI->second.String()));
        }
        auto pointI = val.asMap().find("attachment");
        if (pointI != val.Map().end() && pointI->second.isString()) {
            data.attachment = pointI->second.String();
        }
    }
    return data;
}
