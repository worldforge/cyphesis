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


#include "Property_impl.h"
#include "Property.h"




/// \brief Classes that define properties on in world entities
///
/// Property classes handle the values of Atlas attributes on in
/// game entities, ensuring type safety, and encapsulating certain
/// behaviors related to the presence and value of the attribute.
/// A property instance can be associated with an Entity instance
/// or a class, so it should not store any data specific to any of
/// the Entity instances it has been applied to. When it is taking effect
/// on an Entity for the first time, PropertyBase::install() is called
/// to allow the property to do any setup required, such as install one
/// or more operation handlers. When the property value must be applied
/// to an Entity, PropertyBase::apply() is called so that any side effect
/// of the value can be taken care of.
/// \defgroup PropertyClasses Entity Property Classes

/// \brief Flags used to control properties
///
/// The base class PropertyBase has a flag member which can be used to
/// control or track the property in various ways. The constants in this
/// group define the masks for these flags.
/// \defgroup PropertyFlags Entity Property Flags

/// \brief Constructor called from classes which inherit from Property
/// @param flags default value for the Property flags
PropertyBase::PropertyBase(std::uint32_t flags) : m_flags(flags)
{
}

void PropertyBase::install(LocatedEntity*, const std::string& name)
{
}

void PropertyBase::install(TypeNode*, const std::string& name)
{
}

void PropertyBase::remove(LocatedEntity*, const std::string& name)
{
}

void PropertyBase::apply(LocatedEntity*)
{
}

void PropertyBase::add(const std::string& s,
                       Atlas::Message::MapType& ent) const
{
    get(ent[s]);
}

void PropertyBase::add(const std::string& s,
                       const Atlas::Objects::Entity::RootEntity& ent) const
{
    Atlas::Message::Element val;
    get(val);
    ent->setAttr(s, val);
}

HandlerResult PropertyBase::operation(LocatedEntity*,
                                      const Operation&,
                                      OpVector& res)
{
    return OPERATION_IGNORED;
}

bool PropertyBase::operator==(const PropertyBase& rhs) const
{
    Atlas::Message::Element thisElement;
    get(thisElement);
    Atlas::Message::Element thatElement;
    rhs.get(thatElement);
    return thisElement == thatElement;
}

bool PropertyBase::operator!=(const PropertyBase& rhs) const
{
    return !operator==(rhs);
}

template<>
void Property<int>::set(const Atlas::Message::Element& e)
{
    if (e.isInt()) {
        this->m_data = static_cast<int>(e.Int());
    }
}

template<>
void Property<long>::set(const Atlas::Message::Element& e)
{
    if (e.isInt()) {
        this->m_data = e.Int();
    }
}

template<>
void Property<long long>::set(const Atlas::Message::Element& e)
{
    if (e.isInt()) {
        this->m_data = e.Int();
    }
}

template<>
void Property<float>::set(const Atlas::Message::Element& e)
{
    if (e.isNum()) {
        this->m_data = static_cast<float>(e.asNum());
    }
}

template<>
void Property<double>::set(const Atlas::Message::Element& e)
{
    if (e.isNum()) {
        this->m_data = e.asNum();
    }
}

template<>
void Property<std::string>::set(const Atlas::Message::Element& e)
{
    if (e.isString()) {
        this->m_data = e.String();
    }
}

template<>
void Property<Atlas::Message::ListType>::set(const Atlas::Message::Element& e)
{
    if (e.isList()) {
        this->m_data = e.List();
    }
}

template<>
void Property<Atlas::Message::MapType>::set(const Atlas::Message::Element& e)
{
    if (e.isMap()) {
        this->m_data = e.Map();
    }
}

template<>
void Property<std::string>::add(const std::string& s,
                                Atlas::Message::MapType& ent) const
{
    if (!m_data.empty()) {
        ent[s] = m_data;
    }
}

SoftProperty::SoftProperty(Atlas::Message::Element data) :
        PropertyBase(0), m_data(std::move(data))
{
}

int SoftProperty::get(Atlas::Message::Element& val) const
{
    val = m_data;
    return 0;
}

void SoftProperty::set(const Atlas::Message::Element& val)
{
    m_data = val;
}

SoftProperty* SoftProperty::copy() const
{
    return new SoftProperty(*this);
}

Atlas::Message::Element& SoftProperty::data()
{
    return m_data;
}

const Atlas::Message::Element& SoftProperty::data() const
{
    return m_data;
}


int BoolProperty::get(Atlas::Message::Element& ent) const
{
    ent = m_flags.hasFlags(prop_flag_bool) ? 1 : 0;
    return 0;
}

void BoolProperty::set(const Atlas::Message::Element& ent)
{
    if (ent.isInt()) {
        if (ent.Int() == 0) {
            m_flags.removeFlags(prop_flag_bool);
        } else {
            m_flags.addFlags(prop_flag_bool);
        }
    }
}

BoolProperty* BoolProperty::copy() const
{
    return new BoolProperty(*this);
}

bool BoolProperty::isTrue() const
{
    return m_flags.hasFlags(prop_flag_bool);
}


template
class Property<int>;

template
class Property<long>;

template
class Property<long long>;

template
class Property<float>;

template
class Property<double>;

template
class Property<std::string>;

template
class Property<Atlas::Message::ListType>;

template
class Property<Atlas::Message::MapType>;

template<> const std::string Property<int>::property_atlastype = "int";
template<> const std::string Property<long>::property_atlastype = "int";
template<> const std::string Property<long long>::property_atlastype = "int";
template<> const std::string Property<float>::property_atlastype = "float";
template<> const std::string Property<double>::property_atlastype = "float";
template<> const std::string Property<std::string>::property_atlastype = "string";
template<> const std::string Property<Atlas::Message::ListType>::property_atlastype = "list";
template<> const std::string Property<Atlas::Message::MapType>::property_atlastype = "map";

std::uint32_t PropertyBase::flagsForPropertyName(const std::string& name)
{
    if (name.size() > 1 && name[0] == '_' && name[1] == '_') {
        return prop_flag_visibility_private;
    } else if (!name.empty() && name[0] == '_') {
        return prop_flag_visibility_protected;
    }
    return 0;
}

bool PropertyBase::isValidName(const std::string& name)
{
    if (name.empty() || name.size() > 32) {
        return false;
    }
    for (auto& character : name) {
        if (std::isalnum(character) || character == '_' || character == '$' || character == '-') {
            continue;
        }
        return false;
    }
    return true;
}

