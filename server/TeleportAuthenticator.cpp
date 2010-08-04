// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "TeleportAuthenticator.h"
#include "PendingTeleport.h"

#include "common/log.h"
#include "common/compose.hpp"

#include <string>

TeleportAuthenticator * TeleportAuthenticator::m_instance = NULL;

bool TeleportAuthenticator::isPending(const std::string &entity_id)
{
    PendingTeleportMap::iterator I = m_teleports.find(entity_id);
    return (I != m_teleports.end());
}

int TeleportAuthenticator::addTeleport(const std::string &entity_id,
                                        const std::string &possess_key)
{
    if (isPending(entity_id)) {
        return -1;
    }
    m_teleports[entity_id] = new PendingTeleport(entity_id, possess_key);
    log(INFO, String::compose("Added teleport auth entry for %1,%2",
                                                entity_id,possess_key));
    return 0;
}

int TeleportAuthenticator::removeTeleport(const std::string &entity_id)
{
    if (!isPending(entity_id)) {
        return -1;
    }
    PendingTeleportMap::iterator I = m_teleports.find(entity_id);
    if (I == m_teleports.end()) {
        log(ERROR, String::compose("No teleport auth entry for entity ID %1",
                                                entity_id));
    }
    if (I->second) {
        delete I->second;
    }
    m_teleports.erase(I);
    log(ERROR, String::compose("Removed teleport auth entry for entity ID %1",
                                                entity_id));
    return 0;
}

Entity *TeleportAuthenticator::authenticateTeleport(const std::string &entity_id,
                                            const std::string &possess_key)
{
    if (!isPending(entity_id)) {
        return NULL;
    }
    PendingTeleportMap::iterator i = m_teleports.find(entity_id);
    PendingTeleport *entry = i->second;
    if (entry->validate(entity_id, possess_key)) {
        // We are authenticated!
        // entry->setValidated();
        Entity * entity = BaseWorld::instance().getEntity(entity_id);
        if (entity != 0) {
            log(ERROR, String::compose("Unable to find teleported entity with ID %s", 
                                                                        entity_id));
            removeTeleport(entity_id);
            return NULL;
        }
        removeTeleport(entity_id);
        return entity;
    }
    return NULL;
}
