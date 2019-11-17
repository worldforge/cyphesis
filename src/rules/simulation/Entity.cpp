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

#include "rules/Script.h"
#include "rules/Domain.h"
#include "DomainProperty.h"

#include "BaseWorld.h"
#include "common/debug.h"
#include "common/op_switch.h"
#include "common/TypeNode.h"
#include "common/Link.h"
#include "common/PropertyManager.h"

#include "common/custom.h"
#include "common/operations/Relay.h"

#include "common/Monitors.h"
#include "common/Variable.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

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
Entity::Entity(const std::string& id, long intId) :
    LocatedEntity(id, intId)
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
}

void Entity::setType(const TypeNode* t)
{
    LocatedEntity::setType(t);

    if (t) {
        auto I = s_monitorsMap.find(t);
        if (I == s_monitorsMap.end()) {
            int* valuePtr = new int;
            *valuePtr = 1;
            s_monitorsMap.insert(std::make_pair(t, std::unique_ptr<int>(valuePtr)));

            Monitors::instance().watch(String::compose("entity_count{type=\"%1\"}", t->name()), new Variable<int>(*valuePtr));
        } else {
            int* ptr = I->second.get();
            *ptr = *ptr + 1;
        }
    }
}


void Entity::addChild(LocatedEntity& childEntity)
{
    LocatedEntity::addChild(childEntity);
    auto domain = getDomain();
    if (domain) {
        domain->addEntity(childEntity);
    }
}

void Entity::removeChild(LocatedEntity& childEntity)
{
    if (m_domain) {
        m_domain->removeEntity(childEntity);
    }
    LocatedEntity::removeChild(childEntity);
}


PropertyBase* Entity::setAttr(const std::string& name, const Element& attr)
{
    PropertyBase* prop;
    // If it is an existing property, just update the value.
    auto I = m_properties.find(name);
    if (I != m_properties.end()) {
        prop = I->second.get();
        // Mark it as unclean
        prop->removeFlags(persistence_clean);
    } else {
        PropertyDict::const_iterator J;
        if (m_type != nullptr && (J = m_type->defaults().find(name)) != m_type->defaults().end()) {
            prop = J->second->copy();
            m_properties[name].reset(prop);
        } else {
            // This is an entirely new property, not just a modification of
            // one in defaults, so we need to install it to this Entity.
            auto newProp = PropertyManager::instance().addProperty(name, attr.getType());
            prop = newProp.get();
            m_properties[name] = std::move(newProp);
            prop->install(this, name);
        }
        assert(prop != nullptr);
    }

    prop->set(attr);
    // Allow the value to take effect.
    applyProperty(name, prop);
    return prop;
}

const PropertyBase* Entity::getProperty(const std::string& name) const
{
    auto I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second.get();
    }
    if (m_type != nullptr) {
        I = m_type->defaults().find(name);
        if (I != m_type->defaults().end()) {
            return I->second.get();
        }
    }
    return nullptr;
}

PropertyBase* Entity::modProperty(const std::string& name, const Atlas::Message::Element& def_val)
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second.get();
    }
    if (m_type != nullptr) {
        I = m_type->defaults().find(name);
        if (I != m_type->defaults().end()) {
            // We have a default for this property. Create a new instance
            // property with the same value.
            PropertyBase* new_prop = I->second->copy();
            if (!def_val.isNone()) {
                new_prop->set(def_val);
            }
            I->second->remove(this, name);
            new_prop->removeFlags(flag_class);
            m_properties[name].reset(new_prop);
            new_prop->install(this, name);
            new_prop->apply(this);
            propertyApplied(name, *new_prop);
            return new_prop;
        }
    }
    return nullptr;
}

/// \brief Set the property object for a given attribute
///
/// @param name name of the attribute for which the property is given
/// @param prop the property object to be used
/// @returns a pointer to the property
PropertyBase* Entity::setProperty(const std::string& name,
                                  std::unique_ptr<PropertyBase> prop)
{
    auto p = prop.get();
    m_properties[name] = std::move(prop);
    return p;
}

/// \brief Copy attributes into an Atlas element
///
/// @param omap Atlas map element this entity should be copied into
void Entity::addToMessage(MapType& omap) const
{
    // We need to have a list of keys to pull from attributes.
    PropertyDict::const_iterator J;
    PropertyDict::const_iterator Jend;

    J = m_properties.begin();
    Jend = m_properties.end();
    for (; J != Jend; ++J) {
        J->second->add(J->first, omap);
    }

    omap["stamp"] = (double) m_seq;
    omap["parent"] = m_type;
    m_location.addToMessage(omap);
    omap["objtype"] = "obj";
}

/// \brief Copy attributes into an Atlas entity
///
/// @param ent Atlas entity this entity should be copied into
void Entity::addToEntity(const RootEntity& ent) const
{
    // We need to have a list of keys to pull from attributes.
    for (auto& entry : m_properties) {
        entry.second->add(entry.first, ent);
    }

    ent->setStamp(m_seq);
    if (m_type != nullptr) {
        ent->setParent(m_type->name());
    }
    m_location.addToEntity(ent);
    ent->setObjtype("obj");
}

/// \brief Install a delegate property for an operation
///
/// @param class_no The class number of the operation to be handled
/// @param delegate The name of the property to delegate it to.
void Entity::installDelegate(int class_no, const std::string& delegate)
{
    m_delegates.insert(std::make_pair(class_no, delegate));
}

void Entity::removeDelegate(int class_no, const std::string& delegate)
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
    assert(m_location.m_parent);
    assert(m_location.m_parent->m_contains);
    if (m_contains != nullptr) {
        for (auto& entity : *m_contains) {
            Location& child = entity->m_location;
            // FIXME take account of orientation
            // FIXME velocity and orientation  need to be adjusted

            if (m_location.orientation().isValid() && m_location.pos().isValid()) {

                m_location.m_pos = child.m_pos.toParentCoords(m_location.pos(), m_location.orientation());
                if (m_location.m_orientation.isValid()) {
                    m_location.m_orientation *= m_location.orientation();
                }

                if (child.m_velocity.isValid()) {
                    child.m_velocity.rotate(m_location.orientation());
                }
            } else {
                m_location.m_pos = child.m_pos.toParentCoords(m_location.pos(), Quaternion::IDENTITY());
            }
            m_location.m_parent->addChild(*entity);
        }
    }

    LocatedEntity::destroy();

}

Domain* Entity::getDomain()
{
    return m_domain.get();
}

const Domain* Entity::getDomain() const
{
    return m_domain.get();
}

void Entity::setDomain(std::unique_ptr<Domain> domain)
{
    m_domain = std::move(domain);
    if (m_domain) {
        addFlags(entity_domain);
    } else {
        removeFlags(entity_domain);
    }
}

void Entity::sendWorld(const Operation& op)
{
    BaseWorld::instance().message(op, *this);
}

/// \brief Handle a delete operation
void Entity::DeleteOperation(const Operation&, OpVector&)
{
    //We call on the baseworld to delete ourselves here. This allows
    //other components, such as properties, to preempt the deletion if
    //they so want.
    BaseWorld::instance().delEntity(this);
}

/// \brief Handle a imaginary operation
void Entity::ImaginaryOperation(const Operation&, OpVector&)
{
}

/// \brief Handle a look operation
void Entity::LookOperation(const Operation&, OpVector&)
{
}

/// \brief Handle a move operation
void Entity::MoveOperation(const Operation&, OpVector&)
{
}

/// \brief Handle a set operation
void Entity::SetOperation(const Operation&, OpVector&)
{
}

/// \brief Handle a talk operation
void Entity::TalkOperation(const Operation&, OpVector&)
{
}

/// \brief Handle a update operation
void Entity::UpdateOperation(const Operation&, OpVector&)
{
}

/// \brief Handle a relay operation
void Entity::RelayOperation(const Operation& op, OpVector& res)
{
}

void Entity::CreateOperation(const Operation& op, OpVector& res)
{
}

void Entity::addListener(OperationsListener* listener)
{
    if (std::find(m_listeners.begin(), m_listeners.end(), listener) == m_listeners.end()) {
        m_listeners.push_back(listener);
    }
}

void Entity::removeListener(OperationsListener* listener)
{
    auto I = std::find(m_listeners.begin(), m_listeners.end(), listener);
    if (I != m_listeners.end()) {
        m_listeners.erase(I);
    }
}

void Entity::externalOperation(const Operation& op, Link& link)
{
    if (op->getClassNo() != Atlas::Objects::Operation::THOUGHT_NO) {
        OpVector res;
        clientError(op, "An entity can only be externally controlled by Thoughts.", res, getId());
        for (auto& resOp : res) {
            link.send(resOp);
        }
    }

    OpVector res;
    operation(op, res);
    for (auto& resOp : res) {
        sendWorld(resOp);
    }
}

void Entity::operation(const Operation& op, OpVector& res)
{
    HandlerResult hr = OPERATION_IGNORED;

    if (!m_scripts.empty()) {
        for (auto& script: m_scripts) {
            auto hr_call = script->operation(op->getParent(), op, res);
            //Stop on the first blocker. Only change "hr" value if it's "handled".
            if (hr_call != OPERATION_IGNORED) {
                if (hr_call == OPERATION_BLOCKED) {
                    return;
                }
                hr = hr_call;
            }
        }
    }

    auto J = m_delegates.equal_range(op->getClassNo());
    for (; J.first != J.second; ++J.first) {
        HandlerResult hr_call = callDelegate(J.first->second, op, res);
        //We'll record the most blocking of the different results only.
        if (hr != OPERATION_BLOCKED) {
            if (hr_call != OPERATION_IGNORED) {
                hr = hr_call;
            }
        }
    }

    //If the operation was blocked we shouldn't send it on to the entity.
    if (hr == OPERATION_BLOCKED) {
        return;
    }

    //TODO: decide on whether we should honour result here?
    for (auto& listener : m_listeners) {
        listener->operation(this, op, res);
    }
    return callOperation(op, res);
}

HandlerResult Entity::callDelegate(const std::string& name,
                                   const Operation& op,
                                   OpVector& res)
{
    PropertyBase* p = nullptr;
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        p = I->second.get();
    } else if (m_type != nullptr) {
        I = m_type->defaults().find(name);
        if (I != m_type->defaults().end()) {
            p = I->second.get();
        }
    }
    if (p != nullptr) {
        return p->operation(this, op, res);
    }
    return OPERATION_IGNORED;
}

/// \brief Find and call the handler for an operation
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void Entity::callOperation(const Operation& op, OpVector& res)
{
    auto op_no = op->getClassNo();
    OP_SWITCH(op, op_no, res,)
}

void Entity::onContainered(const Ref<LocatedEntity>& oldLocation)
{
    containered.emit(oldLocation);
}

void Entity::onUpdated()
{
    updated.emit();
}

Ref<LocatedEntity> Entity::createNewEntity(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        return {};
    }
    try {
        RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
        if (!ent.isValid()) {
            error(op, "Entity to be created is malformed", res, getId());
            return {};
        }
        auto obj = createNewEntity(ent);

        if (!obj) {
            error(op, "Create op failed.", res, op->getFrom());
            return {};
        }

        Anonymous new_ent;
        obj->addToEntity(new_ent);

        if (!op->isDefaultSerialno()) {
            Atlas::Objects::Operation::Info i;
            i->setArgs1(new_ent);
            i->setTo(op->getFrom());
            res.push_back(i);
        }

        Operation c(op.copy());
        c->setArgs1(new_ent);

        Sight s;
        s->setArgs1(c);
        //TODO: perhaps check that we don't send private and protected properties?
        broadcast(s, res, Visibility::PUBLIC);
        return obj;
    }
    catch (const std::runtime_error& e) {
        log(ERROR, String::compose("Error when trying to create entity: %1", e.what()));
        error(op, String::compose("Error when trying to create entity: %1", e.what()), res, getId());
        return {};
    }
}

Ref<LocatedEntity> Entity::createNewEntity(const RootEntity& ent)
{
    const std::string& type = ent->getParent();
    if (type.empty()) {
        throw std::runtime_error("Entity to be created has empty parent.");
    }

    //If there's no location set we'll use the same one as the current entity.
    if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG) && (m_location.m_parent)) {
        ent->setLoc(m_location.m_parent->getId());
    }
    debug_print(getId() << " creating " << type);

    return BaseWorld::instance().addNewEntity(type, ent);

}
