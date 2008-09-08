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

#ifndef RULESETS_ACTIVE_PROPERTY_FACTORY_IMPL_H
#define RULESETS_ACTIVE_PROPERTY_FACTORY_IMPL_H

#include "ActivePropertyFactory.h"

#include "HandlerProperty.h"

/// \brief ActivePropertyFactory constructor
///
/// @param class_no Class number of the operation that will trigger this
/// Property.
/// @param handler Handler function called when the right operation arrives
template <typename T>
ActivePropertyFactory<T>::ActivePropertyFactory(int class_no, Handler handler)
                        : m_operationClassNo(class_no), m_handler(handler)
{
}

template <typename T>
PropertyBase * ActivePropertyFactory<T>::newProperty()
{
    return new HandlerProperty<T>(m_operationClassNo, m_handler);
}

/// \brief MultiActivePropertyFactory constructor
///
/// @param handlers Map of operation class that will trigger a handler to
/// the handler it will trigger.
template<class T>
MultiActivePropertyFactory<T>::MultiActivePropertyFactory(const HandlerMap &
                                                          handlers) :
                               m_handlers(handlers)
{
}

template <class T>
PropertyBase * MultiActivePropertyFactory<T>::newProperty()
{
    return new T(m_handlers);
}

#endif // RULESETS_ACTIVE_PROPERTY_FACTORY_IMPL_H
