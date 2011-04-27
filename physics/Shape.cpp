// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#include "Shape_impl.h"

#include <Atlas/Message/Element.h>

#include <wfmath/polygon.h>
#include <wfmath/stream.h>

using Atlas::Message::MapType;

Shape::Shape()
{
}

///////////////////////////////////////////////////////////////////////

template<>
WFMath::Point<3> MathShape<WFMath::AxisBox, 2>::lowCorner() const
{
    WFMath::Point<2> corner = m_shape.lowCorner();
    return WFMath::Point<3>(corner.x(), corner.y(), 0);
}

template<>
WFMath::Point<3> MathShape<WFMath::AxisBox, 2>::highCorner() const
{
    WFMath::Point<2> corner = m_shape.highCorner();
    return WFMath::Point<3>(corner.x(), corner.y(), 0);
}

template<>
void MathShape<WFMath::AxisBox, 2>::fromAtlas(const MapType & data)
{
    MapType::const_iterator I = data.find("points");
    if (I == data.end() || !I->second.isList()) {
        return;
    }
    m_shape.fromAtlas(I->second.List());
}

///////////////////////////////////////////////////////////////////////

template<>
WFMath::Point<3> MathShape<WFMath::Ball, 2>::lowCorner() const
{
    WFMath::Point<2> corner = m_shape.boundingBox().lowCorner();
    return WFMath::Point<3>(corner.x(), corner.y(), 0);
}

template<>
WFMath::Point<3> MathShape<WFMath::Ball, 2>::highCorner() const
{
    WFMath::Point<2> corner = m_shape.boundingBox().highCorner();
    return WFMath::Point<3>(corner.x(), corner.y(), 0);
}

///////////////////////////////////////////////////////////////////////

template<>
const char * MathShape<WFMath::Polygon, 2>::getType() const
{
    return "polygon";
}

template<>
WFMath::CoordType MathShape<WFMath::Polygon, 2>::area() const
{
    WFMath::CoordType area = 0;

    int n = m_shape.numCorners();
    for (int i = 0; i < n; ++i) {
        WFMath::Point<2> corner = m_shape.getCorner(i);
        WFMath::Point<2> corner2 = m_shape.getCorner((i + 1) % n);
        area += corner.x() * corner2.y();
        area -= corner.y() * corner2.x();
    }

   return fabs(area / 2.f);
}

template<>
WFMath::AxisBox<2> MathShape<WFMath::Polygon, 2>::footprint() const
{
    return m_shape.boundingBox();
}

template<>
WFMath::Point<3> MathShape<WFMath::Polygon, 2>::lowCorner() const
{
    WFMath::Point<2> corner = m_shape.boundingBox().lowCorner();
    return WFMath::Point<3>(corner.x(), corner.y(), 0);
}

template<>
WFMath::Point<3> MathShape<WFMath::Polygon, 2>::highCorner() const
{
    WFMath::Point<2> corner = m_shape.boundingBox().highCorner();
    return WFMath::Point<3>(corner.x(), corner.y(), 0);
}

template<>
void MathShape<WFMath::Polygon, 2>::scale(WFMath::CoordType factor)
{
    for (int i = 0; i < m_shape.numCorners(); ++i) {
        WFMath::Point<2> corner = m_shape.getCorner(i);
        m_shape.moveCorner(i, WFMath::Point<2>(corner.x() * factor,
                                               corner.y() * factor));
    }
}

///////////////////////////////////////////////////////////////////////

template<>
WFMath::Point<3> MathShape<WFMath::RotBox, 2>::lowCorner() const
{
    WFMath::Point<2> corner = m_shape.boundingBox().lowCorner();
    return WFMath::Point<3>(corner.x(), corner.y(), 0);
}

template<>
WFMath::Point<3> MathShape<WFMath::RotBox, 2>::highCorner() const
{
    WFMath::Point<2> corner = m_shape.boundingBox().highCorner();
    return WFMath::Point<3>(corner.x(), corner.y(), 0);
}

///////////////////////////////////////////////////////////////////////

Shape * Shape::newFromAtlas(const MapType & data)
{
    MapType::const_iterator I = data.find("type");
    if (I == data.end() || !I->second.isString()) {
        return 0;
    }
    const std::string & type = I->second.String();
    Shape * new_shape = 0;
    if (type == "polygon") {
        new_shape = new MathShape<WFMath::Polygon>(WFMath::Polygon<2>());
    } else if (type == "ball") {
        new_shape = new MathShape<WFMath::Ball>(WFMath::Ball<2>());
    } else if (type == "rotbox") {
        new_shape = new MathShape<WFMath::RotBox>(WFMath::RotBox<2>());
    }
    if (new_shape != 0) {
        try {
            new_shape->fromAtlas(data);
        }
        catch (Atlas::Message::WrongTypeException e) {
            delete new_shape;
            new_shape = 0;
        }
    }
    return new_shape;
}

template class MathShape<WFMath::AxisBox, 2>;
