// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

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
