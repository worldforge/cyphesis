// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_PROPERTY_IMPL_H
#define COMMON_PROPERTY_IMPL_H

#include "Property.h"

template <typename T>
Property<T>::Property(T & data, unsigned int flags) : PropertyBase(flags),
                                                      m_data(data)
{
}

template <typename T>
void Property<T>::get(Atlas::Message::Element & e)
{
    e = m_data;
}

template <typename T>
void Property<T>::set(const Atlas::Message::Element & e)
{
    m_data = e;
}

#endif // COMMON_PROPERTY_IMPL_H
