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


#include "LocatedEntity.h"

#include "Script.h"
#include "AtlasProperties.h"

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
        m_immutable.insert("objtype");
    }
    return m_immutable;
}

/// \brief LocatedEntity constructor
LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
    m_properties["id"] = new IdProperty(getId());
}

LocatedEntity::~LocatedEntity()
{
    PropertyDict::const_iterator I = m_properties.begin();
    PropertyDict::const_iterator Iend = m_properties.end();
    for (; I != Iend; ++I) {
        delete I->second;
    }
    delete m_script;
    if (m_location.m_loc != 0) {
        m_location.m_loc->decRef();
    }
    delete m_contains;
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
    if (m_type != 0) {
        I = m_type->defaults().find(name);
        if (I != m_type->defaults().end()) {
            return true;
        }
    }
    return false;
}

/// \brief Get the value of an attribute
///
/// @param name Name of attribute to be retrieved
/// @param attr Reference used to store value
/// @return zero if this entity has an attribute with the name given
/// nonzero otherwise
int LocatedEntity::getAttr(const std::string & name,
                           Element & attr) const
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
    return -1;
}

/// \brief Get the value of an attribute if it is the right type
///
/// @param name Name of attribute to be retrieved
/// @param attr Reference used to store value
/// @return zero if this entity has an attribute with the name given
/// nonzero otherwise
int LocatedEntity::getAttrType(const std::string & name,
                               Element & attr,
                               int type) const
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second->get(attr) || (attr.getType() == type ? 0 : 1);
    }
    if (m_type != 0) {
        I = m_type->defaults().find(name);
        if (I != m_type->defaults().end()) {
            return I->second->get(attr) || (attr.getType() == type ? 0 : 1);
        }
    }
    return -1;
}

/// \brief Set the value of an attribute
///
/// @param name Name of attribute to be changed
/// @param attr Value to be stored
PropertyBase * LocatedEntity::setAttr(const std::string & name,
                                      const Element & attr)
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        I->second->set(attr);
        return I->second;
    }
    return m_properties[name] = new SoftProperty(attr);
}

/// \brief Get the property object for a given attribute
///
/// @param name name of the attribute for which the property is required.
/// @return a pointer to the property, or zero if the attributes does
/// not exist, or is not stored using a property object.
const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second;
    }
    return 0;
}

PropertyBase * LocatedEntity::modProperty(const std::string & name)
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second;
    }
    return 0;
}

PropertyBase * LocatedEntity::setProperty(const std::string & name,
                                          PropertyBase * prop)
{
    return m_properties[name] = prop;
}

void LocatedEntity::installDelegate(int, const std::string &)
{
}

/// \brief Called when the container of this entity changes.
///
void LocatedEntity::onContainered(const LocatedEntity * new_loc)
{
}

/// \brief Called when the properties of this entity change.
///
void LocatedEntity::onUpdated()
{
}

/// \brief Called when the entity needs to be removed from its context
void LocatedEntity::destroy()
{
}

Domain * LocatedEntity::getMovementDomain()
{
    return 0;
}

/// \brief Send an operation to the world for dispatch.
///
/// sendWorld() bipasses serialno assignment, so you must ensure
/// that serialno is sorted. This allows client serialnos to get
/// in, so that client gets correct usefull refnos back.
void LocatedEntity::sendWorld(const Operation & op)
{
}

/// \brief Associate a script with this entity
///
/// The previously associated script is deleted.
/// @param scrpt Pointer to the script to be associated with this entity
void LocatedEntity::setScript(Script * scrpt)
{
    delete m_script;
    m_script = scrpt;
}

/// \brief Make this entity a container
///
/// If this entity is not already a contains, set up the necessary
/// storage and property.
void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
        m_properties["contains"] = new ContainsProperty(*m_contains);
    }
}

/// \brief Change the container of an entity
///
/// @param new_loc The entity which is to become this entities new
/// container.
void LocatedEntity::changeContainer(LocatedEntity * new_loc)
{
    assert(m_location.m_loc != 0);
    assert(m_location.m_loc->m_contains != 0);
    m_location.m_loc->m_contains->erase(this);
    if (m_location.m_loc->m_contains->empty()) {
        m_location.m_loc->onUpdated();
    }
    new_loc->makeContainer();
    bool was_empty = new_loc->m_contains->empty();
    new_loc->m_contains->insert(this);
    if (was_empty) {
        new_loc->onUpdated();
    }
    assert(m_location.m_loc->checkRef() > 0);
    LocatedEntity* oldLoc = m_location.m_loc;
    m_location.m_loc = new_loc;
    m_location.m_loc->incRef();
    assert(m_location.m_loc->checkRef() > 0);

    onContainered(oldLoc);
    oldLoc->decRef();
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
