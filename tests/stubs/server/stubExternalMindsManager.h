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

#include "server/ExternalMindsManager.h"

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
    return 0;

}

int ExternalMindsManager::removeConnection(const std::string& routerId)
{
    return 0;
}

void ExternalMindsManager::addPossessionEntryForCharacter(Character& character)
{
}

int ExternalMindsManager::requestPossession(Character& character, const std::string& language, const std::string& script)
{
    return 0;

}

int ExternalMindsManager::requestPossessionFromRegisteredClients(
        const std::string& entity_id)
{
    return 1;
}

void ExternalMindsManager::entity_destroyed(Character* entity)
{
}

void ExternalMindsManager::character_externalLinkChanged(Character* chr)
{
}

