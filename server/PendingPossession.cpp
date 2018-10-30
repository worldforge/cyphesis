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

#include "PendingPossession.h"

/// \brief Cons
///
/// \param id The entity ID to associate a pending possession with
/// \param key The possess key used to authenticate an account as owner of the entity
PendingPossession::PendingPossession(const std::string &id, const std::string &key)
                                            :   m_entity_id(id),
                                                m_possess_key(key),
                                                m_valid(false)
{
}

/// \brief Validate a teleport based on the entity ID and its associated possess key
///
/// \param entity_id The ID of the entity whose ownership is to be claimed
/// \param possess_key The possess key sent for authentication
bool PendingPossession::validate(const std::string &entity_id,
                               const std::string &possess_key) const
{
    return m_entity_id == entity_id && m_possess_key == possess_key;
}

/// \brief Set this possession as validated
void PendingPossession::setValidated()
{
    m_valid = true;
}
