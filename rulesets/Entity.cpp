// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
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

// $Id: Entity.cpp,v 1.129 2007-01-05 17:19:26 alriddoch Exp $

#include "Entity.h"

#include "Script.h"
#include "AtlasProperties.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/inheritance.h"
#include "common/PropertyManager.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/functors/mem_fun.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

/// \brief Set of attribute names which must not be changed
///
/// The attributes named are special and are modified using high level
/// operations, such as Move, not via Set operations, or assigned by
/// normal means.
std::set<std::string> Entity::m_immutable;

/// \brief Singleton accessor for immutables
///
/// The immutable attribute set m_immutables is initialised if necessary,
/// and a reference is returned.
const std::set<std::string> & Entity::immutables()
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

/// \brief Entity constructor
Entity::Entity(const std::string & id, long intId) : BaseEntity(id, intId),
                                         m_refCount(0), m_destroyed(false),
                                         m_script(&noScript), m_seq(0),
                                         m_status(1), m_type("entity"),
                                         m_mass(-1), m_perceptive(false),
                                         m_update_flags(0)
{
    m_properties["status"] = new Property<double>(m_status, a_status);
    m_properties["id"] = new IdProperty(getId());
    m_properties["name"] = new NameProperty(m_name, a_name);
    m_properties["mass"] = new Property<double>(m_mass, a_mass);
    m_properties["contains"] = new ContainsProperty(m_contains);

    SignalProperty<BBox> * sp = new SignalProperty<BBox>(m_location.m_bBox, a_bbox);
    sp->modified.connect(sigc::mem_fun(&m_location, &Location::modifyBBox));
    m_properties["bbox"] = sp;
}

Entity::~Entity()
{
    if (m_script != NULL && m_script != &noScript) {
        delete m_script;
    }
    PropertyDict::const_iterator I = m_properties.begin();
    PropertyDict::const_iterator Iend = m_properties.end();
    for (; I != Iend; ++I) {
        delete I->second;
    }
    if (m_location.m_loc != 0) {
        m_location.m_loc->decRef();
    }
}

/// \brief Check if this entity has a property with the given name
///
/// @param name Name of attribute to be checked
/// @return trye if this entity has an attribute with the name given
/// false otherwise
bool Entity::hasAttr(const std::string & name) const
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return true;
    }
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
bool Entity::getAttr(const std::string & name, Element & attr) const
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second->get(attr);
    }
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
void Entity::setAttr(const std::string & name, const Element & attr)
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        I->second->set(attr);
        m_update_flags |= I->second->flags();
        return;
    }
    MapType::iterator J = m_attributes.find(name);
    if (J == m_attributes.end()) {
        PropertyBase * prop = PropertyManager::instance()->addProperty(this,
                                                                       name);
        if (prop != 0) {
            m_properties[name] = prop;
            m_update_flags |= prop->flags();
        } else {
            m_attributes[name] = attr;
            m_update_flags |= a_attr;
        }
        return;
    }
    J->second = attr;
    // m_attributes[name] = attr;
    m_update_flags |= a_attr;
}

/// \brief Get the property object for a given attribute
///
/// @param name name of the attribute for which the property is required.
/// @return a pointer to the property, or zero if the attributes does
/// not exist, or is not stored using a property object.
PropertyBase * Entity::getProperty(const std::string & name) const
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second;
    }
    return 0;
}

/// \brief Copy attributes into an Atlas element
///
/// @param omap Atlas map element this entity should be copied into
void Entity::addToMessage(MapType & omap) const
{
    // We need to have a list of keys to pull from attributes.
    MapType::const_iterator Iend = m_attributes.end();
    for (MapType::const_iterator I = m_attributes.begin(); I != Iend; ++I) {
        omap[I->first] = I->second;
    }
    PropertyDict::const_iterator J = m_properties.begin();
    PropertyDict::const_iterator Jend = m_properties.end();
    for (; J != Jend; ++J) {
        J->second->add(J->first, omap);
    }
    omap["stamp"] = (double)m_seq;
    omap["parents"] = ListType(1, m_type);
    m_location.addToMessage(omap);
    omap["objtype"] = "obj";
}

/// \brief Copy attributes into an Atlas entity
///
/// @param ent Atlas entity this entity should be copied into
void Entity::addToEntity(const RootEntity & ent) const
{
    // We need to have a list of keys to pull from attributes.
    MapType::const_iterator Iend = m_attributes.end();
    for (MapType::const_iterator I = m_attributes.begin(); I != Iend; ++I) {
        ent->setAttr(I->first, I->second);
    }
    PropertyDict::const_iterator J = m_properties.begin();
    PropertyDict::const_iterator Jend = m_properties.end();
    for (; J != Jend; ++J) {
        J->second->add(J->first, ent);
    }
    ent->setStamp(m_seq);
    ent->setParents(std::list<std::string>(1, m_type));
    m_location.addToEntity(ent);
    ent->setObjtype("obj");
}

/// \brief Install a handler function for an operation
///
/// @param class_no The class number of the operation to be handled
/// @param handler A pointer to the function to be wrapped
void Entity::installHandler(int class_no, Handler handler)
{
    m_operationHandlers.insert(std::make_pair(class_no, handler));
}

/// \brief Associate a script with this entity
///
/// The previously associated script is deleted.
/// @param scrpt Pointer to the script to be associated with this entity
void Entity::setScript(Script * scrpt)
{
    if (m_script != NULL && m_script != &noScript) {
        delete m_script;
    }
    m_script = scrpt;
}

/// \brief Destroy this entity
///
/// Do the jobs required to remove this entity from the world. Handles
/// removing from the containership tree.
void Entity::destroy()
{
    assert(m_location.m_loc != NULL);
    EntitySet & loc_contains = m_location.m_loc->m_contains;
    EntitySet::const_iterator Iend = m_contains.end();
    for (EntitySet::const_iterator I = m_contains.begin(); I != Iend; ++I) {
        Entity * obj = *I;
        // FIXME take account of orientation
        // FIXME velocity and orientation  need to be adjusted
        // Remove the reference to ourself.
        decRef();
        obj->m_location.m_loc = m_location.m_loc;
        m_location.m_loc->incRef();
        if (m_location.orientation().isValid()) {
            obj->m_location.m_pos = obj->m_location.m_pos.toParentCoords(m_location.pos(), m_location.orientation());
            obj->m_location.m_velocity.rotate(m_location.orientation());
            obj->m_location.m_orientation *= m_location.orientation();
        } else {
            static const Quaternion identity(1, 0, 0, 0);
            obj->m_location.m_pos = obj->m_location.m_pos.toParentCoords(m_location.pos(), identity);
        }
        loc_contains.insert(obj);
    }
    loc_contains.erase(this);

    // We don't call decRef() on our parent, because we may not get deleted
    // yet, and we need to keep a reference to our parent in case there
    // are broadcast ops left that we have not yet sent.

    if (m_location.m_loc->m_contains.empty()) {
        m_location.m_loc->m_update_flags |= a_cont;
        m_location.m_loc->updated.emit();
    }
    m_destroyed = true;
    destroyed.emit();
}

/// \brief Read attributes from an Atlas element
///
/// @param ent The Atlas map element containing the attribute values
void Entity::merge(const MapType & ent)
{
    const std::set<std::string> & imm = immutables();
    MapType::const_iterator Iend = ent.end();
    for (MapType::const_iterator I = ent.begin(); I != Iend; ++I) {
        const std::string & key = I->first;
        if (imm.find(key) != imm.end()) continue;
        setAttr(key, I->second);
    }
}

/// \brief Change the container of an entity
///
/// @param new_loc The entity which is to become this entities new
/// container.
void Entity::changeContainer(Entity * new_loc)
{
    m_location.m_loc->m_contains.erase(this);
    if (m_location.m_loc->m_contains.empty()) {
        m_location.m_loc->m_update_flags |= a_cont;
        m_location.m_loc->updated.emit();
    }
    bool was_empty = new_loc->m_contains.empty();
    new_loc->m_contains.insert(this);
    if (was_empty) {
        new_loc->m_update_flags |= a_cont;
        new_loc->updated.emit();
    }
    assert(m_location.m_loc->checkRef() > 0);
    m_location.m_loc->decRef();
    m_location.m_loc = new_loc;
    m_location.m_loc->incRef();
    assert(m_location.m_loc->checkRef() > 0);
    m_update_flags |= a_loc;

    containered.emit();

    std::list<sigc::connection>::iterator I = containered_oneshots.begin();
    std::list<sigc::connection>::iterator Iend = containered_oneshots.end();
    for (; I != Iend; ++I) {
        I->disconnect();
    }
}

/// \brief Subscribe this entity to operations of the type given
///
/// @param op Type of operation this entity should be subscribed to
void Entity::scriptSubscribe(const std::string & op)
{
    // FIXME Need to keep track efficiently of what script has, preferably
    // with something like a std::set of class numbers for quick efficient
    // checking
#if 0
    OpNo n = Inheritance::instance().opEnumerate(op);
    if (n != OP_INVALID) {
        debug(std::cout << "SCRIPT requesting subscription to " << op
                        << std::endl << std::flush;);
        subscribe(op, n);
    } else {
        std::string msg = std::string("SCRIPT requesting subscription to ")
                        + op + " but inheritance could not give me a reference";
        log(ERROR, msg.c_str());
    }
#endif
}

/// \brief Process an operation from an external source.
///
/// The ownership of the operation passed in at this point is handed
/// over to the entity. The calling code must not modify the operation
/// after passing it to externalOperation, or expect the attributes
/// of the operaration to remain the same.
/// @param op The operation to be processed.
void Entity::externalOperation(const Operation & op)
{
    OpVector res;
    operation(op, res);
    OpVector::const_iterator Iend = res.end();
    for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
        if (!op->isDefaultSerialno()) {
            (*I)->setRefno(op->getSerialno());
        }
        sendWorld(*I);
    }
}

void Entity::operation(const Operation & op, OpVector & res)
{
    if (m_script->operation(op->getParents().front(), op, res) != 0) {
        return;
    }
    HandlerMap::const_iterator I = m_operationHandlers.find(op->getClassNo());
    if (I != m_operationHandlers.end()) {
        std::cout << "Found handler for MOVE operations" << std::endl << std::flush;
        I->second(op, res);
    }
    return callOperation(op, res);
}
