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

#include "common/Link.h"
#include "common/Possess.h"
#include "common/log.h"
#include "common/compose.hpp"
#include "rulesets/LocatedEntity.h"

#include <Atlas/Objects/Entity.h>

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
    } else {
        log(INFO,
                String::compose(
                        "New external mind connection registered for router %1. "
                                "There are now %2 connections.",
                        connection.getRouterId(), m_connections.size()));
        return 0;
    }
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

int ExternalMindsManager::requestPossession(LocatedEntity& entity,
        const std::string& possession_key)
{
    if (!m_connections.empty()) {
        //Use the last one registered.
        //TODO: implement a better way to select the connection to use. Should we rotate the connections?
        //Or do some kind of selection?
        ExternalMindsConnection& connection = m_connections.rbegin()->second;

        Atlas::Objects::Operation::Possess possessOp;

        Atlas::Objects::Entity::Anonymous possess_args;
        possess_args->setAttr("possess_key", possession_key);
        possess_args->setAttr("possess_entity_id", entity.getId());

        possessOp->setArgs1(possess_args);
        possessOp->setTo(connection.getRouterId());

        log(INFO,
                String::compose(
                        "Requesting possession of mind for entity %1 from link with id %2 and router with id %3.",
                        entity.getId(), connection.getLink()->getId(),
                        connection.getRouterId()));
        connection.getLink()->send(possessOp);
        return 0;
    }
    return -1;
}

