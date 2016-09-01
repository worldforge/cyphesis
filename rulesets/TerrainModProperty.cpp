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


#include "TerrainModProperty.h"

#include "LocatedEntity.h"
#include "TerrainModTranslator.h"
#include "TerrainProperty.h"
#include "Domain.h"

#include "common/compose.hpp"
#include "common/log.h"
#include "common/debug.h"

#include "modules/Location.h"
#include "modules/TerrainContext.h"

#include <Mercator/TerrainMod.h>

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Root;

/// \brief TerrainModProperty constructor
///
TerrainModProperty::TerrainModProperty() : m_modptr(0), m_innerMod(0)
{
}

TerrainModProperty::~TerrainModProperty()
{
    // TODO remove the mod from the terrain
    // This is already covered from the Delete op handler when
    // the entity is deleted
    delete m_innerMod;
}

TerrainModProperty * TerrainModProperty::copy() const
{
    // This is for instantiation of a class property.
    // This is complex here, as is it not yet clear if this
    // class can be a class property.
    return new TerrainModProperty(*this);
}

void TerrainModProperty::install(LocatedEntity * owner, const std::string & name)
{
    owner->installDelegate(Atlas::Objects::Operation::DELETE_NO, name);
    owner->installDelegate(Atlas::Objects::Operation::MOVE_NO, name);
}

void TerrainModProperty::remove(LocatedEntity *owner, const std::string & name)
{
    owner->removeDelegate(Atlas::Objects::Operation::DELETE_NO, name);
    owner->removeDelegate(Atlas::Objects::Operation::MOVE_NO, name);
}


void TerrainModProperty::apply(LocatedEntity * owner)
{
    if (!owner->m_location.pos().isValid()) {
        log(ERROR, "Terrain Modifier applied to entity with no valid position. " + owner->describeEntity());
        return;
    }

    // Find the terrain
    LocatedEntity* terrainHolder;
    const TerrainProperty * terrain = getTerrain(owner, &terrainHolder);

    if (terrain == 0) {
        log(ERROR, "Terrain Modifier could not find terrain");
        return;
    }

    std::vector<WFMath::AxisBox<2>> terrainAreas;

    if (m_modptr) {
        terrainAreas.push_back(m_modptr->bbox());
    }

    // Parse the Atlas data for our mod
    Mercator::TerrainMod * mod = parseModData(owner, m_data);

    if (mod == 0) {
        log(ERROR, "Terrain Modifier could not be parsed!");
        return;
    }

    terrainAreas.push_back(mod->bbox());

    // If there is an old mod ...
    if (m_modptr != 0) {
        // and the new one is the same, just update
        if (mod == m_modptr) {
            terrain->updateMod(m_modptr);
            if (terrainHolder->getMovementDomain()) {
                terrainHolder->getMovementDomain()->refreshTerrain(terrainAreas);
            }
            return;
        }
        // If the mod has changed then remove the old one and delete it.
        terrain->removeMod(m_modptr);
        delete m_modptr;
    }

    m_modptr = mod;

    // Apply the new mod to the terrain; retain the returned pointer
    terrain->addMod(m_modptr);
    m_modptr->setContext(new TerrainContext(owner));
    m_modptr->context()->setId(owner->getId());
    if (terrainHolder->getMovementDomain()) {
        terrainHolder->getMovementDomain()->refreshTerrain(terrainAreas);
    }
}

HandlerResult TerrainModProperty::operation(LocatedEntity * ent,
                                            const Operation & op,
                                            OpVector & res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::DELETE_NO) {
        return delete_handler(ent, op, res);
    } else if (op->getClassNo() == Atlas::Objects::Operation::MOVE_NO) {
        return move_handler(ent, op, res);
    }
    return OPERATION_IGNORED;
}

void TerrainModProperty::move(LocatedEntity* owner)
{
    LocatedEntity* terrainHolder;
    const TerrainProperty * terrain = getTerrain(owner, &terrainHolder);

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
    if (terrainHolder->getMovementDomain()) {
        terrainHolder->getMovementDomain()->refreshTerrain(std::vector<WFMath::AxisBox<2>>{mod->bbox()});
    }

}

void TerrainModProperty::remove(LocatedEntity * owner)
{
    if (m_modptr) {
        LocatedEntity* terrainHolder;
        const TerrainProperty * terrain = getTerrain(owner, &terrainHolder);
        if (terrain) {
            std::vector<WFMath::AxisBox<2>> terrainAreas;
            terrainAreas.push_back(m_modptr->bbox());
            terrain->removeMod(m_modptr);
            delete m_modptr;
            m_modptr = nullptr;

            if (terrainHolder->getMovementDomain()) {
                terrainHolder->getMovementDomain()->refreshTerrain(terrainAreas);
            }

        } else {
            log(WARNING, "Terrain property was removed from an entity from which no terrain was available.");
        }
    }
}

Mercator::TerrainMod * TerrainModProperty::parseModData(LocatedEntity * owner,
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

HandlerResult TerrainModProperty::move_handler(LocatedEntity * e,
                                               const Operation & op,
                                               OpVector & res)
{
    // FIXME Force instantiation of a class property?

    // Check the validity of the operation.
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return OPERATION_IGNORED;
    }
    RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(args.front());
    if (!ent.isValid()) {
        return OPERATION_IGNORED;
    }
    if (e->getId() != ent->getId()) {
        return OPERATION_IGNORED;
    }

    // Update the modifier
    move(e);
    return OPERATION_IGNORED;
}

HandlerResult TerrainModProperty::delete_handler(LocatedEntity * e,
                                                 const Operation & op,
                                                 OpVector & res)
{
    remove(e);

    return OPERATION_IGNORED;
}

