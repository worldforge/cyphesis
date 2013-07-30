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
    handleNet();
    for (auto& mind : m_minds) {
        mind.idle();
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
    if (op->getClassNo() == Atlas::Objects::Operation::POSSESS_NO) {
        PossessOperation(op, res);
    } else if (op->getClassNo() == Atlas::Objects::Operation::APPEARANCE_NO) {
        //Ignore appearance ops, since they just signal other accounts being connected
    } else if (op->getClassNo()
            == Atlas::Objects::Operation::DISAPPEARANCE_NO) {
        //Ignore disappearance ops, since they just signal other accounts being disconnected
    } else {
        log(NOTICE,
                String::compose("Unknown operation %1 in PossessionClient",
                        op->getParents().front()));
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
                m_minds.emplace_back(m_mindFactory);
                MindClient& mindClient = m_minds.back();
                log(INFO, "New mind created.");

                mindClient.connectLocal(client_socket_name);
                //TODO: allow for multiple logins per account
//                mindClient.login(m_username, m_password);

//For now we'll create a new system account per mind. We should really instead use the one account created by this client, but that requires changes to the server.
                mindClient.createSystemAccount(possessionEntityId);
                mindClient.takePossession(possessionEntityId, possessKey);
            }
        }
    }
}

