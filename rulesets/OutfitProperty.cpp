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

// $Id: OutfitProperty.cpp,v 1.5 2007-01-05 00:47:22 alriddoch Exp $

#include "OutfitProperty.h"

#include "Entity.h"

#include "common/debug.h"

#include <Atlas/Objects/RootEntity.h>

#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/mem_fun.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

static const bool debug_flag = true;

/// \brief OutfitProperty constructor
OutfitProperty::OutfitProperty() : PropertyBase(0)
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
        if (item.get() == 0) {
            val_map[I->first] = "";
        } else {
            val_map[I->first] = item->getId();
        }
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

    // FIXME This code is essentially shared with the add function below
    // Move it into a protected function
    EntityRefMap::const_iterator I = m_data.begin();
    EntityRefMap::const_iterator Iend = m_data.end();
    for (; I != Iend; ++I) {
        const EntityRef & item = I->second;
        if (item.get() != 0) {
            val_map[I->first] = item->getId();
        } else {
            val_map[I->first] = "";
        }
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
        if (item.get() != 0) {
            val_map[I->first] = item->getId();
        } else {
            val_map[I->first] = "";
        }
    }

    ent->setAttr(key, val_map);
}

void OutfitProperty::cleanUp()
{
    std::set<std::string> empty_locations;

    EntityRefMap::const_iterator I = m_data.begin();
    EntityRefMap::const_iterator Iend = m_data.end();
    for (; I != Iend; ++I) {
        if (I->second == 0) {
            empty_locations.insert(I->first);
        }
    }

    std::set<std::string>::const_iterator J = empty_locations.begin();
    std::set<std::string>::const_iterator Jend = empty_locations.end();
    for (; J != Jend; ++J) {
        std::cout << "Outfit purging " << *J << std::endl << std::flush;
        m_data.erase(*J);
    }
}

void OutfitProperty::wear(const std::string & location, Entity * garment)
{
    m_data[location] = EntityRef(garment);

    // FIXME #9 This don't seem to get removed when the entity owning
    // this property is deleted, even though this property is
    // trackable. Perhaps properties don't get deleted? Crazy talk.
    //
    // FIXME #10 We need to disconnect the containered signal when re
    // get triggered, thus removing it, otherwise the calls accumulate.
    garment->containered.connect(sigc::bind(sigc::mem_fun(this, &OutfitProperty::itemRemoved), garment));
    garment->destroyed.connect(sigc::bind(sigc::mem_fun(this, &OutfitProperty::itemRemoved), garment));
}

void OutfitProperty::itemRemoved(Entity * garment)
{
    Element worn_attr;
    std::string key;
    if (garment->getAttr("worn", worn_attr)) {
        if (worn_attr.isString()) {
            key = worn_attr.String();
            assert(!key.empty());
        } else {
            std::cout << "WORN CORRUPT" << std::endl << std::flush;
        }
    } else {
        std::cout << "WORN missing" << std::endl << std::flush;
        
        EntityRefMap::const_iterator I = m_data.begin();
        EntityRefMap::const_iterator Iend = m_data.end();
        for (; I != Iend; ++I) {
            if (I->second == garment) {
                std::cout << "Found in search" << std::endl << std::flush;
                key = I->first;
                assert(!key.empty());
            }
        }
    }
    if (key.empty()) {
        std::cout << "Removing gave up" << std::endl << std::flush;
    }
    std::cout << "Removing now" << std::endl << std::flush;
    m_data[key] = EntityRef(0);
    if (garment->isDestroyed()) {
        std::cout << "Removing destroyed" << std::endl << std::flush;
    }
}
