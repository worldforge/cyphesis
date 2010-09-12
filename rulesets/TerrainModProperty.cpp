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

#include "common/compose.hpp"
#include "common/log.h"
#include "common/debug.h"

#include "modules/Location.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TerrainMod.h>

#include <wfmath/atlasconv.h>

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


InnerTerrainMod::InnerTerrainMod(TerrainModProperty& terrainMod, const std::string& typemod)
: mTerrainMod(terrainMod)
{
}

InnerTerrainMod::~InnerTerrainMod()
{
}

const std::string& InnerTerrainMod::getTypename() const
{
    return mTypeName;
}

WFMath::Point<3> InnerTerrainMod::parsePosition(Entity * owner, const Atlas::Message::MapType& modElement)
{
    ///If the height is specified use that, else check for a height offset. If none is found, use the default height of the entity position
    WFMath::Point<3> pos = owner->m_location.pos();
    Atlas::Message::MapType::const_iterator mod_I = modElement.find("height");
    if (mod_I != modElement.end()) {
        const Atlas::Message::Element& modHeightElem = mod_I->second;
        if (modHeightElem.isNum()) {
            float height = modHeightElem.asNum();
            pos.z() = height;
        }
    } else {
        mod_I = modElement.find("heightoffset");
        if (mod_I != modElement.end()) {
            const Atlas::Message::Element& modHeightElem = mod_I->second;
            if (modHeightElem.isNum()) {
                float heightoffset = modHeightElem.asNum();
                pos.z() += heightoffset;
            }
        }
    }
    return pos;
}

InnerTerrainModCrater::InnerTerrainModCrater(TerrainModProperty& terrainMod)
: InnerTerrainMod(terrainMod, "cratermod")
, mModifier(0)
{
}

InnerTerrainModCrater::~InnerTerrainModCrater()
{
    delete mModifier;
}

Mercator::TerrainMod* InnerTerrainModCrater::getModifier()
{
    return mModifier;
}


bool InnerTerrainModCrater::parseAtlasData(Entity * owner, const Atlas::Message::MapType& modElement)
{

    const Atlas::Message::Element* shapeMap(0);
    const std::string& shapeType = parseShape(modElement, &shapeMap);
    if (shapeMap) {
        if (shapeType == "ball") {
            WFMath::Point<3> pos = parsePosition(owner, modElement);

            WFMath::Ball<3> shape;
            try {
                shape.fromAtlas(*shapeMap);
                shape.shift(WFMath::Vector<3>(pos.x(), pos.y(), pos.z()));
                mModifier = new Mercator::CraterTerrainMod(shape);
                return true;
            } catch (const Atlas::Message::WrongTypeException& ex) {
                ///Just fall through
            }
        }
    }
    log(ERROR, "Crater terrain mod defined with incorrect shape");
    return false;
}


InnerTerrainModSlope::InnerTerrainModSlope(TerrainModProperty& terrainMod)
: InnerTerrainMod(terrainMod, "slopemod")
, mModifier_impl(0)
{
}

InnerTerrainModSlope::~InnerTerrainModSlope()
{
    delete mModifier_impl;
}

Mercator::TerrainMod* InnerTerrainModSlope::getModifier()
{
    return mModifier_impl->getModifier();
}


bool InnerTerrainModSlope::parseAtlasData(Entity * owner, const Atlas::Message::MapType& modElement)
{
    float dx, dy;
    // Get slopes
    Atlas::Message::MapType::const_iterator mod_I = modElement.find("slopes");
    if (mod_I != modElement.end()) {
        const Atlas::Message::Element& modSlopeElem = mod_I->second;
        if (modSlopeElem.isList()) {
            const Atlas::Message::ListType & slopes = modSlopeElem.asList();
            if (slopes.size() > 1) {
                if (slopes[0].isNum() && slopes[1].isNum()) {
                    dx = slopes[0].asNum();
                    dy = slopes[1].asNum();
                    WFMath::Point<3> pos = parsePosition(owner, modElement);
                    const Atlas::Message::Element* shapeMap(0);
                    const std::string& shapeType = parseShape(modElement, &shapeMap);
                    if (shapeMap) {
                        if (shapeType == "ball") {
                            InnerTerrainModSlope_impl<WFMath::Ball<2> >* modifierImpl = new InnerTerrainModSlope_impl<WFMath::Ball<2> >();
                            mModifier_impl = modifierImpl;
                            return modifierImpl->createInstance(*shapeMap, pos, owner->m_location.orientation(), pos.z(), dx, dy);
                        } else if (shapeType == "rotbox") {
                            InnerTerrainModSlope_impl<WFMath::RotBox<2> >* modifierImpl = new InnerTerrainModSlope_impl<WFMath::RotBox<2> >();
                            mModifier_impl = modifierImpl;
                            return modifierImpl->createInstance(*shapeMap, pos, owner->m_location.orientation(), pos.z(), dx, dy);
                        } else if (shapeType == "polygon") {
                            InnerTerrainModSlope_impl<WFMath::Polygon<2> >* modifierImpl = new InnerTerrainModSlope_impl<WFMath::Polygon<2> >();
                            mModifier_impl = modifierImpl;
                            return modifierImpl->createInstance(*shapeMap, pos, owner->m_location.orientation(), pos.z(), dx, dy);
                        }
                    }
                }
            }
        }
    }
    log(ERROR, "SlopeTerrainMod defined with incorrect shape");
    return false;
}


InnerTerrainModLevel::InnerTerrainModLevel(TerrainModProperty& terrainMod)
: InnerTerrainMod(terrainMod, "levelmod")
, mModifier_impl(0)
{
}

InnerTerrainModLevel::~InnerTerrainModLevel()
{
    delete mModifier_impl;
}

Mercator::TerrainMod* InnerTerrainModLevel::getModifier()
{
    return mModifier_impl->getModifier();
}

bool InnerTerrainModLevel::parseAtlasData(Entity * owner, const Atlas::Message::MapType& modElement)
{
    WFMath::Point<3> pos = parsePosition(owner, modElement);
    const Atlas::Message::Element* shapeMap(0);
    const std::string& shapeType = parseShape(modElement, &shapeMap);
    if (shapeMap) {
        if (shapeType == "ball") {
            InnerTerrainModLevel_impl<WFMath::Ball<2> >* modifierImpl = new InnerTerrainModLevel_impl<WFMath::Ball<2> >();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "rotbox") {
            InnerTerrainModLevel_impl<WFMath::RotBox<2> >* modifierImpl = new InnerTerrainModLevel_impl<WFMath::RotBox<2> >();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "polygon") {
            InnerTerrainModLevel_impl<WFMath::Polygon<2> >* modifierImpl = new InnerTerrainModLevel_impl<WFMath::Polygon<2> >();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, owner->m_location.orientation(), pos.z());
        }
    }
    log(ERROR, "Level terrain mod defined with incorrect shape");
    return false;
}

InnerTerrainModAdjust::InnerTerrainModAdjust(TerrainModProperty& terrainMod)
: InnerTerrainMod(terrainMod, "adjustmod")
, mModifier_impl(0)
{
}

InnerTerrainModAdjust::~InnerTerrainModAdjust()
{
    delete mModifier_impl;
}

Mercator::TerrainMod* InnerTerrainModAdjust::getModifier()
{
    return mModifier_impl->getModifier();
}


bool InnerTerrainModAdjust::parseAtlasData(Entity * owner, const Atlas::Message::MapType& modElement)
{

    WFMath::Point<3> pos = owner->m_location.pos();
    const Atlas::Message::Element* shapeMap(0);
    const std::string& shapeType = parseShape(modElement, &shapeMap);
    if (shapeMap) {
        if (shapeType == "ball") {
            InnerTerrainModAdjust_impl<WFMath::Ball<2> >* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::Ball<2> >();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "rotbox") {
            InnerTerrainModAdjust_impl<WFMath::RotBox<2> >* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::RotBox<2> >();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "polygon") {
            InnerTerrainModAdjust_impl<WFMath::Polygon<2> >* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::Polygon<2> >();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, owner->m_location.orientation(), pos.z());
        }
    }
    log(ERROR, "Adjust terrain mod defined with incorrect shape");
    return false;
}


const std::string& InnerTerrainMod::parseShape(const Atlas::Message::MapType& modElement, const Atlas::Message::Element** shapeMap)
{
    Atlas::Message::MapType::const_iterator shape_I = modElement.find("shape");
    if (shape_I != modElement.end()) {
        const Atlas::Message::Element& shapeElement = shape_I->second;
        if (shapeElement.isMap()) {
            const Atlas::Message::MapType& localShapeMap = shapeElement.asMap();
            *shapeMap = &shapeElement;

            // Get shape's type
            Atlas::Message::MapType::const_iterator type_I = localShapeMap.find("type");
            if (type_I != localShapeMap.end()) {
                const Atlas::Message::Element& shapeTypeElem(type_I->second);
                if (shapeTypeElem.isString()) {
                    const std::string& shapeType = shapeTypeElem.asString();
                    return shapeType;
                }
            }
        }
    }
    static std::string empty("");
    return empty;
}










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

TerrainProperty* TerrainModProperty::getTerrain(Entity * owner)
{
    PropertyBase * terr;
    Entity * ent = owner;

    while ( (terr = ent->modProperty("terrain")) == NULL) {
        ent = (Entity*)(ent->m_location.m_loc);
        if (ent == NULL) {
            return NULL;
        }
    }

    TerrainProperty * tp = dynamic_cast<TerrainProperty*>(terr);
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
    TerrainProperty * terr = NULL;
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
    TerrainProperty* terrain = getTerrain(owner);
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
        TerrainProperty* terrain = getTerrain(owner);
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
