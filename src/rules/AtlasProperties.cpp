// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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


#include "AtlasProperties.h"

#include "rules/LocatedEntity.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

IdProperty::IdProperty(const std::string& data) : PropertyBase(prop_flag_persistence_ephem),
                                                  m_data(data)
{
}

int IdProperty::get(Atlas::Message::Element& e) const
{
    e = m_data;
    return 0;
}

void IdProperty::set(const Atlas::Message::Element& e)
{
}

void IdProperty::add(const std::string& key,
                     Atlas::Message::MapType& ent) const
{
    ent[key] = m_data;
}

void IdProperty::add(const std::string& key, const RootEntity& ent) const
{
    ent->setId(m_data);
}

IdProperty* IdProperty::copy() const
{
    return new IdProperty(*this);
}

LocationProperty::LocationProperty(const LocatedEntity& entity)
        : m_data(entity)
{

}

int LocationProperty::get(Atlas::Message::Element& val) const
{
    if (m_data.m_parent) {
        val = m_data.m_parent->getId();
    }
    return 0;
}

void LocationProperty::set(const Atlas::Message::Element& val)
{
    //not allowed
}

void LocationProperty::add(const std::string& key, Atlas::Message::MapType& map) const
{
    if (m_data.m_parent) {
        map[key] = m_data.m_parent->getId();
    }
}

void LocationProperty::add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const
{
    if (m_data.m_parent) {
        ent->setAttr(key, m_data.m_parent->getId());
    }
}


LocationProperty* LocationProperty::copy() const
{
    return new LocationProperty(m_data);
}

const std::string& LocationProperty::data() const
{
    static std::string empty;
    return m_data.m_parent ? m_data.m_parent->getId() : empty;
}


NameProperty::NameProperty(unsigned int flags) : Property<std::string>(flags)
{
}

void NameProperty::add(const std::string& s, const RootEntity& ent) const
{
    ent->setName(m_data);
}

ContainsProperty::ContainsProperty(LocatedEntitySet& data) :
        PropertyBase(prop_flag_persistence_ephem), m_data(data)
{
}

int ContainsProperty::get(Element& e) const
{
    // FIXME Not sure if this is best. Why did we bother to virtualise
    // addToMessage() if we have to do this here?
    e = ListType();
    auto& contlist = e.List();
    for (auto& entry : m_data) {
        contlist.push_back(entry->getId());
    }
    return 0;
}

void ContainsProperty::set(const Element& e)
{
}

// We do not implement add, as it is probably not going to be used.

void ContainsProperty::add(const std::string& s, const RootEntity& ent) const
{
    if (!m_data.empty()) {
        auto& contains = ent->modifyContains();
        contains.clear();
        for (auto& entry : m_data) {
            contains.push_back(entry->getId());
        }
    }
}

ContainsProperty* ContainsProperty::copy() const
{
    return new ContainsProperty(*this);
}
