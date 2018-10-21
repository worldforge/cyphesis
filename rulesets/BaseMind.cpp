#include <utility>

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

#include "Script.h"
#include "TypeResolver.h"
#include "SimpleTypeStore.h"

#include "common/custom.h"
#include "common/debug.h"
#include "common/op_switch.h"
#include "common/TypeNode.h"
#include "common/Inheritance.h"
#include "common/Setup.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

BaseMind::BaseMind(const std::string& mindId, const std::string& entityId) :
    Router(mindId, std::stol(mindId)),
    m_entityId(entityId),
    m_flags(0),
    m_typeStore(new SimpleTypeStore()),
    m_typeResolver(new TypeResolver(*m_typeStore)),
    m_map(*m_typeResolver),
    m_serialNoCounter(0)
{
}

BaseMind::~BaseMind() = default;


void BaseMind::init(OpVector& res)
{
    Look look;
    Root lookArg;
    lookArg->setId(m_entityId);
    look->setArgs1(lookArg);
    look->setFrom(getId());
    res.push_back(look);
}


/// \brief Process the Sight of a Create operation.
///
/// @param op The Create operation to be processed.
/// @param res The result of the operation is returned here.
void BaseMind::sightCreateOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        debug(std::cout << " no args!" << std::endl << std::flush;);
        return;
    }
    RootEntity ent(smart_dynamic_cast<RootEntity>(args.front()));
    if (!ent.isValid()) {
        log(ERROR, "Got sight(create) of non-entity");
        return;
    }
    // This does not send a look, so anything added this way will not
    // get flagged as visible until we get an appearance. This is important.
    m_map.updateAdd(ent, op->getSeconds());
}

/// \brief Process the Sight of a Delete operation.
///
/// @param op The Delete operation to be processed.
/// @param res The result of the operation is returned here.
void BaseMind::sightDeleteOperation(const Operation& op, OpVector& res)
{
    debug(std::cout << "Sight Delete operation" << std::endl << std::flush;);
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        debug(std::cout << " no args!" << std::endl << std::flush;);
        return;
    }
    const Root& obj = args.front();
    if (!obj.isValid()) {
        log(ERROR, "Sight Delete with invalid entity");
        return;
    }
    const std::string& id = obj->getId();
    if (!id.empty()) {
        m_map.del(obj->getId());
        m_pendingEntitiesOperations.erase(obj->getId());
    } else {
        log(WARNING, "Sight Delete with no ID");
    }
}

/// \brief Process the Sight of a Move operation.
///
/// @param op The Move operation to be processed.
/// @param res The result of the operation is returned here.
void BaseMind::sightMoveOperation(const Operation& op, OpVector& res)
{
    debug(std::cout << "BaseMind::sightOperation(Sight, Move)" << std::endl << std::flush;);
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        debug(std::cout << " no args!" << std::endl << std::flush;);
        return;
    }
    RootEntity ent(smart_dynamic_cast<RootEntity>(args.front()));
    if (!ent.isValid()) {
        log(ERROR, "Got sight(move) of non-entity");
        return;
    }
    m_map.updateAdd(ent, op->getSeconds());
}

/// \brief Process the Sight of a Set operation.
///
/// @param op The Set operation to be processed.
/// @param res The result of the operation is returned here.
void BaseMind::sightSetOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        debug(std::cout << " no args!" << std::endl << std::flush;);
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
        debug(std::cout << " no args!" << std::endl << std::flush;);
        return;
    }
    const Root& arg = args.front();
    Operation op2(Atlas::Objects::smart_dynamic_cast<Operation>(arg));
    if (op2.isValid()) {
        debug(std::cout << " args is an op!" << std::endl << std::flush;);
        std::string event_name("sound_");
        event_name += op2->getParent();

        if (m_script || m_script->operation(event_name, op2, res) != OPERATION_BLOCKED) {
            callSoundOperation(op2, res);
        }
    }
}

void BaseMind::SightOperation(const Operation& op, OpVector& res)
{
    debug_print("BaseMind::SightOperation(Sight)");
    // Deliver argument to sight things
    if (!isAwake()) { return; }
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        debug_print(" no args!");
        return;
    }
    const Root& arg = args.front();
    Operation op2(Atlas::Objects::smart_dynamic_cast<Operation>(arg));
    if (op2.isValid()) {
        debug_print(String::compose(" args is an op (%1)!", op2->getParent()));
        std::string event_name("sight_");
        event_name += op2->getParent();

        //Check that the argument had seconds set; if not the timestamp of the updates will be wrong.
        if (!op2->hasAttrFlag(Atlas::Objects::Operation::SECONDS_FLAG)) {
            //Copy from wrapping op to fix this. This indicates an error in the server.
            op2->setSeconds(op->getSeconds());
            log(WARNING, String::compose("Sight op argument ('%1') had no seconds set.", op2->getParent()));
        }

        if (m_script || m_script->operation(event_name, op2, res) != OPERATION_BLOCKED) {
            callSightOperation(op2, res);
        }
    } else /* if (op2->getObjtype() == "object") */ {
        RootEntity ent(Atlas::Objects::smart_dynamic_cast<RootEntity>(arg));
        if (!ent.isValid()) {
            log(ERROR, "Arg of sight operation is not an op or an entity");
            return;
        }
        debug_print(" arg is an entity!");
        auto me = m_map.updateAdd(ent, op->getSeconds());
        if (me) {
            me->setVisible();
        }
    }
}

void BaseMind::ThinkOperation(const Operation& op, OpVector& res)
{
    //Get the contained op
    debug_print("BaseMind::ThinkOperation(Think)");
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        debug_print(" no args!");
        return;
    }
    const Root& arg = args.front();
    Operation op2(Atlas::Objects::smart_dynamic_cast<Operation>(arg));
    if (op2.isValid()) {
        debug_print(" args is an op!");
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
            res.push_back(resOp);
        }

    }
}

void BaseMind::AppearanceOperation(const Operation& op, OpVector& res)
{
    if (!isAwake()) { return; }
    const std::vector<Root>& args = op->getArgs();
    auto Iend = args.end();
    for (auto I = args.begin(); I != Iend; ++I) {
        if (!(*I)->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            log(ERROR, "BaseMind: Appearance op does not have ID");
            continue;
        }
        const std::string& id = (*I)->getId();
        auto me = m_map.getAdd(id);
        if (me) {
            if ((*I)->hasAttrFlag(Atlas::Objects::STAMP_FLAG)) {
                if ((int) (*I)->getStamp() != me->getSeq()) {
                    Look l;
                    Anonymous m;
                    m->setId(id);
                    l->setArgs1(m);
                    res.push_back(l);
                }
            } else {
                log(ERROR, "BaseMind: Appearance op does not have stamp");
            }
            me->update(op->getSeconds());
            me->setVisible();
        }
    }
}

void BaseMind::DisappearanceOperation(const Operation& op, OpVector& res)
{
    if (!isAwake()) { return; }
    const std::vector<Root>& args = op->getArgs();
    for (const auto& arg : args) {
        const std::string& id = arg->getId();
        if (id.empty()) { continue; }
        auto me = m_map.get(id);
        if (me) {
            me->update(op->getSeconds());
            me->setVisible(false);
        }
    }
}

void BaseMind::UnseenOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        debug(std::cout << " no args!" << std::endl << std::flush;);
        return;
    }
    const Root& arg = args.front();
    if (arg->isDefaultId()) {
        log(ERROR, "BaseMind: Unseen op has no arg ID");
        return;
    }
    m_map.del(arg->getId());
    m_pendingEntitiesOperations.erase(arg->getId());

}

void BaseMind::setOwnEntity(OpVector& res, Ref<MemEntity> ownEntity)
{
    m_ownEntity = std::move(ownEntity);
    m_ownEntity->propertyApplied.connect([&](const std::string& name, const PropertyBase&) {
        if (m_script) {
            auto I = m_propertyScriptCallbacks.find(name);
            if (I != m_propertyScriptCallbacks.end()) {
                m_script->hook(I->second, m_ownEntity.get());
            }
        }
    });

    if (m_script) {
        //If there are any property callbacks registered call them now.
        for (auto& entry : m_propertyScriptCallbacks) {
            if (m_ownEntity->hasAttr(entry.first)) {
                m_script->hook(entry.second, m_ownEntity.get());
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

    //Start by sending a unspecified "Look". This tells the server to send us a bootstrapped view.
    Look l;
    l->setFrom(getId());
    res.push_back(l);

    res.insert(std::end(res), std::begin(m_pendingOperations), std::end(m_pendingOperations));
    m_pendingOperations.clear();
}


void BaseMind::InfoOperation(const Operation& op, OpVector& res)
{
    if (m_typeResolver) {
        auto resolvedTypes = m_typeResolver->InfoOperation(op, res);
        //For any resolved types, find any unresolved entities, set their type and put any pending operations in "res".
        for (auto& type : resolvedTypes) {
            log(INFO, String::compose("Resolved type '%1'.", type->name()));

            auto resolved = m_map.resolveEntitiesForType(type);
            for (auto& entity : resolved) {

                log(INFO, String::compose("Resolved entity %1.", entity->getId()));
                entity->setType(type);

                auto J = m_pendingEntitiesOperations.find(entity->getId());
                if (J != m_pendingEntitiesOperations.end()) {
                    res.insert(std::end(res), std::begin(J->second), std::end(J->second));
                    m_pendingEntitiesOperations.erase(J);
                }

                //If we have resolved our own entity we should do some house keeping
                if (entity->getId() == m_entityId) {
                    log(INFO, String::compose("Resolved own entity for %1.", entity->describeEntity()));
                    setOwnEntity(res, entity);
                }
            }
        }
    }

}

void BaseMind::addPropertyScriptCallback(std::string propertyName, std::string scriptMethod)
{
    m_propertyScriptCallbacks.emplace(propertyName, scriptMethod);
    if (m_ownEntity && m_script) {
        if (m_ownEntity->hasAttr(propertyName)) {
            m_script->hook(scriptMethod, m_ownEntity.get());
        }
    }
}


void BaseMind::operation(const Operation& op, OpVector& res)
{
    // This might end up being quite tricky to do

    // In the python the following happens here:
    //   Find out if the op refers to any ids we don't know about.
    //   If so create look operations to those ids
    //   Set the minds time and date
    if (debug_flag) {
        std::cout << "BaseMind::operation received {" << std::endl;
        debug_dump(op, std::cout);
        std::cout << "}" << std::endl << std::flush;
    }

    int op_no = op->getClassNo();
    m_time.update((int) op->getSeconds());


    if (op_no == Atlas::Objects::Operation::INFO_NO) {
        InfoOperation(op, res);
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


        m_map.check(op->getSeconds());

        bool isPending = false;
        //Unless it's an Unseen op, we should add the entity the op was from.
        if (op_no != Atlas::Objects::Operation::UNSEEN_NO && !op->isDefaultFrom()) {
            auto entity = m_map.getAdd(op->getFrom());
            if (!entity) {
                m_pendingEntitiesOperations[op->getFrom()].push_back(op);
                isPending = true;
            }
        }
        if (!isPending) {
            m_map.sendLooks(res);
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

    m_map.collectTypeResolverOps(res);

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


void BaseMind::callSightOperation(const Operation& op,
                                  OpVector& res)
{
    m_map.getAdd(op->getFrom());
    auto op_no = op->getClassNo();
    if (debug_flag && (op_no == OP_INVALID)) {
        debug(std::cout << getId() << " could not deliver sight of "
                        << op->getParent()
                        << std::endl << std::flush;);
    }
    SUB_OP_SWITCH(op, op_no, res, sight)
}

void BaseMind::callSoundOperation(const Operation& op,
                                  OpVector& res)
{
    // This function essentially does nothing now, except add the source
    // of the sound op to the map.
    m_map.getAdd(op->getFrom());
    auto op_no = op->getClassNo();
    if (debug_flag && (op_no == OP_INVALID)) {
        debug(std::cout << getId() << " could not deliver sound of "
                        << op->getParent()
                        << std::endl << std::flush;);
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

void BaseMind::setScript(Script* scrpt)
{
    m_script.reset(scrpt);
    m_map.setScript(scrpt);
    if (m_script && m_ownEntity) {
        //If there are any property callbacks registered call them now.
        for (auto& entry : m_propertyScriptCallbacks) {
            if (m_ownEntity->hasAttr(entry.first)) {
                m_script->hook(entry.second, m_ownEntity.get());
            }
        }
    }
}

const TypeStore& BaseMind::getTypeStore() const
{
    return *m_typeStore;
}

