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


#include "BaseMind.h"

#include "rules/Script.h"
#include "TypeResolver.h"
#include "rules/SimpleTypeStore.h"

#include "common/custom.h"
#include "common/debug.h"
#include "common/TypeNode.h"
#include "common/operations/Setup.h"
#include "common/operations/Tick.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include "Remotery.h"

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

BaseMind::BaseMind(RouterId mindId, std::string entityId, TypeStore& typeStore) :
        Router(std::move(mindId)),
        m_entityId(std::move(entityId)),
        m_flags(0),
        m_typeStore(typeStore),
        m_typeResolver(new TypeResolver(m_typeStore)),
        m_map(*m_typeResolver),
        mServerTime(0),
        m_serialNoCounter(0),
        m_scriptFactory(nullptr)
{
    m_tickControl.navmesh.interval = std::chrono::milliseconds(600);
    m_tickControl.move.interval = std::chrono::milliseconds(20);
    m_tickControl.think.interval = std::chrono::milliseconds(500);
    m_typeResolver->m_typeProviderId = mindId.m_id;
    m_map.setListener(this);
}

BaseMind::~BaseMind()
{
    destroy();
}


void BaseMind::init(OpVector& res)
{

    //Initiate the ticks.
    processTick(res);

    Look look;
    Root lookArg;
    lookArg->setId(m_entityId);
    look->setArgs1(std::move(lookArg));
    look->setFrom(getId());
    res.emplace_back(std::move(look));
}

void BaseMind::destroy()
{
    m_map.flush();
    m_flags.addFlags(entity_destroyed);
    setScript(nullptr);
}

/// \brief Process the Sight of a Set operation.
///
/// @param op The Set operation to be processed.
/// @param res The result of the operation is returned here.
void BaseMind::sightSetOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        debug_print(" no args!")
        return;
    }
    RootEntity ent(smart_dynamic_cast<RootEntity>(args.front()));
    if (!ent.isValid()) {
        log(ERROR, "Got sight(set) of non-entity");
        return;
    }
    m_map.updateAdd(ent, op->getSeconds());
}

void BaseMind::SoundOperation(const Operation& op, OpVector& res)
{
    // Deliver argument to sound things
    // Louder sounds might eventually make character wake up
    if (!isAwake()) { return; }
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        debug_print(" no args!")
        return;
    }
    const Root& arg = args.front();
    Operation op2(Atlas::Objects::smart_dynamic_cast<Operation>(arg));
    if (op2.isValid()) {
        debug_print(" args is an op!")
        std::string event_name("sound_");
        event_name += op2->getParent();

        if (!m_script || m_script->operation(event_name, op2, res) != OPERATION_BLOCKED) {
            callSoundOperation(op2, res);
        }
    }
}

void BaseMind::SightOperation(const Operation& op, OpVector& res)
{
    debug_print("BaseMind::SightOperation(Sight)")
    // Deliver argument to sight things
    if (!isAwake()) { return; }
    if (op->isDefaultSeconds()) {
        log(ERROR, "Sight operation had no seconds set, ignoring it.");
        return;
    }

    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        debug_print(" no args!")
        return;
    }
    const Root& arg = args.front();
    Operation op2(Atlas::Objects::smart_dynamic_cast<Operation>(arg));
    if (op2.isValid()) {
        debug_print(String::compose(" args is an op (%1)!", op2->getParent()))
        std::string event_name("sight_");
        event_name += op2->getParent();

        //Check that the argument had seconds set; if not the timestamp of the updates will be wrong.
        if (!op2->hasAttrFlag(Atlas::Objects::Operation::SECONDS_FLAG)) {
            //Copy from wrapping op to fix this.
            op2->setSeconds(op->getSeconds());
        }

        if (!m_script || m_script->operation(event_name, op2, res) != OPERATION_BLOCKED) {
            callSightOperation(op2, res);
        }
    } else /* if (op2->getObjtype() == "object") */ {
        RootEntity ent(Atlas::Objects::smart_dynamic_cast<RootEntity>(arg));
        if (!ent.isValid()) {
            log(ERROR, "Arg of sight operation is not an op or an entity");
            return;
        }
        debug_print(" arg is an entity!")
        auto me = m_map.updateAdd(ent, op->getSeconds());
        if (me) {
            me->setVisible();
        }
    }
}

void BaseMind::ThinkOperation(const Operation& op, OpVector& res)
{
    //Get the contained op
    debug_print("BaseMind::ThinkOperation(Think)")
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        debug_print(" no args!")
        return;
    }
    const Root& arg = args.front();
    Operation op2(Atlas::Objects::smart_dynamic_cast<Operation>(arg));
    if (op2.isValid()) {
        debug_print(" args is an op!")
        std::string event_name("think_");
        event_name += op2->getParent();

        OpVector mres;

        if (m_script) {
            m_script->operation(event_name, op2, mres);
        }

        if (!mres.empty() && !op->isDefaultSerialno()) {
            mres.front()->setRefno(op->getSerialno());
        }
        for (auto& resOp : mres) {
            res.emplace_back(std::move(resOp));
        }

    }
}

void BaseMind::AppearanceOperation(const Operation& op, OpVector& res)
{
    if (!isAwake()) { return; }
    auto& args = op->getArgs();
    for (auto& arg : args) {
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            log(ERROR, "BaseMind: Appearance op does not have ID");
            continue;
        }
        const std::string& id = arg->getId();
        auto entity = m_map.getAdd(id);
        if (entity) {
            if (arg->hasAttrFlag(Atlas::Objects::STAMP_FLAG)) {
                //TODO: look over this whole system, so it's either implemented correctly or removed.
                if ((int) arg->getStamp() != entity->getSeq()) {
                    Look l;
                    Anonymous m;
                    m->setId(id);
                    l->setArgs1(m);
                    res.push_back(l);
                }
            } else {
                log(ERROR, "BaseMind: Appearance op does not have stamp");
            }
            entity->update(op->getSeconds());
            entity->setVisible();
        }
    }
}

void BaseMind::removeEntity(const std::string& id, OpVector& res)
{
    m_map.del(id);
    m_pendingEntitiesOperations.erase(id);
    if (m_ownEntity && m_ownEntity->getId() == id) {
        destroy();
    }
}


void BaseMind::DisappearanceOperation(const Operation& op, OpVector& res)
{
    if (!isAwake()) {
        return;
    }
    auto& args = op->getArgs();
    if (args.empty()) {
        debug_print(" no args!")
        return;
    }
    for (auto& arg : args) {
        if (!arg->isDefaultId()) {
            auto& id = arg->getId();
            if (id.empty()) {
                continue;
            }
            removeEntity(id, res);
        }
    }
}

void BaseMind::UnseenOperation(const Operation& op, OpVector& res)
{
    if (!isAwake()) {
        return;
    }
    auto& args = op->getArgs();
    if (args.empty()) {
        debug_print(" no args!")
        return;
    }
    for (auto& arg : args) {
        if (!arg->isDefaultId()) {
            auto& id = arg->getId();
            if (id.empty()) {
                continue;
            }
            removeEntity(id, res);
        }
    }
}

void BaseMind::setOwnEntity(OpVector& res, Ref<MemEntity> ownEntity)
{
    m_ownEntity = std::move(ownEntity);

    if (m_scriptFactory) {
        m_scriptFactory->addScript(*this);
    }


    m_ownEntity->propertyApplied.connect([&](const std::string& name, const PropertyBase&) {
        if (m_script) {
            auto I = m_propertyScriptCallbacks.find(name);
            if (I != m_propertyScriptCallbacks.end()) {
                m_script->hook(I->second, m_ownEntity.get(), res);
            }
        }
    });

    if (m_script) {
        //If there are any property callbacks registered call them now.
        for (auto& entry : m_propertyScriptCallbacks) {
            if (m_ownEntity->hasAttr(entry.first)) {
                m_script->hook(entry.second, m_ownEntity.get(), res);
            }
        }
    }


    //Also send a "Setup" op to the mind, which will trigger any setup hooks.
    Atlas::Objects::Operation::Setup s;
    Anonymous setup_arg;
    setup_arg->setName("mind");
    s->setTo(getId());
    s->setFrom(getId());
    s->setArgs1(setup_arg);
    operation(s, res);

    //Start by sending an unspecified "Look". This tells the server to send us a bootstrapped view.
    Look l;
    l->setFrom(getId());
    res.push_back(l);
}


void BaseMind::InfoOperation(const Operation& op, OpVector& res)
{
    if (m_typeResolver) {
        auto resolvedTypes = m_typeResolver->InfoOperation(op, res);
        //For any resolved types, find any unresolved entities, set their type and put any pending operations in "res".
        for (auto& type : resolvedTypes) {
            //log(NOTICE, String::compose("Mind %1: Resolved type '%2'.", getId(), type->name()));

            auto resolved = m_map.resolveEntitiesForType(type);
            for (auto& entity : resolved) {

                //log(NOTICE, String::compose("Mind %1: Resolved entity %2.", getId(), entity->getId()));

                auto J = m_pendingEntitiesOperations.find(entity->getId());
                if (J != m_pendingEntitiesOperations.end()) {
                    res.insert(std::end(res), std::begin(J->second), std::end(J->second));
                    m_pendingEntitiesOperations.erase(J);
                }

                //If we have resolved our own entity we should do some house keeping
                if (entity->getId() == m_entityId) {
                    log(NOTICE, String::compose("%1: Resolved own entity for %2.", getId(), entity->describeEntity()));
                    setOwnEntity(res, entity);
                }
            }
        }
    }
}

void BaseMind::ErrorOperation(const Operation& op, OpVector& res)
{
    auto args = op->getArgs();
    if (!args.empty()) {
        const Atlas::Objects::Root& arg = args.front();
        Atlas::Message::Element message;
        if (arg->copyAttr("message", message) == 0 && message.isString()) {
            if (getEntity()) {
                log(WARNING, String::compose("BaseMind %1, entity %2, error from server: %3", getId(), getEntity()->describeEntity(), message.String()));
            } else {
                log(WARNING, String::compose("BaseMind %1, error from server: %2", getId(), message.String()));
            }
        } else {
            if (getEntity()) {
                log(WARNING, String::compose("BaseMind %1, entity %2, unspecified error from server", getId(), getEntity()->describeEntity()));
            } else {
                log(WARNING, String::compose("BaseMind %1, unspecified error from server.", getId()));
            }
        }
    }
}

void BaseMind::addPropertyScriptCallback(std::string propertyName, std::string scriptMethod)
{
    m_propertyScriptCallbacks.emplace(propertyName, scriptMethod);
    if (m_ownEntity && m_script) {
        if (m_ownEntity->hasAttr(propertyName)) {
            OpVector res;
            m_script->hook(scriptMethod, m_ownEntity.get(), res);
            std::move(res.begin(), res.end(), std::back_inserter(mOutgoingOperations));
        }
    }
}

void BaseMind::updateServerTimeFromOperation(const Atlas::Objects::Operation::RootOperationData& op)
{
    //It's ok if the server sends an op without 'seconds' set.
    if (!op.isDefaultSeconds()) {
        //Alert if there's a too large difference in time.
        if (op.getSeconds() - mServerTime < -30) {
            log(WARNING, String::compose("Operation '%1' has seconds set (%2) earlier than already recorded seconds (%3).", op.getParent(), op.getSeconds(), mServerTime));
        }
        mServerTime = op.getSeconds();
    }
}


void BaseMind::operation(const Operation& op, OpVector& res)
{
    rmt_ScopedCPUSample(operation, 0)

    if (debug_flag) {
        std::cout << "BaseMind::operation received {" << std::endl;
        debug_dump(op, std::cout);
        std::cout << "}" << std::endl << std::flush;
    }

    int op_no = op->getClassNo();
    updateServerTimeFromOperation(*op);

    if (op_no == Atlas::Objects::Operation::INFO_NO) {
        InfoOperation(op, res);
    } else if (op_no == Atlas::Objects::Operation::ERROR_NO) {
        ErrorOperation(op, res);
    } else if (op_no == Atlas::Objects::Operation::TICK_NO) {
        processTick(res);
    } else {

        //If we haven't yet resolved our own entity we should delay delivery of the ops
        if (!m_ownEntity) {
            if (!op->isDefaultFrom()) {
                if (op->getFrom() != m_entityId) {
                    m_pendingOperations.push_back(op);
                    return;
                }
            } else {
                log(WARNING, String::compose("Got %1 operation without any 'from'.", op->getParent()));
            }
        }


        bool isPending = false;
        //Unless it's an Unseen op, we should add the entity the op was from.
        //Note that we might get an op from the account itself, so we need to check for that.
        if (op_no != Atlas::Objects::Operation::UNSEEN_NO && !op->isDefaultFrom() && op->getFrom() != getId()) {
            auto entity = m_map.getAdd(op->getFrom());
            if (!entity) {
                m_pendingEntitiesOperations[op->getFrom()].push_back(op);
                isPending = true;
            }
        }
        if (!isPending) {

            if (m_script) {
                m_script->operation("call_triggers", op, res);
                m_script->operation(op->getParent(), op, res);
                //                    if (m_script->operation(op->getParent(), op, res) == OPERATION_BLOCKED) {
                //                        return;
                //                    }
            }
            switch (op_no) {
                case Atlas::Objects::Operation::SIGHT_NO:
                    SightOperation(op, res);
                    break;
                case Atlas::Objects::Operation::SOUND_NO:
                    SoundOperation(op, res);
                    break;
                case Atlas::Objects::Operation::APPEARANCE_NO:
                    AppearanceOperation(op, res);
                    break;
                case Atlas::Objects::Operation::DISAPPEARANCE_NO:
                    DisappearanceOperation(op, res);
                    break;
                default:
                    if (op_no == Atlas::Objects::Operation::UNSEEN_NO) {
                        UnseenOperation(op, res);
                    } else if (op_no == Atlas::Objects::Operation::THINK_NO) {
                        ThinkOperation(op, res);
                    }
                    // ERROR
                    break;
            }
        }
    }


    for (auto& resOp : res) {
        if (resOp->isDefaultFrom()) {
            resOp->setFrom(getId());
        }
    }

    if (debug_flag) {
        for (const auto& resOp : res) {
            std::cout << "BaseMind::operation sent {" << std::endl;
            debug_dump(resOp, std::cout);
            std::cout << "}" << std::endl << std::flush;
        }
    }
}

void BaseMind::externalOperation(const Operation& op, Link&)
{

}

void BaseMind::processTick(OpVector& res)
{
    rmt_ScopedCPUSample(tick, 0)
    //Start by scheduling the next tick op.
    Atlas::Objects::Operation::Tick tick;
    //Do one tick every 10ms. (to be revisited)
    tick->setFutureSeconds(0.01);
    tick->setTo(getId());
    tick->setFrom(getId());
    res.emplace_back(std::move(tick));

    //Do some housekeeping first.
    m_map.check(mServerTime);
    if (m_ownEntity) {

        //Check if we should think?
        auto now = std::chrono::steady_clock::now();
        if (now >= m_tickControl.think.next) {
            rmt_ScopedCPUSample(think, 0)
            m_script->hook("think", m_ownEntity.get(), res);
            m_tickControl.think.next = now + m_tickControl.think.interval;
        }

        //Check if we should do move updates?
        if (now >= m_tickControl.move.next) {
            rmt_ScopedCPUSample(move, 0)
            processMove(res);
            m_tickControl.move.next = now + m_tickControl.move.interval;
        }

        //Check if we should do navmesh updates?
        if (now >= m_tickControl.navmesh.next) {
            rmt_ScopedCPUSample(navmesh, 0)
            processNavmesh();
            m_tickControl.navmesh.next = now + m_tickControl.navmesh.interval;
        }

        //Fill up with any pending ops.
        if (!m_pendingOperations.empty()) {
            res.emplace_back(std::move(m_pendingOperations.front()));
            m_pendingOperations.pop_front();
        }

        if (!mOutgoingOperations.empty()) {
            res.emplace_back(std::move(mOutgoingOperations.front()));
            mOutgoingOperations.pop_front();
        }
    }
    if (!m_map.getTypeResolverOps().empty()) {
        res.emplace_back(std::move(m_map.getTypeResolverOps().front()));
        m_map.getTypeResolverOps().pop_front();
    }

    m_map.sendLook(res);
}


void BaseMind::callSightOperation(const Operation& op,
                                  OpVector& res)
{
    m_map.getAdd(op->getFrom());
    auto op_no = op->getClassNo();
    if (debug_flag && (op_no == OP_INVALID)) {
        debug_print(getId() << " could not deliver sight of "
                        << op->getParent());
    }
    if (op_no == Atlas::Objects::Operation::SET_NO) {
        sightSetOperation(op, res);
    }
}

void BaseMind::callSoundOperation(const Operation& op,
                                  OpVector& res)
{
    // This function essentially does nothing now, except add the source
    // of the sound op to the map.
    m_map.getAdd(op->getFrom());
    auto op_no = op->getClassNo();
    if (debug_flag && (op_no == OP_INVALID)) {
        debug_print(getId() << " could not deliver sound of "
                            << op->getParent())
    }

#if 0
    const MemEntityDict & ents = m_map.getEntities();
    MemEntityDict::const_iterator Iend = ents.end();
    for (MemEntityDict::const_iterator I = ents.begin(); I != Iend; ++I) {
        std::cout << I->second->getId() << ":" << I->second->getType() << " is "
                  << ( I->second->isVisible() ? "visible " : "hid " )
                  << I->second->lastSeen()
                  << std::endl << std::flush;
    }
#endif

    // SUB_OP_SWITCH(op, op_no, res, sound)
}

void BaseMind::setScript(std::unique_ptr<Script> scrpt)
{
    m_script = std::move(scrpt);
    if (m_script && m_ownEntity) {
        m_script->attachPropertyCallbacks(*m_ownEntity);
        OpVector res;
        //If there are any property callbacks registered call them now.
        for (auto& entry : m_propertyScriptCallbacks) {
            if (m_ownEntity->hasAttr(entry.first)) {
                m_script->hook(entry.second, m_ownEntity.get(), res);
            }
        }
        for (auto& resOp: res) {
            mOutgoingOperations.emplace_back(std::move(resOp));
        }
    }
}

const TypeStore& BaseMind::getTypeStore() const
{
    return m_typeStore;
}

std::string BaseMind::describeEntity() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

void BaseMind::entityAdded(MemEntity& entity)
{
    if (!m_ownEntity) {
        //If we have resolved our own entity we should do some house keeping
        if (entity.getId() == m_entityId) {
            log(NOTICE, String::compose("%1: Added own entity for %2.", getId(), entity.describeEntity()));
            OpVector res;
            setOwnEntity(res, Ref<MemEntity>(&entity));
            for (auto& resOp: res) {
                mOutgoingOperations.emplace_back(std::move(resOp));
            }
        }
    }
    if (!m_addHook.empty() && m_script) {
        OpVector res;
        m_script->hook(m_addHook, &entity, res);
        for (auto& resOp: res) {
            mOutgoingOperations.emplace_back(std::move(resOp));
        }
    }
}

void BaseMind::entityUpdated(MemEntity& entity, const Atlas::Objects::Entity::RootEntity& ent, LocatedEntity* oldLocation)
{
    if (!m_updateHook.empty() && m_script) {
        OpVector res;
        m_script->hook(m_updateHook, &entity, res);
        for (auto& resOp: res) {
            mOutgoingOperations.emplace_back(std::move(resOp));
        }
    }
}

void BaseMind::entityDeleted(MemEntity& entity)
{
    if (!m_deleteHook.empty() && m_script) {
        OpVector res;
        m_script->hook(m_deleteHook, &entity, res);
        for (auto& resOp: res) {
            mOutgoingOperations.emplace_back(std::move(resOp));
        }
    }
}


std::ostream& operator<<(std::ostream& s, const BaseMind& d)
{
    if (d.m_ownEntity) {
        s << d.getId() << ", entity: " << *d.m_ownEntity;
    } else {
        s << d.getId() << ", entity id: " << d.m_entityId;
    }
    return s;
}

