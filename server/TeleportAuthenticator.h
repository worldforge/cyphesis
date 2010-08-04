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

#include "common/BaseWorld.h"

#include <string>
#include <map>

typedef std::map<std::string, PendingTeleport *> PendingTeleportMap;

class TeleportAuthenticator
{
    static TeleportAuthenticator * m_instance;
    PendingTeleportMap m_teleports;

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
    /// \param entity_id The entity ID to check for pending teleport
    bool isPending(const std::string &);

    /// \brief Add a teleport authentication entry
    ///
    /// \param entity_id The ID of the entity that has been created
    /// \param key The possess key the client will use to authenticate himself
    int addTeleport(const std::string &, const std::string &);

    /// \brief Remove a teleport authentications entry. Typically after a
    ///        successful authentication
    ///
    /// \param entity_id The ID of the entity whose data is to be removed
    int removeTeleport(const std::string &);

    /// \brief Authenticate a teleport request
    ///
    /// \param account The account to add the entity to
    /// \param entity_id The ID of the entity that was created
    /// \param key The possess key sent by the client
    Entity *authenticateTeleport(const std::string &, const std::string &);
};

#endif
