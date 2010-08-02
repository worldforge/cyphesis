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

TeleportAuthenticator * TeleportAuthenticator::m_instance = NULL;

bool TeleportAuthenticator::isPending(const std::string &entity_id)
{
    std::map<std::string, PendingTeleport *>::iterator i = m_teleports.find(entity_id);
    return (i != m_teleports.end());
}

int TeleportAuthenticator::addTeleport(const std::string &entity_id,
                                        const std::string &possess_key)
{
    if (isPending(entity_id)) {
        return -1;
    }
    m_teleports[entity_id] = new PendingTeleport(entity_id, possess_key);
}

bool TeleportAuthenticator::authenticateTeleport(const std::string &entity_id,
                                            const std::string &possess_key)
{
    if (!isPending(entity_id)) {
        return false;
    }
    std::map<std::string, PendingTeleport *>::iterator i = m_teleports.find(entity_id);
    PendingTeleport *entry = i->second;
    if (entry->validate(entity_id, possess_key)) {
        // We are authenticated!
        entry->setValidated();
        return true;
    } else {
        return false;
    }
}
