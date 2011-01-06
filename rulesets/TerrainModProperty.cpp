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
#include "TerrainModTranslator.h"
#include "TerrainProperty.h"

#include "common/compose.hpp"
#include "common/log.h"
#include "common/debug.h"

#include "modules/Location.h"
#include "modules/TerrainContext.h"

#include <Mercator/TerrainMod.h>

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
    delete m_innerMod;
}

bool TerrainModProperty::get(Element & ent) const
{
    MapType & mod = (ent = MapType()).Map();
    mod = m_data;
    return true;
}

void TerrainModProperty::set(const Element & ent)
{
    if (ent.isMap()) {
        const MapType & mod = ent.Map();
        m_data = mod;
    }

}

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
    const TerrainProperty * terrain = getTerrain(owner);

    if (terrain == 0) {
        log(ERROR, "Terrain Modifier could not find terrain");
        return;
    }

    // Parse the Atlas data for our mod
    Mercator::TerrainMod * mod = parseModData(owner, m_data);

    if (mod == 0) {
        log(ERROR, "Terrain Modifier could not be parsed!");
        return;
    }

    // If there is an old mod ...
    if (m_modptr != 0) {
        // and the new one is the same, just update
        if (mod == m_modptr) {
            terrain->updateMod(m_modptr);
            return;
        }
        // If the mod has changed then remove the old one and dlete it.
        terrain->removeMod(m_modptr);
        delete m_modptr;
        m_modptr = mod;
    }
    // Apply the new mod to the terrain; retain the returned pointer
    terrain->addMod(m_modptr);
    m_modptr->setContext(new TerrainContext(owner));
    m_modptr->context()->setId(owner->getId());
}

void TerrainModProperty::move(Entity* owner)
{
    const TerrainProperty* terrain = getTerrain(owner);

    if (terrain == 0) {
        log(ERROR, "Terrain Modifier could not find terrain");
        return;
    }

    Mercator::TerrainMod* mod = parseModData(owner, m_data);

    if (mod == 0) {
        log(ERROR, "Terrain Modifier could not be parsed!");
        return;
    }

    if (mod != m_modptr) {
        log(ERROR, "Terrain Modifier mysteriously changed when moved!");
        return;
    }
    terrain->updateMod(mod);
}

void TerrainModProperty::remove(Entity * owner)
{
    if (m_modptr) {
        const TerrainProperty* terrain = getTerrain(owner);
        if (terrain) {
            terrain->removeMod(m_modptr);
            delete m_modptr;
            m_modptr = 0;
        }
    }
}

Mercator::TerrainMod * TerrainModProperty::parseModData(Entity * owner,
                                                        const MapType & modMap)
{
    if (m_innerMod == 0) {
        m_innerMod = new TerrainModTranslator;
    }

    if (m_innerMod->parseData(owner->m_location.pos(),
                              owner->m_location.orientation(), modMap)) {
        return m_innerMod->getModifier();
    }

    return 0;
}

int TerrainModProperty::getAttr(const std::string & name,
                                Element & val)
{
    MapType::const_iterator I = m_data.find(name);
    if (I != m_data.end()) {
        val = I->second;
        return 0;
    }
    return -1;
}

void TerrainModProperty::setAttr(const std::string & name,
                                 const Element & val)
{
    m_data[name] = val;
}

