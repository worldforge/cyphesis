/*
 Copyright (C) 2021 Erik Ogenvik

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

#include "Point3Property.h"
#include <wfmath/atlasconv.h>
#include "common/log.h"


Point3Property::Point3Property(const Point3Property& rhs) = default;

int Point3Property::get(Atlas::Message::Element& val) const
{
    if (m_data.isValid()) {
        val = m_data.toAtlas();
        return 0;
    }
    return 1;

}

void Point3Property::set(const Atlas::Message::Element& val)
{
    if (val.isList()) {
        try {
            m_data.fromAtlas(val.List());
        } catch (...) {
            m_data = {};
            log(ERROR, "Point3Property::set: Data was not in format which could be parsed into 3d point.");
        }
    } else if (val.isNone()) {
        m_data = {};
    } else {
        m_data = {};
        log(ERROR, "Point3Property::set: Data was not in format which could be parsed into 3d point.");
    }
}

Point3Property* Point3Property::copy() const
{
    return new Point3Property(*this);
}

void Point3Property::add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const
{
    if (m_data.isValid()) {
        PropertyBase::add(key, ent);
    }
}
