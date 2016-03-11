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

#include "TransformsProperty.h"
#include "DomainProperty.h"
#include "Domain.h"

#include "Script.h"
#include "AtlasProperties.h"

#include "common/Property.h"
#include "common/TypeNode.h"
#include "Domain.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

/// \brief Set of attribute names which must not be changed
///
/// The attributes named are special and are modified using high level
/// operations, such as Move, not via Set operations, or assigned by
/// normal means.
std::set<std::string> LocatedEntity::s_immutable = {"id", "parents", "pos", "loc", "velocity", "orientation", "contains", "objtype", "transforms"};

/// \brief Singleton accessor for immutables
///
/// The immutable attribute set m_immutables is returned.
const std::set<std::string> & LocatedEntity::immutables()
{
    return s_immutable;
}

/// \brief LocatedEntity constructor
LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
    m_properties["id"] = new IdProperty(getId());
    TransformsProperty* transProp = new TransformsProperty();
    m_properties["transforms"] = transProp;
}

LocatedEntity::~LocatedEntity()
{

    if (m_type) {
        for (auto entry : m_type->defaults()) {
            //Only remove if there's no instance specific property.
            if (m_properties.find(entry.first) == m_properties.end()) {
                entry.second->remove(this, entry.first);
            }
        }
    }

    for (auto entry : m_properties) {
        entry.second->remove(this, entry.first);
        delete entry.second;
    }
    delete m_script;
    if (m_location.m_loc != 0) {
        m_location.m_loc->decRef();
    }
    delete m_contains;
}

void LocatedEntity::setType(const TypeNode * t) {
    m_type = t;
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

void LocatedEntity::removeDelegate(int, const std::string &)
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
    return nullptr;
}

const Domain * LocatedEntity::getMovementDomain() const
{
    return nullptr;
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

std::vector<Atlas::Objects::Root> LocatedEntity::getThoughts() const
{
    return std::vector<Atlas::Objects::Root>();
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
    LocatedEntity* oldLoc = m_location.m_loc;
    oldLoc->removeChild(*this);
    new_loc->addChild(*this);
    new_loc->incRef();
    assert(m_location.m_loc->checkRef() > 0);

    onContainered(oldLoc);
    oldLoc->decRef();

}

void LocatedEntity::addChild(LocatedEntity& childEntity)
{
    makeContainer();
    bool was_empty = m_contains->empty();
    m_contains->insert(&childEntity);
    if (was_empty) {
        onUpdated();
    }

    childEntity.m_location.m_loc = this;

    if (m_flags & entity_domain) {
        auto domain = getPropertyClass<DomainProperty>("domain")->getDomain(this);
        domain->addEntity(childEntity);
    }
}

void LocatedEntity::removeChild(LocatedEntity& childEntity)
{
    assert(checkRef() > 0);
    assert(m_contains != 0);
    assert(m_contains->count(&childEntity));
    if (m_flags & entity_domain) {
        auto domain = getPropertyClass<DomainProperty>("domain")->getDomain(this);
        domain->removeEntity(childEntity);
    }
    m_contains->erase(&childEntity);
    if (m_contains->empty()) {
        onUpdated();
    }
}

bool LocatedEntity::isVisibleForOtherEntity(const LocatedEntity* watcher) const
{
    //Are we looking at ourselves?
    if (watcher == this) {
        return true;
    }

    //First find the domain which contains the watcher, as well as if the watcher has a domain itself.
    const LocatedEntity* domainEntity = watcher->m_location.m_loc;
    const LocatedEntity* topWatcherEntity = watcher;
    const Domain* watcherParentDomain = nullptr;

    while (domainEntity != nullptr) {
        watcherParentDomain = domainEntity->getMovementDomain();
        if (watcherParentDomain) {
            break;
        }
        topWatcherEntity = domainEntity;
        domainEntity = domainEntity->m_location.m_loc;
    }

    domainEntity = watcher->m_location.m_loc;
    const Domain* watcherOwnDomain = watcher->getMovementDomain();

    //Now walk upwards from the entity being looked at until we reach either the watcher's parent domain entity,
    //or the watcher itself
    std::vector<const LocatedEntity*> toAncestors;
    toAncestors.reserve(4);
    const LocatedEntity* ancestorEntity = this;
    const Domain* ancestorDomain = nullptr;


    while (true) {
        toAncestors.push_back(ancestorEntity);

        if (ancestorEntity == watcher) {
            ancestorDomain = watcherOwnDomain;
            break;
        }
        if (ancestorEntity == domainEntity) {
            ancestorDomain = watcherParentDomain;
            break;
        }
        if (ancestorEntity == topWatcherEntity) {
            break;
        }
        ancestorEntity = ancestorEntity->m_location.m_loc;
        if (ancestorEntity == nullptr) {
            //Could find no common ancestor; can't be seen.
            return false;
        }
    }

    //Now walk back down the toAncestors list, checking if all entities on the way can be seen.
    //Visibility is only checked for the first immediate child of a domain entity, further grandchildren are considered visible if the top one is, until
    //another domain is reached.
    for (auto I = toAncestors.rbegin(); I != toAncestors.rend(); ++I) {
        const LocatedEntity* ancestor = *I;
        if (ancestorDomain) {
            if (!ancestorDomain->isEntityVisibleFor(*watcher, *ancestor)) {
                return false;
            }
        }

        ancestorDomain = ancestor->getMovementDomain();
    }
    return true;
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

std::string LocatedEntity::describeEntity() const
{
    std::stringstream ss;
    Element nameAttr;
    int nameResult = getAttrType("name", nameAttr, Element::TYPE_STRING);
    ss << getId();
    if (m_type) {
        ss << "(" << m_type->name();
        if (nameResult == 0) {
            ss << ",'" << nameAttr.String() << "'";
        }
        ss << ")";
    } else {
        if (nameResult == 0) {
            ss << "('" << nameAttr.String() << "')";
        }
    }

    return ss.str();
}

