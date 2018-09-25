/*
 Copyright (C) 2015 Erik Ogenvik

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

#ifdef HAVE_CONFIG_H
#endif

#include "PossessionAccount.h"
#include "PossessionClient.h"
#include "MindRegistry.h"

#include "rulesets/MindFactory.h"
#include "rulesets/BaseMind.h"
#include <rulesets/Python_API.h>

#include "common/Possess.h"
#include "common/id.h"
#include "common/custom.h"
#include "common/TypeNode.h"
#include "common/ScriptKit.h"
#include "common/Setup.h"
#include "common/debug.h"
#include <common/Think.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <common/Inheritance.h>


static const bool debug_flag = false;


using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Possess;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

PossessionAccount::PossessionAccount(const std::string& id, long intId, const MindKit& mindFactory, PossessionClient& client) :
    Router(id, intId),
    m_client(client),
    m_mindFactory(mindFactory)
{
    assert(m_mindFactory.m_scriptFactory);

    m_python_connection = python_reload_scripts.connect([&]() {
        for (auto& entry : m_minds) {
            auto entity = entry.second;
            //First store all thoughts
            Atlas::Objects::Operation::Think think;
            think->setArgs1(Atlas::Objects::Operation::Get());
            OpVector res;
            entity->operation(think, res);

            m_mindFactory.m_scriptFactory->addScript(entity.get());

            //After updating the script restore all thoughts
            OpVector ignoresRes;
            for (auto& op : res) {
                entity->operation(op, ignoresRes);
            }
        }

    });
}

PossessionAccount::~PossessionAccount()
{
    m_python_connection.disconnect();
}


void PossessionAccount::enablePossession(OpVector& res)
{

    Atlas::Objects::Operation::Set set;
    set->setTo(getId());
    set->setFrom(getId());

    Atlas::Objects::Entity::Anonymous args;
    args->setId(getId());
    args->setAttr("possessive", 1);
    args->setObjtype("object");

    set->setArgs1(args);

    res.push_back(set);
}

void PossessionAccount::operation(const Operation& op, OpVector& res)
{
    if (!op->isDefaultTo()) {
        auto I = m_minds.find(op->getTo());
        if (I != m_minds.end()) {
            I->second->operation(op, res);
            return;
        }

        I = m_entitiesWithMinds.find(op->getTo());
        if (I != m_entitiesWithMinds.end()) {
            I->second->operation(op, res);
            return;
        }

    }

    if (op->getClassNo() == Atlas::Objects::Operation::POSSESS_NO) {
        PossessOperation(op, res);
    } else if (op->getClassNo() == Atlas::Objects::Operation::APPEARANCE_NO) {
        //Ignore appearance ops, since they just signal other accounts being connected
    } else if (op->getClassNo() == Atlas::Objects::Operation::DISAPPEARANCE_NO) {
        //Ignore disappearance ops, since they just signal other accounts being disconnected
    } else if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
        //Send info ops on to all minds
        for (auto& entry : m_minds) {
            entry.second->operation(op, res);
        }
    } else {
        log(NOTICE, String::compose("Unknown operation %1 in PossessionAccount", op->getParent()));
    }
}

void PossessionAccount::externalOperation(const Operation& op, Link&)
{

}

void PossessionAccount::PossessOperation(const Operation& op, OpVector& res)
{
    debug(std::cout << "Got possession request." << std::endl;);

    auto args = op->getArgs();
    if (!args.empty()) {
        const Root& arg = args.front();

        Element possessKeyElement;
        if (arg->copyAttr("possess_key", possessKeyElement) == 0 && possessKeyElement.isString()) {
            Element possessionEntityIdElement;
            if (arg->copyAttr("possess_entity_id", possessionEntityIdElement) == 0 && possessionEntityIdElement.isString()) {

                const std::string& possessKey = possessKeyElement.String();
                const std::string& possessionEntityId = possessionEntityIdElement.String();
                takePossession(res, possessionEntityId, possessKey);
            }
        }
    }
}

void PossessionAccount::takePossession(OpVector& res, const std::string& possessEntityId, const std::string& possessKey)
{
    log(INFO, String::compose("Taking possession of entity with id %1.", possessEntityId));

    Anonymous what;
    what->setId(possessEntityId);
    what->setAttr("possess_key", possessKey);

    Possess possess;
    possess->setFrom(getId());
    possess->setArgs1(what);
    m_client.sendWithCallback(possess, [this](const Operation& op, OpVector& res) {

        if (op->getClassNo() != Atlas::Objects::Operation::INFO_NO) {
            log(ERROR, "Malformed possession response: not an info.");
        }

        const std::vector<Root>& args = op->getArgs();
        if (args.empty()) {
            log(ERROR, "no args character possession response");
            return;
        }

        RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(args.front());
        if (!ent.isValid()) {
            log(ERROR, "malformed character possession response");
            return;
        }

        if (!ent->hasAttr("entity")) {
            log(ERROR, "malformed character possession response");
            return;
        }
        auto entityElem = ent->getAttr("entity");
        if (!entityElem.isMap()) {
            log(ERROR, "malformed character possession response");
            return;
        }

        auto I = entityElem.Map().find("id");
        if (I == entityElem.Map().end() || !I->second.isString()) {
            log(ERROR, "malformed character possession response");
            return;
        }

        auto entityId = I->second.String();

        createMindInstance(res, ent->getId(), entityId);

    });


    //res.push_back(possess);
}

void PossessionAccount::createMindInstance(OpVector& res, const std::string& mindId, const std::string& entityId)
{
    log(INFO, String::compose("Creating mind instance for entity id %1 with mind id %2.", entityId, mindId));
    Ref<BaseMind> mind = m_mindFactory.newMind(mindId, entityId);
    m_minds.emplace(mindId, mind);
    m_entitiesWithMinds.emplace(entityId, mind);

    m_mindFactory.m_scriptFactory->addScript(mind.get());

    mind->init(res);
//
//
//
//
//    OpVector mindRes;
//
//    //Send the Sight operation we just got on to the mind, since it contains info about the entity.
//    mind->operation(op, mindRes);
//
//    //Also send a "Setup" op to the mind, which will trigger any setup hooks.
//    Atlas::Objects::Operation::Setup s;
//    Anonymous setup_arg;
//    setup_arg->setName("mind");
//    s->setTo(ent->getId());
//    s->setArgs1(setup_arg);
//    mind->operation(s, mindRes);
//
//    //Mark all resulting ops as coming from the mind.
//    for (auto& resOp : mindRes) {
//        resOp->setFrom(entityId);
//        res.push_back(resOp);
//    }
//
//    //Start by sending a unspecified "Look". This tells the server to send us a bootstrapped view.
//    Look l;
//    l->setFrom(entityId);
//    res.push_back(l);
}
