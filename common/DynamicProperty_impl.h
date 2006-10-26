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

// $Id: DynamicProperty_impl.h,v 1.3 2006-10-26 00:48:03 alriddoch Exp $

#ifndef COMMON_DYNAMIC_PROPERTY_IMPL_H
#define COMMON_DYNAMIC_PROPERTY_IMPL_H

#include "DynamicProperty.h"

template <typename T>
DynamicProperty<T>::DynamicProperty() : Property<T>(m_store, 0)
{
}

template <class P, typename T>
Dynamic<P, T>::Dynamic() : P(m_store, 0)
{
}

#endif // COMMON_DYNAMIC_PROPERTY_IMPL_H
