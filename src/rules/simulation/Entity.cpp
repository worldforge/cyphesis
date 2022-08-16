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
#include "common/TypeNode.h"
#include "common/Link.h"

#include "common/custom.h"
#include "common/operations/Relay.h"

#include "common/Monitors.h"
#include "common/Variable.h"
#include "ModeDataProperty.h"
#include "rules/AtlasProperties.h"
#include "rules/PhysicalProperties.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <wfmath/atlasconv.h>

#include <algorithm>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

std::unordered_map<const TypeNode*, int> Entity::s_monitorsMap;

/// \brief Flags used to control entities
///
/// These flags are used to indicate various aspects of entities.
/// \defgroup EntityFlags In World Entity Flags

/// \brief Classes that model in world entities
///
/// These classes are used to model all in world entities or objects.
/// \defgroup EntityClasses In World Entity Classes

Entity::Entity(RouterId id) : LocatedEntity(id)
{

}

Entity::~Entity()
{
    if (m_type) {
        auto I = s_monitorsMap.find(m_type);
        if (I != s_monitorsMap.end()) {
            I->second--;
        }
    }
}

std::unique_ptr<PropertyBase> Entity::createProperty(const std::string& propertyName) const
{
    return PropertyManager::instance().addProperty(propertyName);
}


void Entity::setType(const TypeNode* t)
{
    LocatedEntity::setType(t);

    if (t) {
        auto I = s_monitorsMap.find(t);
        if (I == s_monitorsMap.end()) {
            auto result = s_monitorsMap.emplace(t, 1);

            Monitors::instance().watch(String::compose("entity_count{type=\"%1\"}", t->name()), std::make_unique<Variable<int>>(result.first->second));
        } else {
            I->second++;
        }
    }
}


void Entity::addChild(LocatedEntity& childEntity)
{
    LocatedEntity::addChild(childEntity);
    if (getDomain()) {
        getDomain()->addEntity(childEntity);
    }
}

void Entity::removeChild(LocatedEntity& childEntity)
{
    if (getDomain()) {
        getDomain()->removeEntity(childEntity);
    }
    LocatedEntity::removeChild(childEntity);
}


/// \brief Copy attributes into an Atlas element
///
/// @param omap Atlas map element this entity should be copied into
void Entity::addToMessage(MapType& omap) const
{
    // We need to have a list of keys to pull from attributes.
    for (auto& entry : m_properties) {
        entry.second.property->add(entry.first, omap);
    }

    omap["stamp"] = (double) m_seq;
    omap["parent"] = m_type;
    omap["objtype"] = "obj";
}

/// \brief Copy attributes into an Atlas entity
///
/// @param ent Atlas entity this entity should be copied into
void Entity::addToEntity(const RootEntity& ent) const
{
    // We need to have a list of keys to pull from attributes.
    for (auto& entry : m_properties) {
        entry.second.property->add(entry.first, ent);
    }

    ent->setStamp(m_seq);
    if (m_type != nullptr) {
        ent->setParent(m_type->name());
    }
    ent->setObjtype("obj");
}

/// \brief Install a delegate property for an operation
///
/// @param class_no The class number of the operation to be handled
/// @param delegate The name of the property to delegate it to.
void Entity::installDelegate(int class_no, const std::string& delegate)
{
    m_delegates.emplace(class_no, delegate);
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
    m_flags.addFlags(entity_destroyed);

    if (m_contains && !m_contains->empty()) {
        //We can't iterate directly over m_contains since we will be modifying it.
        auto containsCopy = *m_contains;

        //Move all contained entities to the same location as this entity.
        //TODO: allow this behaviour to be changed for different scenarios.
        for (auto& child: containsCopy) {
            auto entity = dynamic_cast<Entity*>(child.get());
            if (entity) {
                Atlas::Objects::Operation::Move moveOp;
                RootEntity ent;
                ent->setId(entity->getId());
                ent->setAttr(ModeDataProperty::property_name, {});
                if (m_parent) {
                    ent->setLoc(m_parent->getId());
                }
                auto posProp = getPropertyClassFixed<PositionProperty>();
                if (posProp && posProp->data().isValid()) {
                    ent->setPos({posProp->data().x(), posProp->data().y(), posProp->data().z()});
                }
                auto orientProp = getPropertyClassFixed<OrientationProperty>();
                if (orientProp && orientProp->data().isValid()) {
                    orientProp->add(OrientationProperty::property_name, ent);
                }
                auto velocityProp = getPropertyClassFixed<VelocityProperty>();
                if (velocityProp && velocityProp->data().isValid()) {
                    velocityProp->add(VelocityProperty::property_name, ent);
                }
                moveOp->setArgs1(std::move(ent));
                OpVector res;

                entity->operation(moveOp, res);
                for (auto& resOp: res) {
                    entity->sendWorld(resOp);
                }
            }
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
    if (m_domain) {
        m_domain->removed();
    }
    m_domain = std::move(domain);
    if (m_domain) {
        addFlags(entity_domain);
    } else {
        removeFlags(entity_domain);
    }
}

void Entity::sendWorld(Operation op)
{
    BaseWorld::instance().message(std::move(op), *this);
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

    //Skip calling scripts for perception ops, since we don't expect any rule scripts ever acting on that kind of data.
    //TODO: switch the scripts calling feature over to using a system where each script at registration time registers what kind
    // of op they are listening to. That way we can actually let them listen to perception ops.
    if (!op->instanceOf(Atlas::Objects::Operation::PERCEPTION_NO)) {
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

    for (auto& listener : m_listeners) {
        HandlerResult hr_call = listener->operation(*this, op, res);
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
    return callOperation(op, res);
}

HandlerResult Entity::callDelegate(const std::string& name,
                                   const Operation& op,
                                   OpVector& res)
{
    PropertyBase* p = nullptr;
    auto I = m_properties.find(name);
    if (I != m_properties.end()) {
        p = I->second.property.get();
    } else if (m_type != nullptr) {
        auto J = m_type->defaults().find(name);
        if (J != m_type->defaults().end()) {
            p = J->second.get();
        }
    }
    if (p != nullptr) {
        return p->operation(*this, op, res);
    }
    return OPERATION_IGNORED;
}

void Entity::callOperation(const Operation& op, OpVector& res)
{
    auto op_no = op->getClassNo();
    switch (op_no) {
        case Atlas::Objects::Operation::DELETE_NO:
            DeleteOperation(op, res);
            break;
        case Atlas::Objects::Operation::IMAGINARY_NO:
            ImaginaryOperation(op, res);
            break;
        case Atlas::Objects::Operation::LOOK_NO:
            LookOperation(op, res);
            break;
        case Atlas::Objects::Operation::MOVE_NO:
            MoveOperation(op, res);
            break;
        case Atlas::Objects::Operation::SET_NO:
            SetOperation(op, res);
            break;
        case Atlas::Objects::Operation::TALK_NO:
            TalkOperation(op, res);
            break;
        case Atlas::Objects::Operation::CREATE_NO:
            CreateOperation(op, res);
            break;
        case OP_INVALID:
            break;
        default:
            if ((op_no) == Atlas::Objects::Operation::UPDATE_NO) {
                UpdateOperation(op, res);
            } else if ((op_no) == Atlas::Objects::Operation::RELAY_NO) {
                RelayOperation(op, res);
            } else {
                /* ERROR */
            }
            break;
    }
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
            i->setRefno(op->getSerialno());
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
    if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG) && (m_parent)) {
        ent->setLoc(m_parent->getId());
    }
    debug_print(getId() << " creating " << type)

    return BaseWorld::instance().addNewEntity(type, ent);

}
