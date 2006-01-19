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

#ifndef COMMON_PROPERTY_IMPL_H
#define COMMON_PROPERTY_IMPL_H

#include "Property.h"

#include <Atlas/Objects/RootEntity.h>

/// \brief Constructor for immutable Propertys
template <typename T>
ImmutableProperty<T>::ImmutableProperty(const T & data, unsigned int flags) :
                      PropertyBase(flags), m_data(data)
{
}

template <typename T>
void ImmutableProperty<T>::get(Atlas::Message::Element & e) const
{
    e = m_data;
}

template <typename T>
void ImmutableProperty<T>::set(const Atlas::Message::Element & e)
{
}

// The following two are obsolete.
template <typename T>
void ImmutableProperty<T>::add(const std::string & s,
                               Atlas::Message::MapType & ent) const
{
    get(ent[s]);
}

template <typename T>
void ImmutableProperty<T>::add(const std::string & s,
                               const Atlas::Objects::Entity::RootEntity & ent) const
{
    Atlas::Message::Element val;
    get(val);
    ent->setAttr(s, val);
}

/// \brief Constructor for standard Propertys
template <typename T>
Property<T>::Property(T & data, unsigned int flags) :
             ImmutableProperty<T>(data, flags), m_modData(data)
{
}

template <typename T>
void Property<T>::set(const Atlas::Message::Element & e)
{
    m_modData = e;
}

template <typename T>
SignalProperty<T>::SignalProperty(T & data, unsigned int flags) :
                   Property<T>(data, flags)
{
}

template <typename T>
void SignalProperty<T>::set(const Atlas::Message::Element & e)
{
    this->m_modData = e;
    modified.emit();
}

#endif // COMMON_PROPERTY_IMPL_H
