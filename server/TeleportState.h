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

#ifndef SERVER_TELEPORTSTATE_H
#define SERVER_TELEPORTSTATE_H

#include <string>

#include <sys/time.h>

class TeleportState
{
  protected:
    /// \brief Specifies if the teleported entity has a mind or not
    bool m_isMind;
    /// \brief Specifies the possess key generated for this specific teleport
    std::string m_possessKey;
    enum {
        TELEPORT_NONE,      /// \brief Initial state
        TELEPORT_REQUESTED, /// \brief Teleport has been requested
        TELEPORT_CREATED    /// \brief Teleport has succeeded
    } m_state;

    time_t m_teleportTime;  /// \brief The time the teleport took place

  public:
    TeleportState(time_t time);
    
    void setRequested();
    void setCreated();
    void setKey(const std::string & key);

    bool isCreated();
    bool isRequested();

    bool isMind();
    const std::string & getPossessKey();

    time_t getCreateTime();
};

#endif
