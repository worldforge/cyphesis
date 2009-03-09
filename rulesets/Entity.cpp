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

// $Id$

#include "Entity.h"

#include "Script.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/op_switch.h"
#include "common/TypeNode.h"
#include "common/Property.h"
#include "common/PropertyManager.h"

#include "common/Actuate.h"
#include "common/Attack.h"
#include "common/Eat.h"
#include "common/Nourish.h"
#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Update.h"

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

/// \brief Flags used to control entities
///
/// These flags are used to indicate various aspects of entities.
/// \defgroup EntityFlags In World Entity Flags

/// \brief Classes that model in world entities
///
/// These classes are used to model all in world entities or objects.
/// \defgroup EntityClasses In World Entity Classes

/// \brief Entity constructor
Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId), m_motion(0), m_flags(0)
{
}

Entity::~Entity()
{
}

void Entity::setAttr(const std::string & name, const Element & attr)
{
    PropertyBase * prop;
    // If it is an existing property, just update the value.
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        prop = I->second;
        prop->set(attr);
        // Allow the change to take effect.
        prop->apply(this);
        // Mark it as unclean and the Entity as unclean
        prop->resetFlags(per_clean);
        resetFlags(entity_clean);
        return;
    }
    prop = PropertyManager::instance()->addProperty(name, attr.getType());
    assert(prop != 0);
    prop->set(attr);
    m_properties[name] = prop;
    // If this is an entirely new property, not just a modifcation of
    // one in defaults, then we need to install it to this Entity.
    if (m_type == 0 || m_type->defaults().find(name) == m_type->defaults().end()) {
        prop->install(this);
    }
    // Allow the value to take effect.
    prop->apply(this);
    resetFlags(entity_clean);
    return;
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second;
    }
    if (m_type != 0) {
        I = m_type->defaults().find(name);
        if (I != m_type->defaults().end()) {
            return I->second;
        }
    }
    return 0;
}

PropertyBase * Entity::modProperty(const std::string & name)
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second;
    }
    if (m_type != 0) {
        I = m_type->defaults().find(name);
        if (I != m_type->defaults().end()) {
            // We have a default for this property. Create a new instance
            // property with the same value.
            Element val;
            I->second->get(val);
            PropertyBase * new_prop = PropertyManager::instance()->addProperty(name, val.getType());
            new_prop->set(val);
            m_properties[name] = new_prop;
            new_prop->apply(this);
            return new_prop;
        }
    }
    return 0;
}

/// \brief Set the property object for a given attribute
///
/// @param name name of the attribute for which the property is given
/// @param prop the property object to be used
void Entity::setProperty(const std::string & name, PropertyBase * prop)
{
    m_properties[name] = prop;
}

/// \brief Copy attributes into an Atlas element
///
/// @param omap Atlas map element this entity should be copied into
void Entity::addToMessage(MapType & omap) const
{
    // We need to have a list of keys to pull from attributes.
    PropertyDict::const_iterator J;
    PropertyDict::const_iterator Jend;

    if (m_type != 0) {
        J = m_type->defaults().begin();
        Jend = m_type->defaults().end();
        for (; J != Jend; ++J) {
            J->second->add(J->first, omap);
        }
    }

    J = m_properties.begin();
    Jend = m_properties.end();
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
    PropertyDict::const_iterator J;
    PropertyDict::const_iterator Jend;

    if (m_type != 0) {
        J = m_type->defaults().begin();
        Jend = m_type->defaults().end();
        for (; J != Jend; ++J) {
            J->second->add(J->first, ent);
        }
    }

    J = m_properties.begin();
    Jend = m_properties.end();
    for (; J != Jend; ++J) {
        J->second->add(J->first, ent);
    }

    ent->setStamp(m_seq);
    ent->setParents(std::list<std::string>(1, m_type->name()));
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

/// \brief Destroy this entity
///
/// Do the jobs required to remove this entity from the world. Handles
/// removing from the containership tree.
void Entity::destroy()
{
    assert(m_location.m_loc != 0);
    assert(m_location.m_loc->m_contains != 0);
    LocatedEntitySet & loc_contains = *m_location.m_loc->m_contains;
    if (m_contains != 0) {
        LocatedEntitySet::const_iterator Iend = m_contains->end();
        for (LocatedEntitySet::const_iterator I = m_contains->begin(); I != Iend; ++I) {
            Location & child = (*I)->m_location;
            // FIXME take account of orientation
            // FIXME velocity and orientation  need to be adjusted
            // Remove the reference to ourself.
            decRef();
            child.m_loc = m_location.m_loc;
            m_location.m_loc->incRef();
            if (m_location.orientation().isValid()) {
                child.m_pos = child.m_pos.toParentCoords(m_location.pos(),
                                                         m_location.orientation());
                child.m_velocity.rotate(m_location.orientation());
                child.m_orientation *= m_location.orientation();
            } else {
                static const Quaternion identity(1, 0, 0, 0);
                child.m_pos = child.m_pos.toParentCoords(m_location.pos(),
                                                         identity);
            }
            loc_contains.insert(*I);
        }
    }
    loc_contains.erase(this);

    // We don't call decRef() on our parent, because we may not get deleted
    // yet, and we need to keep a reference to our parent in case there
    // are broadcast ops left that we have not yet sent.

    if (loc_contains.empty()) {
        Entity * loc = dynamic_cast<Entity *>(m_location.m_loc);
        // FIXME Do we need to call onUpdated() on the parent entity?
        loc->onUpdated();
    }
    m_flags |= entity_destroyed;
    destroyed.emit();
}

void Entity::ActuateOperation(const Operation &, OpVector &)
{
}

void Entity::AppearanceOperation(const Operation &, OpVector &)
{
}

void Entity::AttackOperation(const Operation &, OpVector &)
{
}

void Entity::CombineOperation(const Operation &, OpVector &)
{
}

void Entity::CreateOperation(const Operation &, OpVector &)
{
}

void Entity::DeleteOperation(const Operation &, OpVector &)
{
}

void Entity::DisappearanceOperation(const Operation &, OpVector &)
{
}

void Entity::DivideOperation(const Operation &, OpVector &)
{
}

void Entity::EatOperation(const Operation &, OpVector &)
{
}

void Entity::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Entity::LookOperation(const Operation &, OpVector &)
{
}

void Entity::MoveOperation(const Operation &, OpVector &)
{
}

void Entity::NourishOperation(const Operation &, OpVector &)
{
}

void Entity::SetOperation(const Operation &, OpVector &)
{
}

void Entity::SightOperation(const Operation &, OpVector &)
{
}

void Entity::SoundOperation(const Operation &, OpVector &)
{
}

void Entity::TalkOperation(const Operation &, OpVector &)
{
}

void Entity::TickOperation(const Operation &, OpVector &)
{
}

void Entity::TouchOperation(const Operation &, OpVector &)
{
}

void Entity::UpdateOperation(const Operation &, OpVector &)
{
}

void Entity::WieldOperation(const Operation &, OpVector &)
{
}

/// \brief Process an operation from an external source, from this Entity
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
        debug(std::cout << "Found handler for " << op->getParents().front()
                        << " operations" << std::endl << std::flush;);
        I->second(this, op, res);
    }
    return callOperation(op, res);
}

/// \brief Find and call the handler for an operation
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void Entity::callOperation(const Operation & op, OpVector & res)
{
    const OpNo op_no = op->getClassNo();
    OP_SWITCH(op, op_no, res,)
}

void Entity::onContainered()
{
    containered.emit();

    containered.clear();
}

void Entity::onUpdated()
{
    updated.emit();
}
