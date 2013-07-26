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

#ifndef SERVER_PENDING_POSSESSION_H
#define SERVER_PENDING_POSSESSION_H

#include <string>

class PendingPossession
{
    /// \brief The ID of the entity that is being possessed
    std::string m_entity_id;
    /// \brief The possess key that authenticates the client to the entity
    std::string m_possess_key;
    /// \brief Specifies if this object is valid
    bool m_valid;

  public:

    PendingPossession(const std::string &, const std::string &);
    const std::string & getPossessKey() const;
    const std::string & getEntityID() const;

    bool validate(const std::string &, const std::string &) const;

    void setValidated();
    bool isValidated() const;
};

/// \brief Get the possess key
///
/// \return The possess key string
inline const std::string & PendingPossession::getPossessKey() const
{
    return m_possess_key;
}

/// \brief Get the entity ID this teleport belongs to
///
/// \return The entity ID of the owner entity
inline const std::string & PendingPossession::getEntityID() const
{
    return m_entity_id;
}

/// \brief Check if this possession is validated or not
///
/// \return Boolean whether this possession is validated or not. A teleport is
///         considered validated if an entity was successfully created at the
///         destination.
inline bool PendingPossession::isValidated() const
{
    return m_valid;
}

#endif // SERVER_PENDING_TELEPORT_H
