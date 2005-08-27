// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "Property_impl.h"

/// \brief Constructor called from classes which inherit from Property
/// @param flags default value for the Property flags
PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::add(const std::string & s, Atlas::Message::MapType & ent)
{
    get(ent[s]);
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent)
{
    Atlas::Message::Element val;
    get(val);
    ent->setAttr(s, val);
}

template<>
void Property<int>::set(const Atlas::Message::Element & e)
{
    if (e.isInt()) {
        m_data = e.asInt();
    }
}

template<>
void Property<long>::set(const Atlas::Message::Element & e)
{
    if (e.isInt()) {
        m_data = e.asInt();
    }
}

template<>
void Property<float>::set(const Atlas::Message::Element & e)
{
    if (e.isNum()) {
        m_data = e.asNum();
    }
}

template<>
void Property<double>::set(const Atlas::Message::Element & e)
{
    if (e.isNum()) {
        m_data = e.asNum();
    }
}

template<>
void Property<std::string>::set(const Atlas::Message::Element & e)
{
    if (e.isString()) {
        m_data = e.asString();
    }
}

template<>
void Property<std::string>::add(const std::string & s,
                                Atlas::Message::MapType & ent)
{
    if (!m_data.empty()) {
        ent[s] = m_data;
    }
}

template class Property<int>;
template class Property<long>;
template class Property<float>;
template class Property<double>;
template class Property<std::string>;

template class ImmutableProperty<std::string>;
