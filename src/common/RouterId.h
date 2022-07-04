/*
 Copyright (C) 2022 Erik Ogenvik

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

#ifndef CYPHESIS_ROUTERID_H
#define CYPHESIS_ROUTERID_H

#include <string>

/**
 * Represents the id of a Router. We currently require a "long" identifier, even though the Atlas protocol only requires a string.
 * We therefore store both the int and string representation in this instance.
 * This allows us to revisit this setup if the need arises (for example if we want decentralized ids, such as GUIDs or similar).
 */
struct RouterId
{
    /**
     * Implicit ctor by design here.
     * @param intId
     */
    RouterId(long intId);
    explicit RouterId(std::string id);
    explicit RouterId(std::string id, long intId);

    RouterId(RouterId&&) = default;
    RouterId(const RouterId&) = default;

    /// \brief String identifier
    const std::string m_id;
    /// \brief Integer identifier
    const long m_intId;

    /**
     * Checks if the router id is valid, i.e. it's 0 or more.
     * @return
     */
    bool isValid() const {
        return m_intId >= 0;
    }
};

inline RouterId::RouterId(long intId)
    : m_id(std::to_string(intId))
    , m_intId(intId)
{
}

inline RouterId::RouterId(std::string id)
    : m_id(std::move(id))
    , m_intId(std::stol(m_id))
{
}

inline RouterId::RouterId(std::string id, long intId)
    : m_id(std::move(id))
    , m_intId(intId)
{
}
#endif  // CYPHESIS_ROUTERID_H
