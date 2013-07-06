// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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


#include "EntityProperty.h"

#include "LocatedEntity.h"

#include "common/BaseWorld.h"
#include "common/debug.h"

#include <Atlas/Objects/RootEntity.h>

#include <iostream>

static const bool debug_flag = false;

EntityProperty::EntityProperty()
{
}

int EntityProperty::get(Atlas::Message::Element & val) const
{
    if (m_data.get() != 0) {
        val = m_data->getId();
        return 0;
    } else {
        return -1;
    }
}

void EntityProperty::set(const Atlas::Message::Element & val)
{
    // INT id?
    if (val.isString()) {
        const std::string & id = val.String();
        if (m_data.get() == 0 || m_data->getId() != id) {
            debug(std::cout << "Assigning " << id << std::endl << std::flush;);
            if (id.empty()) {
                m_data = EntityRef(0);
            } else {
                LocatedEntity * e = BaseWorld::instance().getEntity(id);
                if (e != 0) {
                    debug(std::cout << "Assigned" << std::endl << std::flush;);
                    m_data = EntityRef(e);
                }
            }
        }
    } else if (val.isPtr()) {
        debug(std::cout << "Assigning pointer" << std::endl << std::flush;);
        LocatedEntity * e = static_cast<LocatedEntity*>(val.Ptr());
        m_data = EntityRef(e);
    }
}

void EntityProperty::add(const std::string & s,
                         Atlas::Message::MapType & map) const
{
    if (m_data.get() != 0) {
        map[s] = m_data->getId();
    } else {
        map[s] = "";
    }
}

void EntityProperty::add(const std::string & s,
                         const Atlas::Objects::Entity::RootEntity & ent) const
{
    if (m_data.get() != 0) {
        ent->setAttr(s, m_data->getId());
    } else {
        ent->setAttr(s, "");
    }
}

EntityProperty * EntityProperty::copy() const
{
    return new EntityProperty(*this);
}
