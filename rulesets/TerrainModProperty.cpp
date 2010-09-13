// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#include "TerrainModProperty.h"

#include "Entity.h"
#include "TerrainMod.h"
#include "TerrainProperty.h"

#include "common/compose.hpp"
#include "common/log.h"
#include "common/debug.h"

#include "modules/Location.h"

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

/// \brief TerrainModProperty constructor
///
TerrainModProperty::TerrainModProperty(const HandlerMap & handlers) :
                    m_modptr(0), m_handlers(handlers), m_innerMod(0)
{
}

TerrainModProperty::~TerrainModProperty()
{
	// TODO remove the mod from the terrain
    // This is already covered from the Delete op handler when
    // the entity is deleted
}

bool TerrainModProperty::get(Element & ent) const
{
	///NOTE: what does this do? /erik
    MapType & mod = (ent = MapType()).Map();
    mod = m_terrainmods;
    return true;
}

void TerrainModProperty::set(const Element & ent)
{
    if (ent.isMap()) {
        const MapType & mod = ent.Map();
        m_terrainmods = mod;
    }

}

// void TerrainModProperty::setPos(const Point3D & newPos)
// {
//     if (m_owner != NULL) {
// 
//         TerrainProperty * terr = NULL;
//         // Search for an entity with the terrain property
//         terr = getTerrain();
// 
//         if (terr != NULL) {
// 
//             // If we're updating an existing mod, remove it from the terrain first
//             if (m_modptr != NULL) {
//                 terr->removeMod(m_modptr);
//             }
// 
//             // Parse the Atlas data for our mod, using the new position
//             Mercator::TerrainMod *newMod = parseModData(m_terrainmods);
// 
//             if (newMod != NULL) {
//                 // Apply the new mod to the terrain; retain the returned pointer
//                 m_modptr = terr->setMod(newMod);
//             } else {
//                 m_modptr = NULL;
//                 log(ERROR, "Terrain Modifier could not be parsed!");
//             }
//         }
//     }
// }

const TerrainProperty * TerrainModProperty::getTerrain(Entity * owner)
{
    const PropertyBase * terr;
    LocatedEntity * ent = owner;

    while ( (terr = ent->getProperty("terrain")) == NULL) {
        ent = ent->m_location.m_loc;
        if (ent == NULL) {
            return NULL;
        }
    }

    const TerrainProperty * tp = dynamic_cast<const TerrainProperty*>(terr);
    return tp;
}

void TerrainModProperty::add(const std::string & s, MapType & ent) const
{
    get(ent[s]);
}

void TerrainModProperty::install(Entity * owner)
{
    HandlerMap::const_iterator I = m_handlers.begin();
    HandlerMap::const_iterator Iend = m_handlers.end();
    for (; I != Iend; ++I) {
        owner->installHandler(I->first, I->second);
    }
}

void TerrainModProperty::apply(Entity * owner)
{
    // Find the terrain
    const TerrainProperty * terr = NULL;
    terr = getTerrain(owner);

    if (terr == NULL) {
        log(ERROR, "Terrain Modifier could not find terrain");
        return;
    }

    // If we're updating an existing mod, remove it from the terrain first
    remove(owner);

    // Parse the Atlas data for our mod
    Mercator::TerrainMod *newMod = parseModData(owner, m_terrainmods);

    if (newMod != NULL) {
        // Apply the new mod to the terrain; retain the returned pointer
        m_modptr = terr->setMod(newMod);
    } else {
        m_modptr = NULL;
        log(ERROR, "Terrain Modifier could not be parsed!");
    }
}

void TerrainModProperty::move(Entity* owner, const Point3D & newPos)
{
    remove(owner);
    const TerrainProperty* terrain = getTerrain(owner);
    if (terrain) {
        Mercator::TerrainMod* modifier = parseModData(owner, m_terrainmods);
        if (modifier) {
            terrain->setMod(modifier);
        }
    }
    
}

void TerrainModProperty::remove(Entity * owner)
{
    if (m_modptr) {
        const TerrainProperty* terrain = getTerrain(owner);
        if (terrain) {
            terrain->removeMod(m_modptr);
        }
    }
    m_modptr = 0;
    if (m_innerMod) {
        delete m_innerMod;
        m_innerMod = 0;
    }
}

Mercator::TerrainMod * TerrainModProperty::parseModData(Entity * owner, const MapType & modMap)
{
    // Get modifier type
    Atlas::Message::MapType::const_iterator mod_I = modMap.find("type");
    if (mod_I != modMap.end()) {
        const Atlas::Message::Element& modTypeElem(mod_I->second);
        if (modTypeElem.isString()) {
            const std::string& modType = modTypeElem.asString();
    
            if (modType == "slopemod") {
                m_innerMod = new InnerTerrainModSlope(*this);
            } else if (modType == "levelmod") {
                m_innerMod = new InnerTerrainModLevel(*this);
            } else if (modType == "adjustmod") {
                m_innerMod = new InnerTerrainModAdjust(*this);
            } else  if (modType == "cratermod") {
                m_innerMod = new InnerTerrainModCrater(*this);
            }
        }
    }
    if (m_innerMod) {
        if (m_innerMod->parseAtlasData(owner, modMap)) {
            return m_innerMod->getModifier();
        }
    }

    return NULL;
}
