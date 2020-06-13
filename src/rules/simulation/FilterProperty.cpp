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

#include "FilterProperty.h"
#include "rules/entityfilter/ProviderFactory.h"

FilterProperty::FilterProperty(const FilterProperty& rhs)
        : PropertyBase(rhs)
{
    if (rhs.m_data) {
        m_data = std::make_unique<EntityFilter::Filter>(*rhs.m_data);
    }
}


const EntityFilter::Filter* FilterProperty::getData() const
{
    return m_data.get();
}

FilterProperty* FilterProperty::copy() const
{
    return new FilterProperty(*this);
}

int FilterProperty::get(Atlas::Message::Element& val) const
{
    if (m_data) {
        val = m_data->getDeclaration();
    }
    return 0;
}

void FilterProperty::set(const Atlas::Message::Element& val)
{
    if (val.isString()) {
        m_data = std::make_unique<EntityFilter::Filter>(val.String(), EntityFilter::ProviderFactory());
    } else {
        m_data.reset();
    }
}
