// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

// $Id$

#ifndef COMMON_PROPERTY_FACTORY_IMPL_H
#define COMMON_PROPERTY_FACTORY_IMPL_H

#include "PropertyFactory.h"

template <class T>
PropertyBase * PropertyFactory<T>::newProperty()
{
    return new T();
}

template <class T>
PropertyFactory<T> * PropertyFactory<T>::duplicateFactory() const
{
    return new PropertyFactory<T>;
}

template <class T>
PropertyKit * PropertyFactory<T>::scriptPropertyFactory() const
{
    return 0;
}

template <typename Q>
PropertyBase * PropertyFactory<Property<Q>>::newProperty()
{
    return new Property<Q>();
}

template <typename Q>
PropertyFactory<Property<Q>> * PropertyFactory<Property<Q>>::duplicateFactory() const
{
    return new PropertyFactory<Property<Q>>;
}

template <class FactoryT>
class ScriptPropertyFactory;

template <typename Q>
PropertyKit * PropertyFactory<Property<Q>>::scriptPropertyFactory() const
{
    return new ScriptPropertyFactory<PropertyFactory<Property<Q>>>;
}

#endif // COMMON_PROPERTY_FACTORY_IMPL_H
