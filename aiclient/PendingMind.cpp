#include <utility>

#include <utility>

/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "PendingMind.h"
#include "MindRegistry.h"
#include "common/log.h"
#include "common/Inheritance.h"
#include "common/debug.h"
#include "common/id.h"
#include "common/ScriptKit.h"
#include "common/Setup.h"

#include "rulesets/MindFactory.h"
#include "rulesets/BaseMind.h"
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

PendingMind::PendingMind(std::string entityId, std::string mindId, MindRegistry& locatedEntityRegistry, const MindKit& mindFactory)
    : m_entityId(std::move(entityId)),
      m_mindId(std::move(mindId)),
      m_mindRegistry(locatedEntityRegistry),
      m_mindFactory(mindFactory),
      m_serialNoCounter(0),
      m_typeResolver(new TypeResolver())
{
    m_typeResolver->m_typeProviderId = m_mindId;
}

void PendingMind::init(OpVector& res)
{
    Look look;
    Root lookArg;
    lookArg->setId(m_entityId);
    look->setArgs1(lookArg);
    look->setFrom(m_mindId);
    look->setSerialno(++m_serialNoCounter);
    res.push_back(look);
}

void PendingMind::operation(const Operation& op, OpVector& res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
        auto resolvedTypes = m_typeResolver->InfoOperation(op, res);
        for (auto& type : resolvedTypes) {
            if (type->name() == m_typeName) {
                //Got the type of the entity, we can continue with creation
                createEntity(res, type);
            }
        }
    } else {
        if (op->isDefaultRefno() || op->getRefno() != m_serialNoCounter) {
            m_pendingOperations.emplace_back(op);
        } else {
            m_pendingOperations.push_back(op);
            if (op->getClassNo() != Atlas::Objects::Operation::SIGHT_NO || op->getArgs().empty()) {
                log(ERROR, "Malformed initial sight when setting up mind.");
                return;
            }

            auto ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(op->getArgs().front());

            if (!ent || ent->isDefaultParent()) {
                log(ERROR, "Malformed initial sight when setting up mind.");
                return;
            }

            m_typeName = ent->getParent();

            auto typeNode = Inheritance::instance().getType(m_typeName);

            if (!typeNode) {
                m_typeResolver->requestType(m_typeName, res);
            } else {
                createEntity(res, typeNode);
            }
        }
    }
}

void PendingMind::createEntity(OpVector& res, const TypeNode* type)
{
    debug(std::cout << String::compose("Got info on account, creating mind for entity with id %1 of type %2.", m_entityId, type->name()) << std::endl;);
    log(INFO, String::compose("Creating mind for entity with id %1 of type '%2'. Name '%3'. Mind id: %4", m_entityId, type->name(), m_entitySight->getName(), m_mindId));
    Ref<BaseMind> mind = m_mindFactory.newMind(m_entityId, integerId(m_entityId));
    mind->setMindId(m_mindId);
    mind->setType(type);
    //Move the type resolver from us to the mind.
    mind->setTypeResolver(std::move(m_typeResolver));
    mind->getMap()->addEntity(mind);

    m_mindRegistry.addLocatedEntity(mind);

    if (m_mindFactory.m_scriptFactory != nullptr) {
        debug(std::cout << "Adding script to entity." << std::endl;);
        m_mindFactory.m_scriptFactory->addScript(mind.get());
    }

    OpVector mindRes;

    //Send the Sight operation we just got on to the mind, since it contains info about the entity.
    mind->operation(m_entitySight, mindRes);

    //Also send a "Setup" op to the mind, which will trigger any setup hooks.
    Atlas::Objects::Operation::Setup s;
    Anonymous setup_arg;
    setup_arg->setName("mind");
    s->setTo(m_mindId);
    s->setArgs1(setup_arg);
    mind->operation(s, mindRes);

    //Mark all resulting ops as coming from the mind.
    for (auto& resOp : mindRes) {
        resOp->setFrom(m_mindId);
        res.push_back(resOp);
    }

    //Start by sending a unspecified "Look". This tells the server to send us a bootstrapped view.
    Look l;
    l->setFrom(m_mindId);
    res.push_back(l);

    if (!m_pendingOperations.empty()) {
        log(INFO, String::compose("Sending %1 pending operations to the mind.", m_pendingOperations.size()));
        for (const auto& op : m_pendingOperations) {
            mind->operation(op, res);
        }
        m_pendingOperations.clear();
    }

    m_mindRegistry.removePendingMind(m_mindId);
}
