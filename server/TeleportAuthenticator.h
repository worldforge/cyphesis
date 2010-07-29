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

#ifndef SERVER_TELEPORT_AUTHENTICATOR_H
#define SERVER_TELEPORT_AUTHENTICATOR_H

#include "PendingTeleport.h"

#include <string>
#include <map>

class TeleportAuthenticator
{
    static TeleportAuthenticator * m_instance;
    std::map<std::string, PendingTeleport *> m_teleports;

    public:

    static void init() {
        if(m_instance == 0) {
            m_instance = new TeleportAuthenticator();
        }
    }
    static TeleportAuthenticator * instance() {
        return m_instance;
    }
    static void del() {
        if (m_instance != 0) {
            delete m_instance;
            m_instance = 0;
        }
    }

    /// \brief Checks if there is a pending teleport on an account
    ///
    /// \param account_id The account ID to check for pending teleports
    bool isPending(const std::string &);

    /// \brief Add a teleport authentication entry
    ///
    /// \param account_id The account ID that the teleport is being done by
    /// \param entity_id The ID of the entity that has been created
    /// \param key The possess key the client will use to authenticate himself
    int addTeleport(const std::string &, const std::string &, 
                                            const std::string &);

    /// \brief Authenticate a teleport request
    ///
    /// \param account_id The account ID that the teleport is being done by
    /// \param entity_id The ID of the entity that was created
    /// \param key The possess key sent by the client
    bool authenticateTeleport(const std::string &, const std::string &, 
                                                const std::string &);
};

#endif
