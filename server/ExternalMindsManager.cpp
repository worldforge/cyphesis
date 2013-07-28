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
void ExternalMindsManager::addConnection(
        const ExternalMindsConnection& connection)
{
    m_connections.push_back(connection);
    log(INFO, String::compose("New external mind connection registered. "
            "There are now %1 connections.", m_connections.size()));
}

int ExternalMindsManager::requestPossession(LocatedEntity& entity,
        const std::string& possession_key)
{
    if (!m_connections.empty()) {
        //Use the last one registered.
        ExternalMindsConnection& connection = *m_connections.rbegin();

        Atlas::Objects::Operation::Possess possessOp;

        Atlas::Objects::Entity::Anonymous possess_args;
        possess_args->setAttr("possess_key", possession_key);
        possess_args->setAttr("possess_entity_id", entity.getId());


        possessOp->setArgs1(possess_args);
//        possessOp->setTo(connection.getRouter()->getId());

//        OpVector res;
        log(INFO, "Requesting possession of mind.");
        connection.getLink()->send(possessOp);
        return 0;
    }
    return -1;
}

