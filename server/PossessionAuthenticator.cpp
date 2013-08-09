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

#include "PossessionAuthenticator.h"
#include "PendingPossession.h"

#include "common/BaseWorld.h"
#include "common/log.h"
#include "common/compose.hpp"

#include <cassert>

PossessionAuthenticator * PossessionAuthenticator::m_instance = nullptr;

/// \brief Checks if there is a pending possession on an account
///
/// \param entity_id The entity ID to check for pending teleport
bool PossessionAuthenticator::isPending(const std::string & entity_id) const
{
    PendingPossessionsMap::const_iterator I = m_possessions.find(entity_id);
    return (I != m_possessions.end());
}

/// \brief Add a possession authentication entry
///
/// \param entity_id The ID of the entity whose data is to be added
/// \param possess_key The possess key to authenticate the entity with
int PossessionAuthenticator::addPossession(const std::string & entity_id,
                                       const std::string & possess_key)
{
    if (isPending(entity_id)) {
        return -1;
    }
    PendingPossession * pt = new PendingPossession(entity_id, possess_key);
    if (pt == 0) {
        return -1;
    }
    m_possessions.insert(std::make_pair(entity_id, pt));
    log(INFO, String::compose("Added possession auth entry for %1,%2",
                              entity_id, possess_key));
    return 0;
}

/// \brief Remove a possession authentications entry.
///
/// \param entity_id The ID of the entity whose data is to be removed
int PossessionAuthenticator::removePossession(const std::string &entity_id)
{
    PendingPossessionsMap::iterator I = m_possessions.find(entity_id);
    if (I == m_possessions.end()) {
        log(ERROR, String::compose("No possession auth entry for entity ID %1",
                                                entity_id));
        return -1;
    }
    removePossession(I);
    log(ERROR, String::compose("Removed possession auth entry for entity ID %1",
                                                entity_id));
    return 0;
}

/// \brief Remove a possession authentications entry internals.
///
/// Typically after a successful authentication
/// \param I The iterator in m_possessions to be removed
void PossessionAuthenticator::removePossession(PendingPossessionsMap::iterator I)
{
    assert(I->second != 0);
    delete I->second;
    m_possessions.erase(I);
}

boost::optional<std::string> PossessionAuthenticator::getPossessionKey(const std::string& entity_id)
{
    auto result = m_possessions.find(entity_id);
    if (result != m_possessions.end()) {
        return boost::optional<std::string>(result->second->getPossessKey());
    }
    return boost::optional<std::string>();
}


/// \brief Authenticate a possession request
///
/// \param entity_id The ID of the entity that was created
/// \param possess_key The possess key sent by the client
LocatedEntity * PossessionAuthenticator::authenticatePossession(const std::string &entity_id,
                                            const std::string &possess_key)
{
    PendingPossessionsMap::iterator I = m_possessions.find(entity_id);
    if (I == m_possessions.end()) {
        log(ERROR, String::compose("Unable to find possessable entity with ID %1",
                                                                    entity_id));
        return nullptr;
    }
    PendingPossession *entry = I->second;
    assert(entry != 0);
    if (entry->validate(entity_id, possess_key)) {
        // We are authenticated!
        LocatedEntity * entity = BaseWorld::instance().getEntity(entity_id);
        if (entity == 0) {
            // This means the authentication entry itself is invalid. Remove it.
            log(ERROR, String::compose("Unable to find possessable entity with ID %1",
                                                                        entity_id));
            removePossession(I);
            return nullptr;
        }
        // Don't remove the entry yet. It will be removed after connecting 
        // the entity to the account in Account::LookOperation() successfully
        // removePossession(entity_id);
        return entity;
    }
    // We failed the authentication. Keep authentication entry for retries.
    return nullptr;
}
