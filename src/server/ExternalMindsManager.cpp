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
#endif

#include "ExternalMindsManager.h"
#include "PossessionAuthenticator.h"

#include "common/Link.h"
#include "common/operations/Possess.h"
#include "common/log.h"
#include "common/compose.hpp"
#include "common/debug.h"
#include "rules/LocatedEntity.h"

#include <Atlas/Objects/Entity.h>

#include <wfmath/MersenneTwister.h>

#include <rules/simulation/MindsProperty.h>

#include <iostream>

static const bool debug_flag = false;

ExternalMindsManager::ExternalMindsManager(PossessionAuthenticator& possessionAuthenticator)
        : m_possessionAuthenticator(possessionAuthenticator)
{}


int ExternalMindsManager::addConnection(
        const ExternalMindsConnection& connection)
{
    auto result = m_connections.emplace(connection.getRouterId(), connection);
    if (!result.second) {
        log(WARNING, String::compose(
                "Tried to register a external mind connection for "
                "router %1 for which there's already a connection registered.",
                connection.getRouterId()));
        return -1;
    }
    log(INFO, String::compose(
            "New external mind connection registered for router %1. "
            "There are now %2 connections.",
            connection.getRouterId(), m_connections.size()));

    //As we now have a new connection we'll see if there are any minds in waiting

    for (auto character : m_unpossessedEntities) {
        requestPossessionFromRegisteredClients(character->getId());
    }

    return 0;

}

int ExternalMindsManager::removeConnection(const std::string& routerId)
{
    auto result = m_connections.erase(routerId);
    if (result == 0) {
        log(WARNING,
            String::compose(
                    "Tried to deregister a external mind connection for "
                    "router %1 for which there's no connection registered.",
                    routerId));
        return -1;
    } else {
        log(INFO, String::compose(
                "Deregistered external mind connection registered for router %1. "
                "There are now %2 connections.", routerId,
                m_connections.size()));
        return 0;
    }
}

void ExternalMindsManager::addPossessionEntryForCharacter(LocatedEntity& entity)
{
    std::string key = entity.getId() + "_";
    WFMath::MTRand generator;
    for (int i = 0; i < 32; i++) {
        char ch = (char) ((int) 'a' + generator.rand(25));
        key += ch;
    }

    m_possessionAuthenticator.addPossession(entity.getId(), key);
}

int ExternalMindsManager::requestPossession(LocatedEntity& entity)
{
    addPossessionEntryForCharacter(entity);
    entity.destroyed.connect([this, &entity]() { entity_destroyed(entity); });
    m_unpossessedEntities.insert(&entity);

    entity.propertyApplied.connect([this, &entity](const std::string& propName, const PropertyBase& prop) {
        if (propName == MindsProperty::property_name) {
            auto changedMindsProp = dynamic_cast<const MindsProperty*>(&prop);
            if (changedMindsProp) {
                entity_mindsChanged(entity, *changedMindsProp);
            }
        }
    });

    auto mindsProperty = entity.getPropertyClassFixed<MindsProperty>();
    if (!mindsProperty || mindsProperty->getMinds().empty()) {
        requestPossessionFromRegisteredClients(entity.getId());
    }
    return 0;
}

void ExternalMindsManager::removeRequest(LocatedEntity& character)
{
    m_unpossessedEntities.erase(&character);
}


int ExternalMindsManager::requestPossessionFromRegisteredClients(const std::string& entity_id)
{
    if (!m_connections.empty()) {
        auto result = m_possessionAuthenticator.getPossessionKey(entity_id);
        if (result.is_initialized()) {
            //Use the last one registered.
            //TODO: implement a better way to select the connection to use. Should we rotate the connections?
            //Or do some kind of selection?
            ExternalMindsConnection& connection = m_connections.rbegin()->second;

            Atlas::Objects::Operation::Possess possessOp;

            Atlas::Objects::Entity::Anonymous possess_args;
            possess_args->setAttr("possess_key", *result);
            possess_args->setAttr("possess_entity_id", entity_id);

            possessOp->setArgs1(possess_args);
            possessOp->setTo(connection.getRouterId());

            debug_print(String::compose(
                    "Requesting possession of mind for entity %1 from link with id %2 and router with id %3.",
                    entity_id, connection.getLink()->getId(),
                    connection.getRouterId()));

            connection.getLink()->send(possessOp);
            return 0;
        }
        return -1;
    }

    return 1;
}

void ExternalMindsManager::entity_destroyed(LocatedEntity& entity)
{
    m_unpossessedEntities.erase(&entity);
    m_possessedEntities.erase(&entity);
}

void ExternalMindsManager::entity_mindsChanged(LocatedEntity& entity, const MindsProperty& mindsProp)
{
    //If there are no minds controlling the entity we should try to possess it
    if (mindsProp.getMinds().empty()) {
        m_possessedEntities.erase(&entity);
        m_unpossessedEntities.insert(&entity);

        //The possession entry was removed when the character was possessed last, so we need to add one back.
        addPossessionEntryForCharacter(entity);

        //We'll now check for any registered possessive clients and ask them for possession of the newly unpossessed character.
        requestPossessionFromRegisteredClients(entity.getId());
    } else {
        //Mark that the entity now is possessed (although it might not be by us).
        m_unpossessedEntities.erase(&entity);
        m_possessedEntities.insert(&entity);

    }
}

