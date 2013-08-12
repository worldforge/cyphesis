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

#include "ExternalMindsManager.h"
#include "PossessionAuthenticator.h"

#include "common/Link.h"
#include "common/Possess.h"
#include "common/log.h"
#include "common/compose.hpp"
#include "rulesets/Character.h"
#include "rulesets/ExternalMind.h"

#include <Atlas/Objects/Entity.h>

#include <wfmath/MersenneTwister.h>

#include <sigc++/bind.h>

ExternalMindsManager * ExternalMindsManager::m_instance = nullptr;

ExternalMindsManager::ExternalMindsManager()
{

}

ExternalMindsManager::~ExternalMindsManager()
{
}

ExternalMindsManager * ExternalMindsManager::instance()
{
    if (m_instance == NULL) {
        m_instance = new ExternalMindsManager();
    }
    return m_instance;
}

int ExternalMindsManager::addConnection(
        const ExternalMindsConnection& connection)
{
    auto result = m_connections.insert(
            std::make_pair(connection.getRouterId(), connection));
    if (!result.second) {
        log(WARNING,
                String::compose(
                        "Tried to register a external mind connection for "
                                "router %1 for which there's already a connection registered.",
                        connection.getRouterId()));
        return -1;
    }
    log(INFO,
            String::compose(
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
        log(INFO,
                String::compose(
                        "Deregisted external mind connection registered for router %1. "
                                "There are now %2 connections.", routerId,
                        m_connections.size()));
        return 0;
    }
}

void ExternalMindsManager::addPossessionEntryForCharacter(Character& character)
{
    std::string key = character.getId() + "_";
    WFMath::MTRand generator;
    for (int i = 0; i < 32; i++) {
        char ch = (char)((int)'a' + generator.rand(25));
        key += ch;
    }

    PossessionAuthenticator::instance()->addPossession(character.getId(), key);
}

int ExternalMindsManager::requestPossession(Character& character, const std::string& language, const std::string& script)
{
    //TODO: take preferred language and script into account
    addPossessionEntryForCharacter(character);
    character.destroyed.connect(
            sigc::bind(
                    sigc::mem_fun(*this,
                            &ExternalMindsManager::entity_destroyed),
                    &character));
    m_unpossessedEntities.insert(&character);

    character.externalLinkChanged.connect(
            sigc::bind(
                    sigc::mem_fun(*this,
                            &ExternalMindsManager::character_externalLinkChanged),
                    &character));

    if (!m_connections.empty()) {
        requestPossessionFromRegisteredClients(character.getId());
    }
    return 0;

}

int ExternalMindsManager::requestPossessionFromRegisteredClients(
        const std::string& entity_id)
{
    if (!m_connections.empty()) {
        auto result = PossessionAuthenticator::instance()->getPossessionKey(
                entity_id);
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

            log(INFO,
                    String::compose(
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

void ExternalMindsManager::entity_destroyed(Character* entity)
{
    m_unpossessedEntities.erase(entity);
    m_possessedEntities.erase(entity);
}

void ExternalMindsManager::character_externalLinkChanged(Character* chr)
{
    if (chr->m_externalMind == nullptr || !chr->m_externalMind->isLinked()) {
        //Make sure that the character is disconnected
        if (m_possessedEntities.find(chr) == m_possessedEntities.end()) {
            log(WARNING,
                    String::compose(
                            "Character %1 should be possessed, but isn't.",
                            chr->getId()));
            return;
        }
        m_possessedEntities.erase(chr);
        m_unpossessedEntities.insert(chr);

        //The possession entry was removed when the character was possessed last, so we need to add one back.
        addPossessionEntryForCharacter(*chr);

        //We'll now check for any registered possessive clients and ask them for possession of the newly unpossessed character.
        requestPossessionFromRegisteredClients(chr->getId());
    } else {
        //Make sure that the character is connected
        if (m_unpossessedEntities.find(chr) == m_unpossessedEntities.end()) {
            log(WARNING,
                    String::compose(
                            "Character %1 should be unpossessed, but isn't.",
                            chr->getId()));
            return;
        }
        m_unpossessedEntities.erase(chr);
        m_possessedEntities.insert(chr);

    }
}

