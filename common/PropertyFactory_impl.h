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

#ifndef COMMON_PROPERTY_FACTORY_IMPL_H
#define COMMON_PROPERTY_FACTORY_IMPL_H

#include "PropertyFactory.h"

template <class T>
PropertyBase * PropertyBuilder<T>::newProperty()
{
    // Classic properties need a reference to the value they are handling
    // at startup. Need a new dynamic property class to handle this.
    // Could be a template which inherits from its argument, and stores
    // the value, rather than just having a reference to it.
    // return new T();
    return 0;
}

#endif // COMMON_PROPERTY_FACTORY_IMPL_H
