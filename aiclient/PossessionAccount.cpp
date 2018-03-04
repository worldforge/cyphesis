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
#include "LocatedEntityRegistry.h"

#include "rulesets/MindFactory.h"
#include "rulesets/BaseMind.h"

#include "common/Possess.h"
#include "common/id.h"
#include "common/custom.h"
#include "common/TypeNode.h"
#include "common/ScriptKit.h"
#include "common/Setup.h"
#include "common/debug.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>


static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

PossessionAccount::PossessionAccount(const std::string& id, long intId, LocatedEntityRegistry& locatedEntityRegistry, const MindKit& mindFactory) :
        Router(id, intId), mLocatedEntityRegistry(locatedEntityRegistry), m_mindFactory(mindFactory), m_serialNoCounter(1)
{
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

void PossessionAccount::operation(const Operation & op, OpVector & res)
{

    if (!op->isDefaultRefno() && m_possessionRefNumbers.find(op->getRefno()) != m_possessionRefNumbers.end()) {
        m_possessionRefNumbers.erase(op->getRefno());
        createMind(op, res);
    } else {

        if (op->getClassNo() == Atlas::Objects::Operation::POSSESS_NO) {
            PossessOperation(op, res);
        } else if (op->getClassNo() == Atlas::Objects::Operation::APPEARANCE_NO) {
            //Ignore appearance ops, since they just signal other accounts being connected
        } else if (op->getClassNo() == Atlas::Objects::Operation::DISAPPEARANCE_NO) {
            //Ignore disappearance ops, since they just signal other accounts being disconnected
        } else if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
            //Ignore info ops, since they just signal other accounts doing things
        } else {
            log(NOTICE, String::compose("Unknown operation %1 in PossessionAccount", op->getParent()));
        }
    }
}

void PossessionAccount::externalOperation(const Operation & op, Link &)
{

}

void PossessionAccount::PossessOperation(const Operation& op, OpVector & res)
{
    debug(std::cout << "Got possession request." << std::endl;);

    auto args = op->getArgs();
    if (!args.empty()) {
        const Root & arg = args.front();

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
    debug(std::cout << String::compose("Taking possession of entity with id %1.", possessEntityId) << std::endl;);

    Anonymous what;
    what->setId(possessEntityId);
    what->setAttr("possess_key", possessKey);

    Look l;
    l->setFrom(getId());
    l->setArgs1(what);
    l->setSerialno(m_serialNoCounter++);
    m_possessionRefNumbers.insert(l->getSerialno());
    res.push_back(l);
}

void PossessionAccount::createMind(const Operation & op, OpVector & res)
{

//    std::cout << "PossessionAccount::createMind {" << std::endl;
//    debug_dump(op, std::cout);
//    std::cout << "}" << std::endl << std::flush;

    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "no args character create/take response");
        return;
    }

    RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(args.front());
    if (!ent.isValid()) {
        log(ERROR, "malformed character create/take response");
        return;
    }

    if (ent->isDefaultParent()) {
        log(ERROR, "malformed character create/take response");
        return;
    }

    std::string entityId = ent->getId();
    std::string entityType = ent->getParent();

    debug(std::cout << String::compose("Got info on account, creating mind for entity with id %1 of type %2.", entityId, entityType) << std::endl;);
    log(INFO, String::compose("Creating mind for entity with id %1 of type '%2'. Name '%3'.", entityId, entityType, ent->getName()));
    BaseMind* mind = m_mindFactory.newMind(entityId, integerId(entityId));
    mLocatedEntityRegistry.addLocatedEntity(mind);
    //TODO: setup and get type from Inheritance
    mind->setType(new TypeNode(entityType));

    if (m_mindFactory.m_scriptFactory != nullptr) {
        debug(std::cout << "Adding script to entity." << std::endl;);
        m_mindFactory.m_scriptFactory->addScript(mind);
    }

    OpVector mindRes;

    //Send the Sight operation we just got on to the mind, since it contains info about the entity.
    mind->operation(op, mindRes);

    //Also send a "Setup" op to the mind, which will trigger any setup hooks.
    Atlas::Objects::Operation::Setup s;
    Anonymous setup_arg;
    setup_arg->setName("mind");
    s->setTo(ent->getId());
    s->setArgs1(setup_arg);
    mind->operation(s, mindRes);

    //Mark all resulting ops as coming from the mind.
    for (auto& resOp : mindRes) {
        resOp->setFrom(entityId);
        res.push_back(resOp);
    }

    //Start by sending a unspecified "Look". This tells the server to send us a bootstrapped view.
    Look l;
    l->setFrom(entityId);
    res.push_back(l);

}
