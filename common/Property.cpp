// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "Property_impl.h"

template class Property<int>;
template class Property<long>;
template class Property<float>;
template class Property<double>;
template class Property<std::string>;

template class ImmutableProperty<std::string>;

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void Property<int>::set(const Atlas::Message::Element & e)
{
    m_data = e.asInt();
}

void Property<long>::set(const Atlas::Message::Element & e)
{
    m_data = e.asInt();
}

void Property<float>::set(const Atlas::Message::Element & e)
{
    m_data = e.asFloat();
}

void Property<double>::set(const Atlas::Message::Element & e)
{
    m_data = e.asFloat();
}

void Property<std::string>::set(const Atlas::Message::Element & e)
{
    m_data = e.asString();
}

void Property<std::string>::add(const std::string & s,
                                Atlas::Message::MapType & ent)
{
    if (!m_data.empty()) {
        ent[s] = m_data;
    }
}
