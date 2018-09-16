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

#include "common/custom.h"
#include "common/debug.h"
#include "common/op_switch.h"
#include "common/TypeNode.h"
#include "common/Inheritance.h"

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

/// \brief BaseMind constructor
///
/// @param id String identifier
/// @param intId Integer identifier
/// @param body_name The name attribute of the body this mind controls
BaseMind::BaseMind(const std::string& id, long intId) :
    MemEntity(id, intId),
    m_map(),
    m_serialNoCounter(0)
{
    setVisible(true);
//    m_map.addEntity(this);
}

BaseMind::~BaseMind()
{
    m_map.m_entities.erase(getIntId());
    // FIXME Remove this once MemMap uses parent refcounting
    m_location.m_parent = nullptr;
    // debug(std::cout << getId() << ":" << getType() << " flushing mind with "
    // << m_map.getEntities().size() << " entities in it"
    // << std::endl << std::flush;);
    m_map.flush();
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

        if (m_scripts.empty() || m_scripts.front()->operation(event_name, op2, res) != OPERATION_BLOCKED) {
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

        if (m_scripts.empty() || m_scripts.front()->operation(event_name, op2, res) != OPERATION_BLOCKED) {
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

        if (m_scripts.empty() || m_scripts.front()->operation(event_name, op2, mres) != OPERATION_BLOCKED) {
            int op2ClassNo = op2->getClassNo();
            switch (op2ClassNo) {
                case Atlas::Objects::Operation::SET_NO:
                    thinkSetOperation(op2, mres);
                    break;
                case Atlas::Objects::Operation::DELETE_NO:
                    thinkDeleteOperation(op2, mres);
                    break;
                case Atlas::Objects::Operation::GET_NO:
                    thinkGetOperation(op2, mres);
                    break;
                case Atlas::Objects::Operation::LOOK_NO:
                    thinkLookOperation(op2, mres);
                    break;
                default:
                    log(WARNING, "Got invalid Think operation. We only support 'Set', 'Delete', 'Get' and 'Look'.");
                    break;
            }
        }

        if (!mres.empty() && !op->isDefaultSerialno()) {
            mres.front()->setRefno(op->getSerialno());
        }
        for (auto& resOp : mres) {
            res.push_back(resOp);
        }

    }
}

void BaseMind::thinkSetOperation(const Operation& op, OpVector& res)
{
}

void BaseMind::thinkDeleteOperation(const Operation& op, OpVector& res)
{
}

void BaseMind::thinkGetOperation(const Operation& op, OpVector& res)
{
}

void BaseMind::thinkLookOperation(const Operation& op, OpVector& res)
{
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
    auto Iend = args.end();
    for (auto I = args.begin(); I != Iend; ++I) {
        const std::string& id = (*I)->getId();
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
}

void BaseMind::InfoOperation(const Operation& op, OpVector& res)
{
    if (m_typeResolver) {
        auto resolvedTypes = m_typeResolver->InfoOperation(op, res);
        //For any resolved types, find any unresolved entities, set their type and put any pending operations in "res".
        for (auto& type : resolvedTypes) {
            auto I = m_map.m_unresolvedEntities.find(type->name());
            if (I != m_map.m_unresolvedEntities.end()) {
                for (auto& entity : I->second) {
                    entity->setType(type);
                    auto J = m_pendingEntitiesOperations.find(entity->getId());
                    if (J != m_pendingEntitiesOperations.end()) {
                        res.insert(std::end(res), std::begin(J->second), std::end(J->second));
                    }
                    m_pendingEntitiesOperations.erase(J);
                }
            }
            m_map.m_unresolvedEntities.erase(I);
        }
    }

}

void BaseMind::setMindId(const std::string& mindId)
{
    m_mindId = mindId;
}

const std::string& BaseMind::getMindId() const
{
    return m_mindId;
}

void BaseMind::setTypeResolver(std::unique_ptr<TypeResolver> typeResolver)
{
    m_typeResolver = std::move(typeResolver);
}

//
//void BaseMind::setup(OpVector& res)
//{
//    Look look;
//    Root lookArg;
//    lookArg->setId(getId());
//    look->setArgs1(lookArg);
//    look->setFrom(getMindId());
//    look->setSerialno(m_serialNoCounter++);
//
//    m_callbacks[look->getSerialno()] = [this](const Operation& op, OpVector& res){
//        if (op->getClassNo() != Atlas::Objects::Operation::SIGHT_NO || op->getArgs().empty()) {
//            log(ERROR, "Malformed initial sight when setting up mind.");
//            return;
//        }
//
//        auto ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(op->getArgs().front());
//
//        if (!ent || ent->isDefaultParent()) {
//            log(ERROR, "Malformed initial sight when setting up mind.");
//            return;
//        }
//
//        auto parent = ent->getParent();
//
//        auto typeNode = Inheritance::instance().getType(parent);
//
//        if (!typeNode) {
//            m_typeResolver->requestType(parent, res);
//        } else {
//
//        }
//
//
//
//        m_typeResolver->
//
//
//    };
//
//    res.push_back(look);
//
//}

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
    if (!op->isDefaultRefno()) {
        auto I = m_callbacks.find(op->getRefno());
        if (I != m_callbacks.end()) {
            I->second(op, res);
            m_callbacks.erase(I);
        }
    } else {


        if (op_no == Atlas::Objects::Operation::INFO_NO) {
            InfoOperation(op, res);
        } else {

            m_map.check(op->getSeconds());

            //Unless it's an Unseen op, we should add the entity the op was from.
            if (op_no != Atlas::Objects::Operation::UNSEEN_NO && !op->isDefaultFrom()) {
                auto entity = m_map.getAdd(op->getFrom());
                if (!entity) {
                    m_pendingEntitiesOperations[op->getFrom()].push_back(op);
                    return;
                }
            }
            m_map.sendLooks(res);
            if (!m_scripts.empty()) {
                m_scripts.front()->operation("call_triggers", op, res);
                if (m_scripts.front()->operation(op->getParent(), op, res) == OPERATION_BLOCKED) {
                    return;
                }
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
    if (debug_flag) {
        for (const auto& resOp : res) {
            std::cout << "BaseMind::operation sent {" << std::endl;
            debug_dump(resOp, std::cout);
            std::cout << "}" << std::endl << std::flush;
        }
    }
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
    LocatedEntity::setScript(scrpt);
    if (m_scripts.size() == 1) {
        m_map.setScript(m_scripts.front());
    }
}
