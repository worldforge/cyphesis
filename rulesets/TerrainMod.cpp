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

// $Id$

#include "TerrainMod.h"

#include "InnerTerrainMod_impl.h"

#include "common/log.h"
#include "common/debug.h"

#include <Mercator/TerrainMod.h>

#include <wfmath/atlasconv.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

/**
 * @brief Ctor.
 * This is protected to prevent any other class than subclasses of this to call it.
 * @param terrainMod The TerrainMod instance to which this instance belongs to.
 * @param typemod The type of terrainmod this handles, such as "cratermod" or "slopemod. This will be stored in mTypeName.
 */
InnerTerrainMod::InnerTerrainMod(const std::string& typemod) : mTypeName(typemod)
{
}

/// @brief Dtor.
InnerTerrainMod::~InnerTerrainMod()
{
}

/**
 * @brief Gets the type of terrain mod handled by this.
 * This corresponds to the "type" attribute of the "terrainmod" atlas attribute, for example "cratermod" or "slopemod".
 * Internally, it's stored in the mTypeName field, as set through the constructor.
 * @return The type of mod handled by any instance of this.
 */
const std::string& InnerTerrainMod::getTypename() const
{
    return mTypeName;
}

Mercator::TerrainMod* InnerTerrainMod::getModifier()
{
    return m_impl.getModifier();
}

/**
 * @brief Parses the position of the mod.
 * If no height data is given the height of the entity the mod belongs to will be used.
 * If however a "height" value is set, that will be used instead.
 * If no "height" value is set, but a "heightoffset" is present, that value will be added to the height set by the position of the entity the mod belongs to.
 * @param modElement The top mod element.
 * @return The position of the mod, where the height has been adjusted.
 */
float InnerTerrainMod::parsePosition(const WFMath::Point<3> & pos, const MapType& modElement)
{
    ///If the height is specified use that, else check for a height offset. If none is found, use the default height of the entity position
    MapType::const_iterator I = modElement.find("height");
    if (I != modElement.end()) {
        const Element& modHeightElem = I->second;
        if (modHeightElem.isNum()) {
            return modHeightElem.asNum();
        }
    } else {
        I = modElement.find("heightoffset");
        if (I != modElement.end()) {
            const Element& modHeightElem = I->second;
            if (modHeightElem.isNum()) {
                float heightoffset = modHeightElem.asNum();
                return pos.z() + heightoffset;
            }
        }
    }
    return pos.z();
}

InnerTerrainModCrater::InnerTerrainModCrater()
: InnerTerrainMod("cratermod")
{
}

InnerTerrainModCrater::~InnerTerrainModCrater()
{
}


bool InnerTerrainModCrater::parseAtlasData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation, const MapType& modElement)
{
    float level = parsePosition(pos, modElement);
    Element shapeMap;
    Shape shapeType = parseShape(modElement, shapeMap);
    if (!shapeMap.isNone()) {
        if (shapeType == SHAPE_BALL) {
            return m_impl.createInstance<WFMath::Ball, Mercator::CraterTerrainMod>(shapeMap, pos, orientation, level);
        }
    }
    log(ERROR, "Crater terrain mod defined with incorrect shape");
    return false;
}


InnerTerrainModSlope::InnerTerrainModSlope()
: InnerTerrainMod("slopemod")
{
}

InnerTerrainModSlope::~InnerTerrainModSlope()
{
}

bool InnerTerrainModSlope::parseAtlasData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation, const MapType& modElement)
{
    // Get slopes
    MapType::const_iterator I = modElement.find("slopes");
    if (I != modElement.end()) {
        const Element& modSlopeElem = I->second;
        if (modSlopeElem.isList()) {
            const ListType & slopes = modSlopeElem.asList();
            if (slopes.size() > 1) {
                if (slopes[0].isNum() && slopes[1].isNum()) {
                    const float dx = slopes[0].asNum();
                    const float dy = slopes[1].asNum();
                    float level = parsePosition(pos, modElement);
                    Element shapeMap;
                    Shape shapeType = parseShape(modElement, shapeMap);
                    if (!shapeMap.isNone()) {
                        if (shapeType == SHAPE_BALL) {
                            return m_impl.createInstance<WFMath::Ball, Mercator::SlopeTerrainMod>(shapeMap, pos, orientation, level, dx, dy);
                        } else if (shapeType == SHAPE_ROTBOX) {
                            return m_impl.createInstance<WFMath::RotBox, Mercator::SlopeTerrainMod>(shapeMap, pos, orientation, level, dx, dy);
                        } else if (shapeType == SHAPE_POLYGON) {
                            return m_impl.createInstance<WFMath::Polygon, Mercator::SlopeTerrainMod>(shapeMap, pos, orientation, level, dx, dy);
                        }
                    }
                }
            }
        }
    }
    log(ERROR, "SlopeTerrainMod defined with incorrect shape");
    return false;
}


InnerTerrainModLevel::InnerTerrainModLevel()
: InnerTerrainMod("levelmod")
{
}

InnerTerrainModLevel::~InnerTerrainModLevel()
{
}

bool InnerTerrainModLevel::parseAtlasData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation, const MapType& modElement)
{
    float level = parsePosition(pos, modElement);
    Element shapeMap;
    Shape shapeType = parseShape(modElement, shapeMap);
    if (!shapeMap.isNone()) {
        if (shapeType == SHAPE_BALL) {
            return m_impl.createInstance<WFMath::Ball, Mercator::LevelTerrainMod>(shapeMap, pos, orientation, level);
        } else if (shapeType == SHAPE_ROTBOX) {
            return m_impl.createInstance<WFMath::RotBox, Mercator::LevelTerrainMod>(shapeMap, pos, orientation, level);
        } else if (shapeType == SHAPE_POLYGON) {
            return m_impl.createInstance<WFMath::Polygon, Mercator::LevelTerrainMod>(shapeMap, pos, orientation, level);
        }
    }
    log(ERROR, "Level terrain mod defined with incorrect shape");
    return false;
}

InnerTerrainModAdjust::InnerTerrainModAdjust()
: InnerTerrainMod("adjustmod")
{
}

InnerTerrainModAdjust::~InnerTerrainModAdjust()
{
}

bool InnerTerrainModAdjust::parseAtlasData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation, const MapType& modElement)
{

    Element shapeMap;
    Shape shapeType = parseShape(modElement, shapeMap);
    if (!shapeMap.isNone()) {
        if (shapeType == SHAPE_BALL) {
            return m_impl.createInstance<WFMath::Ball, Mercator::AdjustTerrainMod>(shapeMap, pos, orientation, pos.z());
        } else if (shapeType == SHAPE_ROTBOX) {
            return m_impl.createInstance<WFMath::RotBox, Mercator::AdjustTerrainMod>(shapeMap, pos, orientation, pos.z());
        } else if (shapeType == SHAPE_POLYGON) {
            return m_impl.createInstance<WFMath::Polygon, Mercator::AdjustTerrainMod>(shapeMap, pos, orientation, pos.z());
        }
    }
    log(ERROR, "Adjust terrain mod defined with incorrect shape");
    return false;
}

/**
 * @brief Parses the atlas data of the modifiers, finding the base atlas element for the shape definition, and returning the kind of shape specified.
 * This is an utility method to help with those many cases where we need to parse the shape data in order to determine the kind of shape. The actual parsing and creation of the shape happens in InnerTerrainMod_impl however, since that depends on templated calls. However, in order to know what kind of template to use we must first look at the type of shape, thus the need for this method.
 * @param modElement The atlas element containing the modifier.
 * @param shapeMap A shape data is found, and it's in the map form, it will be put here.
 * @return The name of the shape, or an empty string if no valid data could be found.
 */
InnerTerrainMod::Shape InnerTerrainMod::parseShape(const MapType& modElement, Element& shapeMap)
{
    MapType::const_iterator I = modElement.find("shape");
    if (I != modElement.end()) {
        const Element& shapeElement = I->second;
        if (shapeElement.isMap()) {
            const MapType& localShapeMap = shapeElement.asMap();
            shapeMap = localShapeMap;

            // Get shape's type
            MapType::const_iterator J = localShapeMap.find("type");
            if (J != localShapeMap.end()) {
                const Element& shapeTypeElem(J->second);
                if (shapeTypeElem.isString()) {
                    const std::string& shapeType = shapeTypeElem.asString();
                    if (shapeType == "rotbox") {
                        return SHAPE_ROTBOX;
                    } else if (shapeType == "polygon") {
                        return SHAPE_POLYGON;
                    } else if (shapeType == "ball") {
                        return SHAPE_BALL;
                    }
                }
            }
        }
    }
    return SHAPE_UNKNOWN;
}
