// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "LocatedEntity.h"

#include "common/Property_impl.h"
#include "common/type_utils.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Entity::RootEntity;

template<>
int Property<IdList>::get(Element & e) const
{
    e = Atlas::Message::ListType();
    idListasObject(m_data, e.asList());
    return 0;
}

template<>
void Property<IdList>::set(const Element & e)
{
    if (e.isList()) {
        if (idListFromAtlas(e.asList(), this->m_data) != 0) {
            this->m_data.clear();
        }
    }
}

template<>
void Property<IdList>::add(const std::string & s, MapType & ent) const
{
    if (!m_data.empty()) {
        get(ent[s]);
    }
}

template<>
void Property<IdList>::add(const std::string & s, const RootEntity & ent) const
{
    if (!m_data.empty()) {
        ListType list;
        idListasObject(m_data, list);
        ent->setAttr(s, list);
    }
}

template class Property<IdList>;
