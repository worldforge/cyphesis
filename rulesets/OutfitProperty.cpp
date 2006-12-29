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

// $Id: OutfitProperty.cpp,v 1.2 2006-12-29 00:02:34 alriddoch Exp $

#include "OutfitProperty.h"

#include "Entity.h"

#include "common/debug.h"

#include <Atlas/Objects/RootEntity.h>

using Atlas::Message::MapType;

static const bool debug_flag = true;

OutfitProperty::OutfitProperty(unsigned int flags) : PropertyBase(flags)
{
}

bool OutfitProperty::get(Atlas::Message::Element & val) const
{
    val = MapType();
    MapType & val_map = val.Map();

    EntityRefMap::const_iterator I = m_data.begin();
    EntityRefMap::const_iterator Iend = m_data.end();
    for (; I != Iend; ++I) {
        const EntityRef & item = I->second;
        assert(item.get() != 0);
        val_map[I->first] = item->getId();
    }

    return true;
}

void OutfitProperty::set(const Atlas::Message::Element & val)
{
    // INT id?

    if (!val.isMap()) {
        debug(std::cout << "Value of outfit is not a map" << std::endl << std::flush;);
        return;
    }

    const MapType & val_map = val.Map();

    MapType::const_iterator I = val_map.begin();
    MapType::const_iterator Iend = val_map.end();

    for (; I != Iend; ++I) {
        const std::string & key = I->first;
        const Atlas::Message::Element & item = I->second;

        if (item.isString()) {
            const std::string & id = item.String();
            std::cout << "Assigning it to " << id << std::endl << std::flush;
            Entity * e = BaseWorld::instance().getEntity(id);
            if (e != 0) {
                std::cout << "Got it" << std::endl << std::flush;
                m_data[key] = EntityRef(e);
            }
        } else if (item.isPtr()) {
            std::cout << "Assigning it to pointer" << std::endl << std::flush;
            Entity * e = (Entity*)item.Ptr();
            assert(e != 0);
            m_data[key] = EntityRef(e);
        } else {
            debug(std::cout << "Key " << key << " is of type "
                            << item.getType() << " when setting outfit"
                            << std::endl << std::flush;);
        }
    }
}

void OutfitProperty::add(const std::string & key,
                         Atlas::Message::MapType & map) const
{
    if (m_data.empty()) {
        return;
    }

    MapType & val_map = (map[key] = MapType()).Map();

    EntityRefMap::const_iterator I = m_data.begin();
    EntityRefMap::const_iterator Iend = m_data.end();
    for (; I != Iend; ++I) {
        const EntityRef & item = I->second;
        assert(item.get() != 0);
        val_map[I->first] = item->getId();
    }
}

void OutfitProperty::add(const std::string & key,
                         const Atlas::Objects::Entity::RootEntity & ent) const
{
    if (m_data.empty()) {
        return;
    }

    MapType val_map;

    EntityRefMap::const_iterator I = m_data.begin();
    EntityRefMap::const_iterator Iend = m_data.end();
    for (; I != Iend; ++I) {
        const EntityRef & item = I->second;
        assert(item.get() != 0);
        val_map[I->first] = item->getId();
    }

    ent->setAttr(key, val_map);
}
