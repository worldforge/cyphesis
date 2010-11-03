//
// C++ Interface: TerrainMod_impl
//
// Description: 
//
//
// Author: Erik Hjortsberg <erik.hjortsberg@iteam.se>, (C) 2008
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.//
//
#ifndef RULESETS_TERRAIN_MOD_IMPL_H
#define RULESETS_TERRAIN_MOD_IMPL_H

#include <Mercator/TerrainMod.h>

#include <Atlas/Message/Element.h>

#include <wfmath/ball.h>
#include <wfmath/polygon.h>

#include "common/log.h"

/**
 @brief Base class for the inner terrain mod implementations.
 We keep this separate from the TerrainMod classes because of the way these classes interacts with templated WFMath structures.
 The parseShapeAtlasData() method can also be used externally as it's a static public.
 @author Erik Hjortsberg <erik.hjortsberg@iteam.se>
*/
class InnerTerrainMod_impl
{
public:

    /**
     *    @brief Ctor.
     */
    InnerTerrainMod_impl() {}

    /**
     *    @brief Dtor.
     */
    virtual ~InnerTerrainMod_impl() {}

    /**
     * @brief Common method for parsing shape data from Atlas.
     * Since each different shape expects different Atlas data this is a templated method with specialized implemtations for each available shape. If you call this and get error regarding missing implementations it probably means that there's no implementation for the type of shape you're calling it with.
     * Note that a new shape instance will be put on the heap if the parsing is successful, and it's up to the calling code to properly delete it when done.
     * @param shapeElement The atlas map element which contains the shape data. Often this is found with the key "shape" in the atlas data.
     * @param pos The original position of the entity to which this shape will belong. The shape will be positioned according to this.
     * @param shape The resulting shape is meant to be put here, if successfully created. That means that a new shape instance will be created, and it's then up to the calling method to properly delete it, to avoid memory leaks.
     * @return True if the atlas data was successfully parsed and a shape was created.
     */
    template <template <int> class Shape>
    static bool parseShapeAtlasData(const Atlas::Message::Element& shapeElement,
                                    const WFMath::Point<3>& pos,
                                    const WFMath::Quaternion& orientation,
                                    Shape<2> & shape);


    template <template <int> class Shape,
              template <template <int> class Shape> class Mod>
    bool createInstance(const Atlas::Message::Element& shapeElement,
                        const WFMath::Point<3>& pos,
                        const WFMath::Quaternion& orientation,
                        float,
                        float,
                        float);

    
    template <template <int> class Shape,
              template <template <int> class Shape> class Mod>
    bool createInstance(const Atlas::Message::Element& shapeElement,
                        const WFMath::Point<3>& pos,
                        const WFMath::Quaternion& orientation,
                        float);

    /**
     * @brief Gets the modifier which this instance represents.
     * @return A pointer to a terrain modifier, or null if none could be created.
     */
    Mercator::TerrainMod* getModifier()
    {
        return mTerrainMod;
    }

protected:

    Mercator::TerrainMod * mTerrainMod;
};

template<template <int> class Shape>
bool InnerTerrainMod_impl::parseShapeAtlasData(const Atlas::Message::Element& shapeElement,
                                               const WFMath::Point<3>& pos,
                                               const WFMath::Quaternion& orientation,
                                               Shape <2> & shape)
{
    try {
        shape.fromAtlas(shapeElement);
    } catch (...) {
        ///Just log an error and return false, this isn't fatal.
        log(WARNING, "Error when parsing shape from atlas.");
        return false;
    }

    if (!shape.isValid()) {
        return false;
    }

    if (orientation.isValid()) {
        /// rotation about Z axis
        WFMath::Vector<3> xVec = WFMath::Vector<3>(1.0, 0.0, 0.0).rotate(orientation);
        double theta = atan2(xVec.y(), xVec.x());
        WFMath::RotMatrix<2> rm;
        shape.rotatePoint(rm.rotation(theta), WFMath::Point<2>(0, 0));
    }

    shape.shift(WFMath::Vector<2>(pos.x(), pos.y()));
    return true;
}

template <template <int> class Shape,
          template <template <int> class Shape> class Mod>
bool InnerTerrainMod_impl::createInstance(
      const Atlas::Message::Element& shapeElement,
      const WFMath::Point<3>& pos,
      const WFMath::Quaternion& orientation,
      float level,
      float dx,
      float dy)
{
    Shape<2>  shape;
    if (parseShapeAtlasData(shapeElement, pos, orientation, shape)) {
        mTerrainMod = new Mod<Shape>(level, dx, dy, shape);
        return true;
    }
    return false;
}

template <template <int> class Shape,
          template <template <int> class S> class Mod>
bool InnerTerrainMod_impl::createInstance(
      const Atlas::Message::Element& shapeElement,
      const WFMath::Point<3>& pos,
      const WFMath::Quaternion& orientation,
      float height)
{
    Shape<2>  shape;
    if (parseShapeAtlasData(shapeElement, pos, orientation, shape)) {
        mTerrainMod = new Mod<Shape>(height, shape);
        return true;
    }
    return false;
}

#endif
