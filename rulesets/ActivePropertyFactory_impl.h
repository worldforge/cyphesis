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

// $Id: ActivePropertyFactory_impl.h,v 1.7 2007-12-20 18:47:28 alriddoch Exp $

#ifndef RULESETS_ACTIVE_PROPERTY_FACTORY_IMPL_H
#define RULESETS_ACTIVE_PROPERTY_FACTORY_IMPL_H

#include "ActivePropertyFactory.h"
#include "Entity.h"

/// \brief ActivePropertyFactory constructor
///
/// @param class_no Class number of the operation that will trigger this
/// Property.
/// @param handler Handler function called when the right operation arrives
template <class T>
ActivePropertyFactory<T>::ActivePropertyFactory(int class_no, Handler handler)
                        : m_operationClassNo(class_no), m_handler(handler)
{
}

template <class T>
PropertyBase * ActivePropertyFactory<T>::newProperty(Entity * property_owner)
{
    // Install operation handler on property_owner
    property_owner->installHandler(m_operationClassNo, m_handler);

    return new T();
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
PropertyBase * MultiActivePropertyFactory<T>::newProperty(Entity * owner)
{
    HandlerMap::const_iterator I = m_handlers.begin();
    HandlerMap::const_iterator Iend = m_handlers.end();
    for (; I != Iend; ++I) {
        owner->installHandler(I->first, I->second);
    }

    return new T();
}

#endif // RULESETS_ACTIVE_PROPERTY_FACTORY_IMPL_H
