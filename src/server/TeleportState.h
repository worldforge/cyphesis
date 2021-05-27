// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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

#ifndef SERVER_TELEPORT_STATE_H
#define SERVER_TELEPORT_STATE_H

#include <chrono>
#include <string>

class TeleportState
{
    protected:
        /// \brief Specifies the possess key generated for this specific teleport
        std::string m_possessKey;
        enum
        {
            TELEPORT_NONE,      /// \brief Initial state
            TELEPORT_REQUESTED, /// \brief Teleport has been requested
            TELEPORT_CREATED    /// \brief Teleport has succeeded
        } m_state;

        std::chrono::steady_clock::time_point m_teleportTime;  /// \brief The time the teleport took place

    public:
        TeleportState(const TeleportState& rhs) = default;

        explicit TeleportState(std::chrono::steady_clock::time_point time);

        TeleportState& operator=(const TeleportState& rhs) = default;

        void setRequested();

        void setCreated();

        void setKey(const std::string& key);

        bool isCreated() const;

        bool isRequested() const;

        bool isMind() const;

        const std::string& getPossessKey() const;

        std::chrono::steady_clock::time_point getCreateTime() const;
};

/// \brief Check if the teleported entity has been create on the remote end
///
/// @return Boolean whether entity has been created or not
inline bool TeleportState::isCreated() const
{
    return (m_state == TELEPORT_CREATED);
}

/// \brief Check if the teleport state is 'currently requesting'
///
/// @return Whether the state is requesting or not
inline bool TeleportState::isRequested() const
{
    return (m_state == TELEPORT_REQUESTED);
}

/// \brief Check if a teleported entity has an external mind
///
/// @return Whether the teleported entity has an external mind
inline bool TeleportState::isMind() const
{
    return !m_possessKey.empty();
}

/// \brief Get the possess key generated for this teleport
///
/// @return The randomly generated possess key for this teleport
inline const std::string& TeleportState::getPossessKey() const
{
    return m_possessKey;
}

inline std::chrono::steady_clock::time_point TeleportState::getCreateTime() const
{
    return m_teleportTime;
}

#endif // SERVER_TELEPORT_STATE_H
