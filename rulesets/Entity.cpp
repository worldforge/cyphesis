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


#include "Entity.h"

#include "Script.h"
#include "Domain.h"
#include "DomainProperty.h"
#include "TransformsProperty.h"
#include "Motion.h"

#include "common/BaseWorld.h"
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
#include "common/custom.h"

#include "common/Monitors.h"
#include "common/Variable.h"
#include "common/compose.hpp"

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

std::unordered_map<const TypeNode*, std::unique_ptr<int>> Entity::s_monitorsMap;

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
        LocatedEntity(id, intId), m_motion(nullptr)
{
}

Entity::~Entity()
{
    if (m_type) {
        auto I = s_monitorsMap.find(m_type);
        if (I != s_monitorsMap.end()) {
            int* ptr = I->second.get();
            *ptr = *ptr - 1;
        }
    }

    delete m_motion;
}

void Entity::setType(const TypeNode * t) {
    LocatedEntity::setType(t);

    if (t) {
        auto I = s_monitorsMap.find(t);
        if (I == s_monitorsMap.end()) {
            int* valuePtr = new int;
            *valuePtr = 1;
            s_monitorsMap.insert(std::make_pair(t, std::unique_ptr<int>(valuePtr)));

            Monitors::instance()->watch(String::compose("entity_count{type=\"%1\"}", t->name()), new Variable<int>(*valuePtr));
        } else {
            int* ptr = I->second.get();
            *ptr = *ptr + 1;
        }
    }
}


PropertyBase * Entity::setAttr(const std::string & name, const Element & attr)
{
    PropertyBase * prop;
    // If it is an existing property, just update the value.
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        prop = I->second;
        // Mark it as unclean
        prop->resetFlags(per_clean);
    } else {
        PropertyDict::const_iterator I;
        if (m_type != 0 &&
            (I = m_type->defaults().find(name)) != m_type->defaults().end()) {
            prop = I->second->copy();
        } else {
            // This is an entirely new property, not just a modification of
            // one in defaults, so we need to install it to this Entity.
            prop = PropertyManager::instance()->addProperty(name,
                                                            attr.getType());
            prop->install(this, name);
        }
        assert(prop != 0);
        m_properties[name] = prop;
    }

    prop->set(attr);
    // Allow the value to take effect.
    prop->apply(this);
    // Mark the Entity as unclean
    resetFlags(entity_clean);
    return prop;
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
            PropertyBase * new_prop = I->second->copy();
            I->second->remove(this, name);
            new_prop->flags() &= ~flag_class;
            m_properties[name] = new_prop;
            new_prop->apply(this);
            new_prop->install(this, name);
            return new_prop;
        }
    }
    return 0;
}

/// \brief Set the property object for a given attribute
///
/// @param name name of the attribute for which the property is given
/// @param prop the property object to be used
/// @returns a pointer to the property
PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return m_properties[name] = prop;
}

/// \brief Copy attributes into an Atlas element
///
/// @param omap Atlas map element this entity should be copied into
void Entity::addToMessage(MapType & omap) const
{
    // We need to have a list of keys to pull from attributes.
    PropertyDict::const_iterator J;
    PropertyDict::const_iterator Jend;

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

    J = m_properties.begin();
    Jend = m_properties.end();
    for (; J != Jend; ++J) {
        J->second->add(J->first, ent);
    }

    ent->setStamp(m_seq);
    if (m_type != 0) {
        ent->setParents(std::list<std::string>(1, m_type->name()));
    }
    m_location.addToEntity(ent);
    ent->setObjtype("obj");
}

/// \brief Install a delegate property for an operation
///
/// @param class_no The class number of the operation to be handled
/// @param delegate The name of the property to delegate it to.
void Entity::installDelegate(int class_no, const std::string & delegate)
{
    m_delegates.insert(std::make_pair(class_no, delegate));
}

void Entity::removeDelegate(int class_no, const std::string & delegate)
{
    auto I = m_delegates.find(class_no);
    if (I != m_delegates.end() && I->second == delegate) {
        m_delegates.erase(I);
    }
}

/// \brief Destroy this entity
///
/// Do the jobs required to remove this entity from the world. Handles
/// removing from the containership tree.
void Entity::destroy()
{
    assert(m_location.m_loc != 0);
    assert(m_location.m_loc->m_contains != 0);
    if (m_contains != 0) {
        LocatedEntitySet::const_iterator Iend = m_contains->end();
        for (LocatedEntitySet::const_iterator I = m_contains->begin(); I != Iend; ++I) {
            Location & child = (*I)->m_location;
            // FIXME take account of orientation
            // FIXME velocity and orientation  need to be adjusted
            TransformsProperty* transformsProp = (*I)->requirePropertyClassFixed<TransformsProperty>();

            if (m_location.orientation().isValid() && m_location.pos().isValid()) {

                transformsProp->getTranslate() = WFMath::Vector<3>(child.m_pos.toParentCoords(m_location.pos(),
                                                         m_location.orientation()));
                if (transformsProp->getRotate().isValid()) {
                    transformsProp->getRotate() *= m_location.orientation();
                }

                if (child.m_velocity.isValid()) {
                    child.m_velocity.rotate(m_location.orientation());
                }
            } else {
                static const Quaternion identity(1, 0, 0, 0);
                transformsProp->getTranslate() = WFMath::Vector<3>(child.m_pos.toParentCoords(m_location.pos(),
                                                         identity));
            }
            transformsProp->apply(*I);
            // Remove the reference to ourself.
            decRef();
            m_location.m_loc->addChild(**I);
            m_location.m_loc->incRef();
        }
    }

    delete m_script;
    m_script = 0;

    // We don't call decRef() on our parent, because we may not get deleted
    // yet, and we need to keep a reference to our parent in case there
    // are broadcast ops left that we have not yet sent.
    // It will be decRef()ed automatically from our (LocatedEntity)
    // destructor
    m_location.m_loc->removeChild(*this);
    m_flags |= entity_destroyed;
    destroyed.emit();
}

Domain * Entity::getMovementDomain()
{
    if (m_flags & entity_domain) {
        return getPropertyClass<DomainProperty>("domain")->getDomain(this);
    } else {
        return nullptr;
    }
}

const Domain * Entity::getMovementDomain() const
{
    if (m_flags & entity_domain) {
        return getPropertyClass<DomainProperty>("domain")->getDomain(this);
    } else {
        return nullptr;
    }
    return nullptr;

}


void Entity::sendWorld(const Operation & op)
{
    BaseWorld::instance().message(op, *this);
}

/// \brief Handle a actuate operation
void Entity::ActuateOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a appearance operation
void Entity::AppearanceOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a attack operation
void Entity::AttackOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a combine operation
void Entity::CombineOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a create operation
void Entity::CreateOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a delete operation
void Entity::DeleteOperation(const Operation &, OpVector &)
{
    //We call on the baseworld to delete ourselves here. This allows
    //other components, such as properties, to preempt the deletion if
    //they so want.
    BaseWorld::instance().delEntity(this);
}

/// \brief Handle a disappearance operation
void Entity::DisappearanceOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a divide operation
void Entity::DivideOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a eat operation
void Entity::EatOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a get operation
void Entity::GetOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a imaginary operation
void Entity::ImaginaryOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a info operation
void Entity::InfoOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a look operation
void Entity::LookOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a move operation
void Entity::MoveOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a nourish operation
void Entity::NourishOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a set operation
void Entity::SetOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a sight operation
void Entity::SightOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a sound operation
void Entity::SoundOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a talk operation
void Entity::TalkOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a tick operation
void Entity::TickOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a touch operation
void Entity::TouchOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a update operation
void Entity::UpdateOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a use operation
void Entity::UseOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a wield operation
void Entity::WieldOperation(const Operation &, OpVector &)
{
}

/// \brief Handle a relay operation
void Entity::RelayOperation(const Operation & op, OpVector & res)
{
    if (op->getArgs().empty()) {
        log(ERROR, "Entity::RelayOperation no args.");
        return;
    }
    Operation relayedOp = Atlas::Objects::smart_dynamic_cast<Operation>(
            op->getArgs().front());

    if (!relayedOp.isValid()) {
        log(ERROR,
                "Entity::RelayOperation first arg is not an operation.");
        return;
    }

    if (op->isDefaultSerialno()) {
        log(ERROR, "Entity::RelayOperation no serial number.");
        return;
    }

    //Add a sight of the operation
    Sight sight;
    sight->setArgs1(relayedOp);

    Atlas::Objects::Operation::Generic responseOp;
    responseOp->setType("relay", Atlas::Objects::Operation::RELAY_NO);
    responseOp->setArgs1(sight);
    responseOp->setTo(op->getFrom());
    res.push_back(responseOp);

    //Make sure that the contained op is addressed to the entity
    relayedOp->setTo(getId());
    operation(relayedOp, res);

}

void Entity::externalOperation(const Operation & op, Link &)
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
    if (m_script != 0 &&
        m_script->operation(op->getParents().front(), op, res) != 0) {
        return;
    }

    auto J = m_delegates.equal_range(op->getClassNo());
    HandlerResult hr = OPERATION_IGNORED;
    for (;J.first != J.second; ++J.first) {
        HandlerResult hr_call = callDelegate(J.first->second, op, res);
        //We'll record the most blocking of the different results only.
        if (hr != OPERATION_BLOCKED) {
            if (hr_call != OPERATION_IGNORED) {
                hr = hr_call;
            }
        }
        // How to access the property? We need a non-const pointer to call
        // operation, but to get this easily we need to force instantiation
        // from the type dict, making properties way less efficient.
        // Making the operation() method const strongly limits the usefulness
        // of delegates, but if we fetch the pointer the hard way, we then
        // require the method to handle instantiation on demand.
        //
        // Can we make a clean way to handle the property in the general case
        // handle instantiation itself? Making it responsible for copying
        // itself on instantiation would be faster than the
        // get/set/PropertyManager currently required in in modProperty.
    }
    //If the operation was blocked we shouldn't send it on to the entity.
    if (hr == OPERATION_BLOCKED) {
        return;
    }
    return callOperation(op, res);
}

HandlerResult Entity::callDelegate(const std::string & name,
                                   const Operation & op,
                                   OpVector & res)
{
    PropertyBase * p = 0;
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        p = I->second;
    } else if (m_type != 0) {
        I = m_type->defaults().find(name); 
        if (I != m_type->defaults().end()) {
            p = I->second;
        }
    }
    if (p != 0) {
        return p->operation(this, op, res);
    }
    return OPERATION_IGNORED;
}

/// \brief Find and call the handler for an operation
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void Entity::callOperation(const Operation & op, OpVector & res)
{
    auto op_no = op->getClassNo();
    OP_SWITCH(op, op_no, res,)
}

void Entity::onContainered(const LocatedEntity* oldLocation)
{
    containered.emit(oldLocation);
}

void Entity::onUpdated()
{
    updated.emit();
}
