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

#include "PendingTeleport.h"

PendingTeleport::PendingTeleport(const std::string &id, const std::string &key) 
                                            :   m_entity_id(id),
                                                m_possess_key(key),
                                                m_valid(false)
{
}

const std::string & PendingTeleport::getPossessKey()
{
    return m_possess_key;
}

const std::string & PendingTeleport::getEntityID()
{
    return m_entity_id;
}

bool PendingTeleport::validate(const std::string &entity_id,
                                const std::string &possess_key)
{
    if(m_entity_id == entity_id && m_possess_key == possess_key) {
        return true;
    } else {
        return false;
    }
}

bool PendingTeleport::setValidated()
{
    m_valid = true;
}

bool PendingTeleport::isValidated()
{
    return m_valid;
}
