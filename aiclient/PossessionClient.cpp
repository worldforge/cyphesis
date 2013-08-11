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

#include "PossessionClient.h"

#include "common/Possess.h"
#include "common/log.h"
#include "common/compose.hpp"
#include "common/sockets.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::RootOperation;

PossessionClient::PossessionClient(MindFactory& mindFactory) :
        m_mindFactory(mindFactory)
{

}

PossessionClient::~PossessionClient()
{
}

void PossessionClient::idle()
{
    OpVector res;
    for (auto& mind : m_minds) {
        mind.second->idle(res);
    }

    for (auto& resOp : res) {
        m_connection.send(resOp);
    }
}

void PossessionClient::enablePossession()
{

    Atlas::Objects::Operation::Set set;
    set->setTo(m_playerId);
    set->setFrom(m_playerId);

    Atlas::Objects::Entity::Anonymous args;
    args->setId(m_playerId);
    args->setAttr("possessive", 1);
    args->setObjtype("object");

    set->setArgs1(args);

    m_connection.send(set);
}

void PossessionClient::operation(const Operation & op, OpVector & res)
{
    if (!op->isDefaultRefno()) {
        auto entry = m_refNoOperations.find(op->getRefno());
        if (entry != m_refNoOperations.end()) {
            //If there's a refno, alter the address of the operation
            op->setTo(entry->second);
            m_refNoOperations.erase(op->getRefno());
        }
    }

    if (op->getTo() == m_playerId || op->isDefaultTo()) {
        if (op->getClassNo() == Atlas::Objects::Operation::POSSESS_NO) {
            PossessOperation(op, res);
        } else if (op->getClassNo()
                == Atlas::Objects::Operation::APPEARANCE_NO) {
            //Ignore appearance ops, since they just signal other accounts being connected
        } else if (op->getClassNo()
                == Atlas::Objects::Operation::DISAPPEARANCE_NO) {
            //Ignore disappearance ops, since they just signal other accounts being disconnected
        } else {
            log(NOTICE,
                    String::compose("Unknown operation %1 in PossessionClient",
                            op->getParents().front()));
        }
    } else {
        auto mindI = m_minds.find(op->getTo());
        if (mindI != m_minds.end()) {
            mindI->second->operation(op, res);
            if (mindI->second->isMindDestroyed()) {
                log(INFO, "Removing AI mind as entity was deleted.");
                //The mind was destroyed as a result of the operation; we should remove it.
                m_minds.erase(mindI);
            }
        } else {
            log(ERROR, "Op sent to unrecognized address.");
        }
    }
}

void PossessionClient::PossessOperation(const Operation& op, OpVector & res)
{
    log(INFO, "Got possession request.");

    auto args = op->getArgs();
    if (!args.empty()) {
        const Root & arg = args.front();

        Element possessKeyElement;
        if (arg->copyAttr("possess_key", possessKeyElement) == 0
                && possessKeyElement.isString()) {
            Element possessionEntityIdElement;
            if (arg->copyAttr("possess_entity_id", possessionEntityIdElement)
                    == 0 && possessionEntityIdElement.isString()) {

                const std::string& possessKey = possessKeyElement.asString();
                const std::string& possessionEntityId =
                        possessionEntityIdElement.asString();
                m_minds.insert(
                        std::make_pair(possessionEntityId,
                                std::make_shared < MindClient
                                        > (m_mindFactory)));
                const auto& mindClient =
                        m_minds.find(possessionEntityId)->second;
                log(INFO, "New mind created.");

                OpVector newRes;
                mindClient->takePossession(newRes, m_connection, m_playerId,
                        possessionEntityId, possessKey);

                for (auto op : newRes) {
                    if (!op->isDefaultSerialno()) {
                        m_refNoOperations.insert(
                                std::make_pair(op->getSerialno(),
                                        possessionEntityId));
                    }
                    res.push_back(op);
                }
            }
        }
    }
}

