// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id: LocatedEntity.cpp,v 1.2 2007-12-02 20:53:51 alriddoch Exp $

#include "LocatedEntity.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

/// \brief LocatedEntity constructor
LocatedEntity::LocatedEntity(const std::string & id, long intId) :
                             BaseEntity(id, intId), m_refCount(0), m_seq(0)
{
}

LocatedEntity::~LocatedEntity()
{
    if (m_location.m_loc != 0) {
        // m_location.m_loc->decRef();
    }
}

/// \brief Check if this entity has a property with the given name
///
/// @param name Name of attribute to be checked
/// @return trye if this entity has an attribute with the name given
/// false otherwise
bool LocatedEntity::hasAttr(const std::string & name) const
{
    MapType::const_iterator J = m_attributes.find(name);
    if (J != m_attributes.end()) {
        return true;
    }
    return false;
    
}

/// \brief Get the value of an attribute
///
/// @param name Name of attribute to be retrieved
/// @param attr Reference used to store value
/// @return trye if this entity has an attribute with the name given
/// false otherwise
bool LocatedEntity::getAttr(const std::string & name, Element & attr) const
{
    MapType::const_iterator J = m_attributes.find(name);
    if (J != m_attributes.end()) {
        attr = J->second;
        return true;
    }
    return false;
}

/// \brief Set the value of an attribute
///
/// @param name Name of attribute to be changed
/// @param attr Value to be stored
void LocatedEntity::setAttr(const std::string & name, const Element & attr)
{
    MapType::iterator J = m_attributes.find(name);
    if (J == m_attributes.end()) {
        m_attributes[name] = attr;
        return;
    }
    J->second = attr;
}
