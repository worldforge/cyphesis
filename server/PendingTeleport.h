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

#ifndef SERVER_PENDING_TELEPORT_H
#define SERVER_PENDING_TELEPORT_H

#include <string>

class PendingTeleport
{
    std::string m_entity_id;
    std::string m_possess_key;
    bool m_valid;

    public:

    PendingTeleport(const std::string &, const std::string &);
    const std::string & getPossessKey();
    const std::string & getEntityID();

    bool validate(const std::string &, const std::string &);

    bool setValidated();
    bool isValidated();
};

#endif
