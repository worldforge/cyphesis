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

#include <wfmath/line.h>
#include <wfmath/polygon.h>
#include <wfmath/stream.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

Shape::Shape()
{
}

///////////////////////////////////////////////////////////////////////

template<>
const char * MathShape<WFMath::AxisBox, 2>::getType() const
{
    return "box";
}

template<>
int MathShape<WFMath::AxisBox, 2>::fromAtlas(const Element & data)
{
    int ret = -1;
    try {
        if (data.isMap()) {
            const MapType & datamap = data.Map();
            MapType::const_iterator I = datamap.find("points");
            if (I != datamap.end()) {
                m_shape.fromAtlas(I->second);
                ret = 0;
            }
        } else {
            m_shape.fromAtlas(data.List());
            ret = 0;
        }
    }
    catch (Atlas::Message::WrongTypeException e) {
    }
    return ret;
}

template<>
void MathShape<WFMath::AxisBox, 2>::toAtlas(MapType & data) const
{
    data["type"] = getType();
    data["points"] = m_shape.toAtlas();
}


///////////////////////////////////////////////////////////////////////

template<>
const char * MathShape<WFMath::Line, 2>::getType() const
{
    return "line";
}

template<>
void MathShape<WFMath::Line, 2>::scale(WFMath::CoordType factor)
{
    for (size_t i = 0; i < m_shape.numCorners(); ++i) {
        WFMath::Point<2> corner = m_shape.getCorner(i);
        m_shape.moveCorner(i, WFMath::Point<2>(corner.x() * factor,
                                               corner.y() * factor));
    }
}

///////////////////////////////////////////////////////////////////////

template<>
const char * MathShape<WFMath::Point, 2>::getType() const
{
    return "point";
}

template<>
bool MathShape<WFMath::Point, 2>::intersect(const WFMath::Point<2> & p) const
{
    return WFMath::Equal(m_shape, p);
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

    size_t n = m_shape.numCorners();
    for (size_t i = 0; i < n; ++i) {
        WFMath::Point<2> corner = m_shape.getCorner(i);
        WFMath::Point<2> corner2 = m_shape.getCorner((i + 1) % n);
        area += corner.x() * corner2.y();
        area -= corner.y() * corner2.x();
    }

   return std::fabs(area / 2.f);
}

template<>
void MathShape<WFMath::Polygon, 2>::scale(WFMath::CoordType factor)
{
    for (size_t i = 0; i < m_shape.numCorners(); ++i) {
        WFMath::Point<2> corner = m_shape.getCorner(i);
        m_shape.moveCorner(i, WFMath::Point<2>(corner.x() * factor,
                                               corner.y() * factor));
    }
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
        new_shape = new MathShape<WFMath::Polygon>;
    } else if (type == "line") {
        new_shape = new MathShape<WFMath::Line>;
    } else if (type == "ball") {
        new_shape = new MathShape<WFMath::Ball>;
    } else if (type == "rotbox") {
        new_shape = new MathShape<WFMath::RotBox>;
    } else if (type == "box") {
        new_shape = new MathShape<WFMath::AxisBox>;
    }
    if (new_shape != 0) {
        int res = new_shape->fromAtlas(data);
        if (res != 0) {
            delete new_shape;
            new_shape = 0;
        }
    }
    return new_shape;
}

template class MathShape<WFMath::AxisBox, 2>;
template class MathShape<WFMath::Ball, 2>;
template class MathShape<WFMath::Line, 2>;
template class MathShape<WFMath::Point, 2>;
template class MathShape<WFMath::Polygon, 2>;
template class MathShape<WFMath::RotBox, 2>;
