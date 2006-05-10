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

#include "Entity.h"

#include "Container.h"

#include "common/Property_impl.h"
#include "common/type_utils.h"

#include "physics/BBox.h"

#include <wfmath/atlasconv.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Entity::RootEntity;

template<>
bool ImmutableProperty<BBox>::get(Element & e) const
{
    e = m_data.toAtlas();
    return true;
}

template<>
void Property<BBox>::set(const Element & e)
{
    if (e.isList() && (e.asList().size() > 2)) {
        m_modData.fromAtlas(e.asList());
    }
}

template<>
void ImmutableProperty<BBox>::add(const std::string & s, MapType & ent) const
{
    if (m_data.isValid()) {
        ent[s] = m_data.toAtlas();
    }
}

template<>
void ImmutableProperty<BBox>::add(const std::string & s, const RootEntity & ent) const
{
    if (m_data.isValid()) {
        ent->setAttr(s, m_data.toAtlas());
    }
}

template<>
bool ImmutableProperty<IdList>::get(Element & e) const
{
    e = Atlas::Message::ListType();
    idListasObject(m_data, e.asList());
    return true;
}

template<>
void Property<IdList>::set(const Element & e)
{
    if (e.isList()) {
        idListFromAtlas(e.asList(), m_modData);
    }
}

template<>
void ImmutableProperty<IdList>::add(const std::string & s, MapType & ent) const
{
    if (!m_data.empty()) {
        get(ent[s]);
    }
}

template<>
void ImmutableProperty<IdList>::add(const std::string & s, const RootEntity & ent) const
{
    if (!m_data.empty()) {
        ListType list;
        idListasObject(m_data, list);
        ent->setAttr(s, list);
    }
}

template<>
bool ImmutableProperty<EntitySet>::get(Element & e) const
{
    e = ListType();
    ListType & contlist = e.asList();
    EntitySet::const_iterator Iend = m_data.end();
    for (EntitySet::const_iterator I = m_data.begin(); I != Iend; ++I) {
        contlist.push_back((*I)->getId());
    }
    return true;
}

template<>
void ImmutableProperty<EntitySet>::add(const std::string & s,
                                       MapType & ent) const
{
    if (!m_data.empty()) {
        get(ent[s]);
    }
}

template<>
void ImmutableProperty<EntitySet>::add(const std::string & s,
                                       const RootEntity & ent) const
{
    if (!m_data.empty()) {
        Element v;
        get(v);
        ent->setAttr(s, v);
    }
}

template<>
bool ImmutableProperty<Container>::get(Element & e) const
{
    // FIXME Not sure if this is best. Why did we bother to virtualise
    // addToMessage() if we have to do this here?
    e = ListType();
    ListType & contlist = e.asList();
    Container::const_iterator Iend = m_data.end();
    for (Container::const_iterator I = m_data.begin(); I != Iend; ++I) {
        contlist.push_back((*I)->getId());
    }
    return true;
}

template<>
void ImmutableProperty<Container>::add(const std::string & s,
                                       MapType & ent) const
{
    if (!m_data.empty()) {
        m_data.addToMessage(s, ent);
    }
}

template<>
void ImmutableProperty<Container>::add(const std::string & s,
                                       const RootEntity & ent) const
{
    if (!m_data.empty()) {
        m_data.addToEntity(s, ent);
    }
}

template<>
void SignalProperty<BBox>::set(const Element & e)
{
    if (e.isList() && (e.asList().size() > 2)) {
        m_modData.fromAtlas(e.asList());
        modified.emit();
    }
}

template class Property<BBox>;
template class Property<IdList>;

template class ImmutableProperty<EntitySet>;
template class ImmutableProperty<Container>;

template class SignalProperty<BBox>;
