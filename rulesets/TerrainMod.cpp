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

#include "rulesets/Entity.h"
#include "rulesets/InnerTerrainMod_impl.h"

#include "common/log.h"
#include "common/debug.h"

#include "modules/Location.h"

#include <Mercator/TerrainMod.h>

#include <wfmath/atlasconv.h>

#include <cassert>

static const bool debug_flag = false;

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

/**
 * @brief Parses the position of the mod.
 * If no height data is given the height of the entity the mod belongs to will be used.
 * If however a "height" value is set, that will be used instead.
 * If no "height" value is set, but a "heightoffset" is present, that value will be added to the height set by the position of the entity the mod belongs to.
 * @param modElement The top mod element.
 * @return The position of the mod, where the height has been adjusted.
 */
WFMath::Point<3> InnerTerrainMod::parsePosition(Entity * owner, const MapType& modElement)
{
    ///If the height is specified use that, else check for a height offset. If none is found, use the default height of the entity position
    WFMath::Point<3> pos = owner->m_location.pos();
    MapType::const_iterator I = modElement.find("height");
    if (I != modElement.end()) {
        const Element& modHeightElem = I->second;
        if (modHeightElem.isNum()) {
            float height = modHeightElem.asNum();
            pos.z() = height;
        }
    } else {
        I = modElement.find("heightoffset");
        if (I != modElement.end()) {
            const Element& modHeightElem = I->second;
            if (modHeightElem.isNum()) {
                float heightoffset = modHeightElem.asNum();
                pos.z() += heightoffset;
            }
        }
    }
    return pos;
}

InnerTerrainModCrater::InnerTerrainModCrater()
: InnerTerrainMod("cratermod")
, mModifier_impl(0)
{
}

InnerTerrainModCrater::~InnerTerrainModCrater()
{
    delete mModifier_impl;
}

Mercator::TerrainMod* InnerTerrainModCrater::getModifier()
{
    return mModifier_impl->getModifier();
}


bool InnerTerrainModCrater::parseAtlasData(Entity * owner, const MapType& modElement)
{
    WFMath::Point<3> pos = parsePosition(owner, modElement);
    Element shapeMap;
    const std::string& shapeType = parseShape(modElement, shapeMap);
    if (!shapeMap.isNone()) {
        if (shapeType == "ball") {
            InnerTerrainModCrater_impl<WFMath::Ball>* modifierImpl = new InnerTerrainModCrater_impl<WFMath::Ball>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
        }
    }
    log(ERROR, "Crater terrain mod defined with incorrect shape");
    return false;
}


InnerTerrainModSlope::InnerTerrainModSlope()
: InnerTerrainMod("slopemod")
, mModifier_impl(0)
{
}

InnerTerrainModSlope::~InnerTerrainModSlope()
{
    delete mModifier_impl;
}

Mercator::TerrainMod* InnerTerrainModSlope::getModifier()
{
    if (mModifier_impl == 0) {
        return 0;
    }
    return mModifier_impl->getModifier();
}


bool InnerTerrainModSlope::parseAtlasData(Entity * owner, const MapType& modElement)
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
                    WFMath::Point<3> pos = parsePosition(owner, modElement);
                    Element shapeMap;
                    const std::string& shapeType = parseShape(modElement, shapeMap);
                    if (!shapeMap.isNone()) {
                        if (shapeType == "ball") {
                            InnerTerrainModSlope_impl<WFMath::Ball>* modifierImpl = new InnerTerrainModSlope_impl<WFMath::Ball>();
                            mModifier_impl = modifierImpl;
                            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z(), dx, dy);
                        } else if (shapeType == "rotbox") {
                            InnerTerrainModSlope_impl<WFMath::RotBox>* modifierImpl = new InnerTerrainModSlope_impl<WFMath::RotBox>();
                            mModifier_impl = modifierImpl;
                            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z(), dx, dy);
                        } else if (shapeType == "polygon") {
                            InnerTerrainModSlope_impl<WFMath::Polygon>* modifierImpl = new InnerTerrainModSlope_impl<WFMath::Polygon>();
                            mModifier_impl = modifierImpl;
                            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z(), dx, dy);
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
, mModifier_impl(0)
{
}

InnerTerrainModLevel::~InnerTerrainModLevel()
{
    delete mModifier_impl;
}

Mercator::TerrainMod* InnerTerrainModLevel::getModifier()
{
    if (mModifier_impl == 0) {
        return 0;
    }
    return mModifier_impl->getModifier();
}

bool InnerTerrainModLevel::parseAtlasData(Entity * owner, const MapType& modElement)
{
    WFMath::Point<3> pos = parsePosition(owner, modElement);
    Element shapeMap;
    const std::string& shapeType = parseShape(modElement, shapeMap);
    if (!shapeMap.isNone()) {
        if (shapeType == "ball") {
            InnerTerrainModLevel_impl<WFMath::Ball>* modifierImpl = new InnerTerrainModLevel_impl<WFMath::Ball>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "rotbox") {
            InnerTerrainModLevel_impl<WFMath::RotBox>* modifierImpl = new InnerTerrainModLevel_impl<WFMath::RotBox>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "polygon") {
            InnerTerrainModLevel_impl<WFMath::Polygon>* modifierImpl = new InnerTerrainModLevel_impl<WFMath::Polygon>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
        }
    }
    log(ERROR, "Level terrain mod defined with incorrect shape");
    return false;
}

InnerTerrainModAdjust::InnerTerrainModAdjust()
: InnerTerrainMod("adjustmod")
, mModifier_impl(0)
{
}

InnerTerrainModAdjust::~InnerTerrainModAdjust()
{
    delete mModifier_impl;
}

Mercator::TerrainMod* InnerTerrainModAdjust::getModifier()
{
    if (mModifier_impl == 0) {
        return 0;
    }
    return mModifier_impl->getModifier();
}


bool InnerTerrainModAdjust::parseAtlasData(Entity * owner, const MapType& modElement)
{

    WFMath::Point<3> pos = owner->m_location.pos();
    Element shapeMap;
    const std::string& shapeType = parseShape(modElement, shapeMap);
    if (!shapeMap.isNone()) {
        if (shapeType == "ball") {
            InnerTerrainModAdjust_impl<WFMath::Ball>* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::Ball>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "rotbox") {
            InnerTerrainModAdjust_impl<WFMath::RotBox>* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::RotBox>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "polygon") {
            InnerTerrainModAdjust_impl<WFMath::Polygon>* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::Polygon>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
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
const std::string& InnerTerrainMod::parseShape(const MapType& modElement, Element& shapeMap)
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
                    return shapeType;
                }
            }
        }
    }
    static std::string empty("");
    return empty;
}
