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

#include "TerrainMod.h"

#include "rulesets/Entity.h"
#include "rulesets/InnerTerrainMod_impl.h"

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

InnerTerrainMod::InnerTerrainMod(const std::string& typemod) : mTypeName(typemod)
{
}

InnerTerrainMod::~InnerTerrainMod()
{
}

const std::string& InnerTerrainMod::getTypename() const
{
    return mTypeName;
}

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


bool InnerTerrainModCrater::parseAtlasData(Entity * owner, const MapType& modElement)
{

    Element shapeMap;
    const std::string& shapeType = parseShape(modElement, shapeMap);
    if (!shapeMap.isNone()) {
        if (shapeType == "ball") {
            WFMath::Point<3> pos = parsePosition(owner, modElement);

            WFMath::Ball<3> shape;
            try {
                shape.fromAtlas(shapeMap);
                if (shape.isValid()) {
                    shape.shift(WFMath::Vector<3>(pos.x(), pos.y(), pos.z()));
                    mModifier = new Mercator::CraterTerrainMod(shape);
                    return true;
                }
            } catch (const Atlas::Message::WrongTypeException& ex) {
                ///Just fall through
            }
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
                            InnerTerrainModSlope_impl<WFMath::Ball<2> >* modifierImpl = new InnerTerrainModSlope_impl<WFMath::Ball<2> >();
                            mModifier_impl = modifierImpl;
                            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z(), dx, dy);
                        } else if (shapeType == "rotbox") {
                            InnerTerrainModSlope_impl<WFMath::RotBox<2> >* modifierImpl = new InnerTerrainModSlope_impl<WFMath::RotBox<2> >();
                            mModifier_impl = modifierImpl;
                            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z(), dx, dy);
                        } else if (shapeType == "polygon") {
                            InnerTerrainModSlope_impl<WFMath::Polygon<2> >* modifierImpl = new InnerTerrainModSlope_impl<WFMath::Polygon<2> >();
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
            InnerTerrainModLevel_impl<WFMath::Ball<2> >* modifierImpl = new InnerTerrainModLevel_impl<WFMath::Ball<2> >();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "rotbox") {
            InnerTerrainModLevel_impl<WFMath::RotBox<2> >* modifierImpl = new InnerTerrainModLevel_impl<WFMath::RotBox<2> >();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "polygon") {
            InnerTerrainModLevel_impl<WFMath::Polygon<2> >* modifierImpl = new InnerTerrainModLevel_impl<WFMath::Polygon<2> >();
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
            InnerTerrainModAdjust_impl<WFMath::Ball<2> >* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::Ball<2> >();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "rotbox") {
            InnerTerrainModAdjust_impl<WFMath::RotBox<2> >* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::RotBox<2> >();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
        } else if (shapeType == "polygon") {
            InnerTerrainModAdjust_impl<WFMath::Polygon<2> >* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::Polygon<2> >();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(shapeMap, pos, owner->m_location.orientation(), pos.z());
        }
    }
    log(ERROR, "Adjust terrain mod defined with incorrect shape");
    return false;
}


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
