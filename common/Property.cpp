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

// $Id: Property.cpp,v 1.11 2006-10-26 00:48:03 alriddoch Exp $

#include "Property_impl.h"

/// \brief Constructor called from classes which inherit from Property
/// @param flags default value for the Property flags
PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
    get(ent[s]);
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
    Atlas::Message::Element val;
    get(val);
    ent->setAttr(s, val);
}

template<>
void Property<int>::set(const Atlas::Message::Element & e)
{
    if (e.isInt()) {
        m_modData = e.asInt();
    }
}

template<>
void Property<long>::set(const Atlas::Message::Element & e)
{
    if (e.isInt()) {
        m_modData = e.asInt();
    }
}

template<>
void Property<float>::set(const Atlas::Message::Element & e)
{
    if (e.isNum()) {
        m_modData = e.asNum();
    }
}

template<>
void Property<double>::set(const Atlas::Message::Element & e)
{
    if (e.isNum()) {
        m_modData = e.asNum();
    }
}

template<>
void Property<std::string>::set(const Atlas::Message::Element & e)
{
    if (e.isString()) {
        m_modData = e.String();
    }
}

template<>
void ImmutableProperty<std::string>::add(const std::string & s,
                                         Atlas::Message::MapType & ent) const
{
    if (!m_data.empty()) {
        ent[s] = m_data;
    }
}

template class Property<int>;
template class Property<long>;
template class Property<float>;
template class Property<double>;
template class Property<std::string>;

template class ImmutableProperty<std::string>;
