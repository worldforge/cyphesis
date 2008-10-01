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

#include "World.h"

#include "common/log.h"
#include "common/debug.h"

#include "modules/Location.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TerrainMod.h>

#include "TerrainProperty.h"

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

typedef Mercator::Terrain::Pointstore Pointstore;
typedef Mercator::Terrain::Pointcolumn Pointcolumn;

/// \brief TerrainModProperty constructor
///
TerrainModProperty::TerrainModProperty(const HandlerMap & handlers) :
                    PropertyBase(0),
                    m_modptr(0), m_owner(0), m_handlers(handlers)
{
}

bool TerrainModProperty::get(Element & ent) const
{
    MapType & mod = (ent = MapType()).Map();
    mod = m_terrainmods;
    return true;
}

Mercator::TerrainMod * TerrainModProperty::getModifier()
{
    return m_modptr;
}

void TerrainModProperty::set(const Element & ent)
{
    if (ent.isMap()) {
        const MapType & mod = ent.Map();
        m_terrainmods = mod;
    }

    if (m_owner != NULL) {

        // Find the terrain
        TerrainProperty * terr = NULL;
        terr = getTerrain();

        if (terr != NULL) {

            // If we're updating an existing mod, remove it from the terrain first
            if (m_modptr != NULL) {
                terr->removeMod(m_modptr);
            }

            // Parse the Atlas data for our mod
            Mercator::TerrainMod *newMod = parseModData(ent);

            if (newMod != NULL) {
                // Apply the new mod to the terrain; retain the returned pointer
                m_modptr = terr->setMod(newMod);
            } else {
                m_modptr = NULL;
                log(ERROR, "Terrain Modifier could not be parsed!");
            }
        }
    }
}

void TerrainModProperty::setPos(const Point3D & newPos)
{
    if (m_owner != NULL) {

        TerrainProperty * terr = NULL;
        // Search for an entity with the terrain property
        terr = getTerrain();

        if (terr != NULL) {

            // If we're updating an existing mod, remove it from the terrain first
            if (m_modptr != NULL) {
                terr->removeMod(m_modptr);
            }

            // Parse the Atlas data for our mod, using the new position
            Mercator::TerrainMod *newMod = parseModData(m_terrainmods, newPos);

            if (newMod != NULL) {
                // Apply the new mod to the terrain; retain the returned pointer
                m_modptr = terr->setMod(newMod);
            } else {
                m_modptr = NULL;
                log(ERROR, "Terrain Modifier could not be parsed!");
            }
        }
    }
}

TerrainProperty* TerrainModProperty::getTerrain()
{
    PropertyBase * terr;
    Entity * ent = m_owner;

    while ( (terr = ent->modProperty("terrain")) == NULL) {
        ent = (Entity*)(ent->m_location.m_loc);
        if (ent == NULL) {
            return NULL;
        }
    }

    return dynamic_cast<TerrainProperty*>(terr);
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

    // FIXME Don't do this!
    m_owner = owner;
}

void TerrainModProperty::move(Entity* owner, const Point3D & newPos)
{
    // Get terrain
    TerrainProperty * terrain = NULL;
    terrain = getTerrain();

    if (terrain != NULL) {
        // Clear the mod from the old position
        terrain->removeMod(m_modptr);

        // Apply the mod at the new position
        setPos(newPos);
    }
}

void TerrainModProperty::remove()
{
    TerrainProperty * terrain = NULL;
    terrain = getTerrain();

    if (terrain != NULL) {
        terrain->removeMod(m_modptr);
    }
}

Mercator::TerrainMod * TerrainModProperty::parseModData(const Element & modifier)
{
    if (!modifier.isMap()) {
        log(ERROR, "Invalid terrain mod data");
        return NULL;
    }

    const Atlas::Message::MapType & modMap = modifier.asMap();
    m_terrainmods = modMap;
    std::string modType;

    Atlas::Message::MapType shapeMap;
    Atlas::Message::MapType::const_iterator mod_I;

    // Get modifier type
    mod_I = modMap.find("type");
    if (mod_I != modMap.end()) {
        const Atlas::Message::Element& modTypeElem(mod_I->second);
        if (modTypeElem.isString()) {
            modType = modTypeElem.asString();
        }
    }

    // Get modifier position
    Point3D pos = m_owner->m_location.pos();


    // Get modifier's shape data
    mod_I = modMap.find("shape");
    if (mod_I != modMap.end()) {
        const Atlas::Message::Element& shapeElem(mod_I->second);
        if (shapeElem.isMap()) {
            shapeMap = shapeElem.asMap();
        } 
    }

    // Build modifier from data
    if (modType == "slopemod") {
        float dx = 0, dy = 0, level = 0;
        // Get slopes
        mod_I = modMap.find("slopes");
        if (mod_I != modMap.end()) {
            const Atlas::Message::Element& modSlopeElem = mod_I->second;
            if (modSlopeElem.isList()) {
                const Atlas::Message::ListType & slopes = modSlopeElem.asList();
                dx = (int)slopes[0].asNum();
                dy = (int)slopes[1].asNum();
            }
        }
        // Get level
        mod_I = modMap.find("height");
        if (mod_I != modMap.end()) {
            const Atlas::Message::Element& modHeightElem = mod_I->second;
            level = modHeightElem.asNum();
        }

        pos.z() = level;        // Note that the height of the mod is in pos.z()
        return newSlopeMod(shapeMap, pos, dx, dy);
    }

    if (modType == "levelmod") {
        float level = 0;
        // Get level
        mod_I = modMap.find("height");
        if (mod_I != modMap.end()) {
            const Atlas::Message::Element& modHeightElem = mod_I->second;
            level = modHeightElem.asNum();
        }

        pos.z() = level;        // Note that the level the terrain will be raised to is in pos.z()
        return newLevelMod(shapeMap, pos);
    }

    if (modType == "adjustmod") {
        float level = 0;
        // Get level
        mod_I = modMap.find("height");
        if (mod_I != modMap.end()) {
            const Atlas::Message::Element& modHeightElem = mod_I->second;
            level = modHeightElem.asNum();
        }

        pos.z() = level;        // Note that the level used in the adjustment is in pos.z()
        return newAdjustMod(shapeMap, pos);
    }
    //log(INFO,"Successfully parsed an adjustmod");

    if (modType == "cratermod") {

        return newCraterMod(shapeMap, pos);
    }

    return NULL;
}

Mercator::TerrainMod * TerrainModProperty::parseModData(const Element & modifier, const Point3D & newPos)
{
    if (!modifier.isMap()) {
        log(ERROR, "Invalid terrain mod data");
        return NULL;
    }

    const Atlas::Message::MapType & modMap = modifier.asMap();
    m_terrainmods = modMap;
    std::string modType;

    Atlas::Message::MapType shapeMap;
    Atlas::Message::MapType::const_iterator mod_I;

    // Get modifier type
    mod_I = modMap.find("type");
    if (mod_I != modMap.end()) {
    const Atlas::Message::Element& modTypeElem(mod_I->second);
        if (modTypeElem.isString()) {
            modType = modTypeElem.asString();
        }
    }

    // Get modifier's shape data
    mod_I = modMap.find("shape");
    if (mod_I != modMap.end()) {
        const Atlas::Message::Element& shapeElem(mod_I->second);
        if (shapeElem.isMap()) {
            shapeMap = shapeElem.asMap();
        } 
    }

    // Build modifier from data
    if (modType == "slopemod") {
        float dx = 0, dy = 0, level = 0;
        // Get slopes
        mod_I = modMap.find("slopes");
        if (mod_I != modMap.end()) {
            const Atlas::Message::Element& modSlopeElem = mod_I->second;
            if (modSlopeElem.isList()) {
                const Atlas::Message::ListType & slopes = modSlopeElem.asList();
                dx = (int)slopes[0].asNum();
                dy = (int)slopes[1].asNum();
            }
        }
        // Get level
        mod_I = modMap.find("height");
        if (mod_I != modMap.end()) {
            const Atlas::Message::Element& modHeightElem = mod_I->second;
            level = modHeightElem.asNum();
        }

        Point3D pos = newPos;
        pos.z() = level;        // Note that the height of the mod is in pos.z()
        return newSlopeMod(shapeMap, pos, dx, dy);
    }

    if (modType == "levelmod") {
        float level = 0;
        // Get level
        mod_I = modMap.find("height");
        if (mod_I != modMap.end()) {
            const Atlas::Message::Element& modHeightElem = mod_I->second;
            level = modHeightElem.asNum();
        }

        Point3D pos = newPos;
        pos.z() = level;        // Note that the level the terrain will be raised to is in pos.z()
        return newLevelMod(shapeMap, pos);
    }
    
    if (modType == "adjustmod") {
        float level = 0;
        // Get level
        mod_I = modMap.find("height");
        if (mod_I != modMap.end()) {
            const Atlas::Message::Element& modHeightElem = mod_I->second;
            level = modHeightElem.asNum();
        }
        Point3D pos = newPos;
        pos.z() = level;        // Note that the level used in the adjustment is in pos.z()
        return newAdjustMod(shapeMap, pos);
    }
    //log(INFO,"Successfully parsed an adjustmod");

    if (modType == "cratermod") {

        return newCraterMod(shapeMap, newPos);
    }

    return NULL;
}

Mercator::TerrainMod* TerrainModProperty::newCraterMod(const Atlas::Message::MapType shapeMap, const Point3D & pos)
{
    std::string shapeType;

    // Get modifier's shape
    Atlas::Message::MapType::const_iterator shape_I;
    // Get shape's type
    shape_I = shapeMap.find("type");
    if (shape_I != shapeMap.end()) {
        const Atlas::Message::Element& shapeTypeElem(shape_I->second);
        if (shapeTypeElem.isString()) {
        shapeType = shapeTypeElem.asString();
        }
    }
    // end shape data

    if (shapeType == "ball" ) {
        float shapeRadius = 0;
        // Get sphere's radius
        Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
        if (shape_I != shapeMap.end()) {
            const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
            shapeRadius = shapeRadiusElem.asNum();
        }

        // Make sphere
        WFMath::Ball<3> modShape = WFMath::Ball<3>(pos, shapeRadius); ///FIXME: assumes 3d ball...

        //log(INFO,"Successfully parsed a cratermod");
        // Make modifier
        Mercator::CraterTerrainMod *NewMod;
        NewMod = new Mercator::CraterTerrainMod(modShape);

        return NewMod;
    }

    log(ERROR, "CraterTerrainMod defined with incorrect shape");
    return NULL;
}

Mercator::TerrainMod* TerrainModProperty::newLevelMod(const Atlas::Message::MapType shapeMap, const Point3D & pos)
{
    std::string shapeType;
    Atlas::Message::MapType::const_iterator shape_I;

    // Get shape's type
    shape_I = shapeMap.find("type");
    if (shape_I != shapeMap.end()) {
        const Atlas::Message::Element & shapeTypeElem(shape_I->second);
        if (shapeTypeElem.isString()) {
            shapeType = shapeTypeElem.asString();
        }
    }

    if (shapeType == "ball") {
        float shapeRadius = 0;
        // Get sphere's radius
        shape_I = shapeMap.find("radius");
        if (shape_I != shapeMap.end()) {
            const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
            shapeRadius = shapeRadiusElem.asNum();
        }

        // Make disc
        WFMath::Point<2> pos_2d(pos.x(),pos.y());
        WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius); ///FIXME: assumes 2d ball...

        // Make Modifier
        Mercator::LevelTerrainMod<WFMath::Ball<2> > *NewMod;
        NewMod = new Mercator::LevelTerrainMod<WFMath::Ball<2> >(pos.z(), modShape);

        return NewMod;
    }

    if (shapeType == "rotbox") {
        WFMath::Point<2> shapePoint;
        WFMath::Vector<2> shapeVector;
        // Get rotbox's position
        shape_I = shapeMap.find("point");
        if (shape_I != shapeMap.end()) {
        const Atlas::Message::Element& shapePointElem(shape_I->second);
            if (shapePointElem.isList()) {
                const Atlas::Message::ListType & pointList = shapePointElem.asList();
                shapePoint = WFMath::Point<2>((int)pointList[0].asNum(), (int)pointList[1].asNum());
            }
        }
        // Get rotbox's vector
        shape_I = shapeMap.find("vector");
        if (shape_I != shapeMap.end()) {
            const Atlas::Message::Element& shapeVectorElem(shape_I->second);
            if (shapeVectorElem.isList()) {
                const Atlas::Message::ListType & vectorList = shapeVectorElem.asList(); 
                shapeVector = WFMath::Vector<2>((int)vectorList[0].asNum(), (int)vectorList[1].asNum());
            }
        }

        // Make rotbox
        ///FIXME: needs to use shapeDim instead of 2
        WFMath::RotBox<2> modShape = WFMath::RotBox<2>(shapePoint, shapeVector, WFMath::RotMatrix<2>()); 

        // Make modifier
        Mercator::LevelTerrainMod<WFMath::RotBox<2> > *NewMod;
        NewMod = new Mercator::LevelTerrainMod<WFMath::RotBox<2> >(pos.z(), modShape);

        return NewMod;
    }

    log(ERROR, "LevelTerrainMod defined with incorrect shape data");
    return NULL;
}

Mercator::TerrainMod * TerrainModProperty::newSlopeMod(const Atlas::Message::MapType shapeMap,
                                                       const Point3D & pos, float dx, float dy)
{
    std::string shapeType;

    // Get modifier's shape
    Atlas::Message::MapType::const_iterator shape_I;
    // Get shape's type
    shape_I = shapeMap.find("type");
    if (shape_I != shapeMap.end()) {
        const Atlas::Message::Element& shapeTypeElem(shape_I->second);
        if (shapeTypeElem.isString()) {
        shapeType = shapeTypeElem.asString();
        }
    }
    // end shape data

    if (shapeType == "ball") {
        float shapeRadius;
        // Get sphere's radius
        Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
        if (shape_I != shapeMap.end()) {
        const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
        shapeRadius = shapeRadiusElem.asNum();
        }

        // Make disc
        WFMath::Point<2> pos_2d(pos.x(),pos.y());
        WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius);

        // log(INFO, "Successfully parsed a slopemod");

        // Make modifier

    }

    log(ERROR, "SlopeTerrainMod defined with incorrect shape data");
    return NULL;
}

Mercator::TerrainMod * TerrainModProperty::newAdjustMod(const Atlas::Message::MapType shapeMap, const Point3D & pos)
{
    std::string shapeType;

    // Get modifier's shape
    Atlas::Message::MapType::const_iterator shape_I;
    // Get shape's type
    shape_I = shapeMap.find("type");
    if (shape_I != shapeMap.end()) {
        const Atlas::Message::Element& shapeTypeElem(shape_I->second);

        if (shapeTypeElem.isString()) {
        shapeType = shapeTypeElem.asString();
        }
    }
    // end shape data

    if (shapeType == "ball") {
        float shapeRadius;
        // Get sphere's radius
        Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
        if (shape_I != shapeMap.end()) {
            const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
            shapeRadius = shapeRadiusElem.asNum();
        }

        // Make sphere
        WFMath::Point<2> pos_2d(pos.x(), pos.y());
        WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius);

        // Make modifier

        // Apply Modifier

    }

    log(ERROR, "AdjustTerrainMod defined with incorrect shape data");
    return NULL;
}
