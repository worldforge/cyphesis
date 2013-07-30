/*
 Copyright (C) 2013 Erik Ogenvik

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
#include "config.h"
#endif

#include "MindClient.h"

#include "common/log.h"
#include "common/compose.hpp"
#include "common/debug.h"
#include "common/id.h"
#include "common/ScriptKit.h"
#include "common/Setup.h"
#include "common/TypeNode.h"
#include "common/Tick.h"

#include "rulesets/MindFactory.h"
#include "rulesets/BaseMind.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

MindClient::MindClient(MindFactory& mindFactory) :
        m_mindFactory(mindFactory), m_mind(0), m_nextTick(0)
{

}

MindClient::~MindClient()
{
}

void MindClient::idle()
{
    handleNet();

    //Send a Tick operation every second
    //TODO: make this better and more dynamic
    m_systemTime.update();
    if (m_systemTime.seconds() >= m_nextTick) {
        Atlas::Objects::Operation::Tick tick;

        OpVector res;
        operationToMind(tick, res);
        for (auto& resOp : res) {
            m_connection.send(resOp);
        }

        m_systemTime.update();
        m_nextTick = m_systemTime.seconds() + 1;
    }

}

void MindClient::takePossession(const std::string& possessEntityId,
        const std::string& possessKey)
{
    log(INFO,
            String::compose("Taking possession of entity with id %1.",
                    possessEntityId));

    m_entityId = possessEntityId;

    Anonymous what;
    what->setId(possessEntityId);
    what->setAttr("possess_key", possessKey);

    Look l;
    l->setFrom(m_playerId);
    l->setArgs1(what);
    OpVector res;
    if (m_connection.sendAndWaitReply(l, res) != 0) {
        std::cerr << "ERROR: Failed to take possession." << std::endl
                << std::flush;
    }
    Operation resOp = res.front();
    if (resOp->getClassNo() == Atlas::Objects::Operation::SIGHT_NO) {
        createMind(resOp);
    } else {
        log(ERROR,
                String::compose("Unrecognized response to possession: %1",
                        resOp->getParents().front()));
    }
}

Root MindClient::login(const std::string& username, const std::string& password)
{
    m_username = username;
    m_password = password;

    Anonymous player_ent;
    player_ent->setAttr("username", username);
    player_ent->setAttr("password", password);
    player_ent->setParents(std::list<std::string>(1, "sys"));

    debug(
            std::cout << "Logging " << username << " in with " << password
                    << " as password" << std::endl << std::flush
            ;);

    Login loginAccountOp;
    loginAccountOp->setArgs1(player_ent);
    OpVector res;
    if (m_connection.sendAndWaitReply(loginAccountOp, res) != 0) {
        std::cerr << "ERROR: Failed to log into server" << std::endl
                << std::flush;
        return Root(0);
    }

    const Root& ent = res.front()->getArgs().front();

    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "ERROR: Logged in, but account has no id" << std::endl
                << std::flush;
    } else {
        m_playerId = ent->getId();
    }

    return ent;

}

void MindClient::operationToMind(const Operation & op, OpVector & res)
{
    if (m_mind) {
        OpVector mindRes;
        m_mind->operation(op, mindRes);
        for (Operation& resOp : mindRes) {
            if (resOp->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
//Just filter out any tick operations for now. We'll have to decide to handle this better.
            } else {
                resOp->setFrom(m_mind->getId());
                res.push_back(resOp);
            }
        }
    }
}

void MindClient::operation(const Operation & op, OpVector & res)
{
    if (m_mind && op->getTo() == m_entityId) {
        operationToMind(op, res);
    } else {
        if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
//            InfoOperation(op, res);
        } else if (op->getClassNo() == Atlas::Objects::Operation::SIGHT_NO) {
        } else if (op->getClassNo() == Atlas::Objects::Operation::ERROR_NO) {
            //Handled (by printing to log) in BaseClient for now. Should we be able to listen for specific refno's?
        } else if (op->getClassNo() == Atlas::Objects::Operation::GET_NO
                && m_mind != nullptr) {
            //Send Get operations on to the mind, as they are used for thought inspection
            operationToMind(op, res);

            Atlas::Objects::Operation::Info info;
            std::vector<Root> newRes;
            //Why can't I do "info->setArgs(res)"?
            for (auto& operation : res) {
                newRes.push_back(operation);
            }
            info->setArgs(newRes);
            res.clear();
            res.push_back(info);
        } else {
            log(NOTICE,
                    String::compose("Unknown operation %1 in MindClient",
                            op->getParents().front()));
        }
    }
}

void MindClient::createMind(const Operation& op)
{

    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "no args character create/take response");
        return;
    }

    RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(
            args.front());
    if (!ent.isValid()) {
        log(ERROR, "malformed character create/take response");
        return;
    }

    std::string entityId = ent->getId();
    std::string entityType = ent->getParents().front();

    log(INFO,
            String::compose(
                    "Got info on account, creating mind for entity with id %1 of type %2.",
                    entityId, entityType));
    m_mind = m_mindFactory.newMind(entityId, integerId(entityId));
    //TODO: setup and get type from Inheritance
    m_mind->setType(new TypeNode(entityType));

    if (m_mindFactory.m_scriptFactory != 0) {
        log(INFO, "Adding script to entity.");
        m_mindFactory.m_scriptFactory->addScript(m_mind);
    }

    //Send the Sight operation we just got on to the mind, since it contains info about the entity.
    OpVector res;
    operationToMind(op, res);

    //Also send a "Setup" op to the mind, which will trigger any setup hooks.
    Atlas::Objects::Operation::Setup s;
    Anonymous setup_arg;
    setup_arg->setName("mind");
    s->setTo(ent->getId());
    s->setArgs1(setup_arg);
    operationToMind(s, res);

    for (auto& resOp : res) {
        m_connection.send(resOp);
    }

    //Start by sending a unspecified "Look". This tells the server to send us a bootstrapped view.
    Look l;
    l->setFrom(entityId);
    m_connection.send(l);

}

