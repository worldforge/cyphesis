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


#ifndef TESTS_PROPERTY_STUB_IMPL_H
#define TESTS_PROPERTY_STUB_IMPL_H

#include "common/Property.h"

/// \brief Constructor for immutable Propertys
template <typename T>
Property<T>::Property(unsigned int flags) :
                      PropertyBase(flags)
{
}

template <typename T>
int Property<T>::get(Atlas::Message::Element & e) const
{
    return 0;
}

template <typename T>
void Property<T>::set(const Atlas::Message::Element & e)
{
}

// The following two are obsolete.
template <typename T>
void Property<T>::add(const std::string & s,
                               Atlas::Message::MapType & ent) const
{
}

template <typename T>
void Property<T>::add(const std::string & s,
                      const Atlas::Objects::Entity::RootEntity & ent) const
{
}

template <typename T>
Property<T> * Property<T>::copy() const
{
    return 0;
}

#endif // TESTS_PROPERTY_STUB_IMPL_H
