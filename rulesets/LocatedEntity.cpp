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

// $Id: LocatedEntity.cpp,v 1.8 2007-12-31 18:11:02 alriddoch Exp $

#include "LocatedEntity.h"

#include "Script.h"

#include "common/Property.h"
#include "common/TypeNode.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

/// \brief Set of attribute names which must not be changed
///
/// The attributes named are special and are modified using high level
/// operations, such as Move, not via Set operations, or assigned by
/// normal means.
std::set<std::string> LocatedEntity::m_immutable;

/// \brief Singleton accessor for immutables
///
/// The immutable attribute set m_immutables is initialised if necessary,
/// and a reference is returned.
const std::set<std::string> & LocatedEntity::immutables()
{
    if (m_immutable.empty()) {
        m_immutable.insert("parents");
        m_immutable.insert("pos");
        m_immutable.insert("loc");
        m_immutable.insert("velocity");
        m_immutable.insert("orientation");
        m_immutable.insert("contains");
    }
    return m_immutable;
}

/// \brief LocatedEntity constructor
LocatedEntity::LocatedEntity(const std::string & id, long intId) :
                             BaseEntity(id, intId), m_refCount(0),
                             m_seq(0), m_script(&noScript), m_type(0),
                             m_contains(new LocatedEntitySet)
{
}

LocatedEntity::~LocatedEntity()
{
    if (m_script != NULL && m_script != &noScript) {
        delete m_script;
    }
    if (m_location.m_loc != 0) {
        m_location.m_loc->decRef();
    }
    if (m_contains != 0) {
        delete m_contains;
    }
}

/// \brief Check if this entity has a property with the given name
///
/// @param name Name of attribute to be checked
/// @return trye if this entity has an attribute with the name given
/// false otherwise
bool LocatedEntity::hasAttr(const std::string & name) const
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
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
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second->get(attr);
    }
    if (m_type != 0) {
        I = m_type->defaults().find(name);
        if (I != m_type->defaults().end()) {
            return I->second->get(attr);
        }
    }
    return false;
}

/// \brief Set the value of an attribute
///
/// @param name Name of attribute to be changed
/// @param attr Value to be stored
void LocatedEntity::setAttr(const std::string & name, const Element & attr)
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        I->second->set(attr);
        return;
    }
    m_properties[name] = new SoftProperty(attr);
    return;
}

/// \brief Get the property object for a given attribute
///
/// @param name name of the attribute for which the property is required.
/// @return a pointer to the property, or zero if the attributes does
/// not exist, or is not stored using a property object.
PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second;
    }
    return 0;
}

/// \brief Associate a script with this entity
///
/// The previously associated script is deleted.
/// @param scrpt Pointer to the script to be associated with this entity
void LocatedEntity::setScript(Script * scrpt)
{
    if (m_script != NULL && m_script != &noScript) {
        delete m_script;
    }
    m_script = scrpt;
}

/// \brief Change the container of an entity
///
/// @param new_loc The entity which is to become this entities new
/// container.
void LocatedEntity::changeContainer(LocatedEntity * new_loc)
{
    assert(m_location.m_loc->m_contains != 0);
    m_location.m_loc->m_contains->erase(this);
#if 0
    if (m_location.m_loc->m_contains->empty()) {
        m_location.m_loc->m_update_flags |= a_cont;
        m_location.m_loc->updated.emit();
    }
#endif
    bool was_empty = new_loc->m_contains->empty();
    new_loc->m_contains->insert(this);
#if 0
    if (was_empty) {
        new_loc->m_update_flags |= a_cont;
        new_loc->updated.emit();
    }
#endif
    assert(m_location.m_loc->checkRef() > 0);
    m_location.m_loc->decRef();
    m_location.m_loc = new_loc;
    m_location.m_loc->incRef();
    assert(m_location.m_loc->checkRef() > 0);
#if 0
    m_update_flags |= a_loc;

    containered.emit();

    std::list<sigc::connection>::iterator I = containered_oneshots.begin();
    std::list<sigc::connection>::iterator Iend = containered_oneshots.end();
    for (; I != Iend; ++I) {
        I->disconnect();
    }
#endif
}

/// \brief Read attributes from an Atlas element
///
/// @param ent The Atlas map element containing the attribute values
void LocatedEntity::merge(const MapType & ent)
{
    const std::set<std::string> & imm = immutables();
    MapType::const_iterator Iend = ent.end();
    for (MapType::const_iterator I = ent.begin(); I != Iend; ++I) {
        const std::string & key = I->first;
        if (imm.find(key) != imm.end()) continue;
        setAttr(key, I->second);
    }
}
