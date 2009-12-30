// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

// $Id$

#include "SpawnEntity.h"

#include "rulesets/Entity.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

SpawnEntity::SpawnEntity(Entity * e, const MapType & data) : m_ent(e)
{
    MapType::const_iterator I = data.find("character_types");
    if (I != data.end() && I->second.isList()) {
        m_characterTypes = I->second.List();
    }
}

int SpawnEntity::spawnEntity(const std::string & type,
                             const RootEntity & dsc)
{
    if (m_ent.get() == 0) {
        return -1;
    }
    dsc->setLoc(m_ent->m_location.m_loc->getId());
    ::addToEntity(m_ent->m_location.pos(), dsc->modifyPos());
    ListType::const_iterator I = m_characterTypes.begin();
    ListType::const_iterator Iend = m_characterTypes.begin();
    for (; I != Iend; ++I) {
        if (*I == type) {
            return 0;
        }
    }
    return -1;
}

int SpawnEntity::populateEntity(Entity * ent,
                                const RootEntity & dsc,
                                OpVector & res)
{
    return 0;
}

int SpawnEntity::addToMessage(MapType & msg) const
{
    msg.insert(std::make_pair("character_types", m_characterTypes));
    return 0;
}
