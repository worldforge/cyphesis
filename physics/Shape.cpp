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


#include "Shape_impl.h"
#include "Course.h"

#include <wfmath/stream.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

using WFMath::CoordType;

using WFMath::AxisBox;
using WFMath::Ball;
using WFMath::Line;
using WFMath::Point;
using WFMath::Polygon;
using WFMath::RotBox;

using WFMath::numeric_constants;

template<int dim> class LinearCourse : public Course<dim, Line>
{
};

Shape::Shape()
{
}

Shape::~Shape()
{
}

//////////////////////////////// Ball /////////////////////////////////

template<>
Polygon<2> MathShape<Ball, 2>::outline(CoordType precision) const
{
    Polygon<2> shape_outline;
    CoordType radius = m_shape.radius();
    CoordType segments = radius * numeric_constants<CoordType>::pi() * 2.f / precision;
    // FIXME lrint this properly
    size_t count = (size_t)std::ceil(segments);
    CoordType seg = numeric_constants<CoordType>::pi() * 2.f / count;
    for (size_t i = 0; i < count; ++i) {
        shape_outline.addCorner(i, Point<2>(radius * std::cos(seg * i),
                                            radius * std::sin(seg * i)));
    }
    return shape_outline;
}

template<>
const char * MathShape<Ball, 2>::getType() const
{
    return "circle";
}

template<>
int MathShape<Ball, 2>::fromAtlas(const Element & data)
{
    int ret = -1;
    try {
        if (data.isMap()) {
            m_shape.fromAtlas(data.Map());
            ret = 0;
        }
    }
    catch (Atlas::Message::WrongTypeException e) {
    }
    return ret;
}

template<>
void MathShape<Ball, 2>::toAtlas(MapType & data) const
{
    Element e = m_shape.toAtlas();
    if (e.isMap()) {
        data = e.Map();
        data["type"] = getType();
    }
}

/////////////////////////////// Course ////////////////////////////////

template<>
Polygon<2> MathShape<LinearCourse, 2>::outline(CoordType precision) const
{
    Polygon<2> shape_outline;
    size_t count = m_shape.numCorners();
    if (count == 0) {
        shape_outline.resize(count * 2);
        for (size_t i = 0; i < count; ++i) {
            WFMath::Vector<2> dir(0,0);
            if (i != 0) {
                dir += m_shape.getCorner(i - 1) - m_shape.getCorner(i);
            }
            if (i < count - 1) {
                dir += m_shape.getCorner(i) - m_shape.getCorner(i + 1);
            }
            CoordType mag = dir.mag();
            dir /= mag;
            // FIXME Actually sort the outline
        }
    }
    return shape_outline;
}

template<>
const char * MathShape<LinearCourse, 2>::getType() const
{
    return "course";
}

////////////////////////////// AxisBox ////////////////////////////////

template<>
const char * MathShape<AxisBox, 2>::getType() const
{
    return "box";
}

template<>
int MathShape<AxisBox, 2>::fromAtlas(const Element & data)
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
            m_shape.fromAtlas(data.asList());
            ret = 0;
        }
    }
    catch (Atlas::Message::WrongTypeException e) {
    }
    return ret;
}

template<>
void MathShape<AxisBox, 2>::toAtlas(MapType & data) const
{
    data["type"] = getType();
    data["points"] = m_shape.toAtlas();
}

//////////////////////////////// Line /////////////////////////////////

template<>
const char * MathShape<Line, 2>::getType() const
{
    return "line";
}

template<>
void MathShape<Line, 2>::scale(CoordType factor)
{
    size_t count = m_shape.numCorners();
    for (size_t i = 0; i < count; ++i) {
        Point<2> corner = m_shape.getCorner(i);
        m_shape.moveCorner(i, Point<2>(corner.x() * factor,
                                       corner.y() * factor));
    }
}

/////////////////////////////// Point /////////////////////////////////

template<>
const char * MathShape<Point, 2>::getType() const
{
    return "point";
}

template<>
bool MathShape<Point, 2>::intersect(const Point<2> & p) const
{
    return WFMath::Equal(m_shape, p);
}

template<>
int MathShape<Point, 2>::fromAtlas(const Element & data)
{
    int ret = -1;
    try {
        if (data.isMap()) {
            const MapType & datamap = data.Map();
            MapType::const_iterator I = datamap.find("pos");
            if (I != datamap.end()) {
                m_shape.fromAtlas(I->second);
                ret = 0;
            }
        } else {
            m_shape.fromAtlas(data.asList());
            ret = 0;
        }
    }
    catch (Atlas::Message::WrongTypeException e) {
    }
    return ret;
}

template<>
void MathShape<Point, 2>::toAtlas(MapType & data) const
{
    Element e = m_shape.toAtlas();
    if (e.isList()) {
        data["pos"] = e.asList();
        data["type"] = getType();
    }
}

////////////////////////////// Polygon ////////////////////////////////

template<>
Polygon<2> MathShape<Polygon, 2>::outline(CoordType precision) const
{
    return m_shape;
}

template<>
const char * MathShape<Polygon, 2>::getType() const
{
    return "polygon";
}

template<>
CoordType MathShape<Polygon, 2>::area() const
{
    CoordType area = 0;

    size_t count = m_shape.numCorners();
    for (size_t i = 0; i < count; ++i) {
        Point<2> corner = m_shape.getCorner(i);
        Point<2> corner2 = m_shape.getCorner((i + 1) % count);
        area += corner.x() * corner2.y();
        area -= corner.y() * corner2.x();
    }

   return std::fabs(area / 2.f);
}

template<>
void MathShape<Polygon, 2>::scale(CoordType factor)
{
    for (size_t i = 0; i < m_shape.numCorners(); ++i) {
        Point<2> corner = m_shape.getCorner(i);
        m_shape.moveCorner(i, Point<2>(corner.x() * factor,
                                       corner.y() * factor));
    }
}

/////////////////////////////// RotBox ////////////////////////////////

template<>
const char * MathShape<RotBox, 2>::getType() const
{
    return "rotbox";
}

template<>
int MathShape<RotBox, 2>::fromAtlas(const Element & data)
{
    int ret = -1;
    try {
        if (data.isMap()) {
            m_shape.fromAtlas(data.Map());
            ret = 0;
        }
    }
    catch (Atlas::Message::WrongTypeException e) {
    }
    return ret;
}

template<>
void MathShape<RotBox, 2>::toAtlas(MapType & data) const
{
    Element e = m_shape.toAtlas();
    if (e.isMap()) {
        data = e.Map();
        data["type"] = getType();
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
        new_shape = new MathShape<Polygon>;
    } else if (type == "line") {
        new_shape = new MathShape<Line>;
    } else if (type == "circle") {
        new_shape = new MathShape<Ball>;
    } else if (type == "point") {
        new_shape = new MathShape<Point>;
    } else if (type == "rotbox") {
        new_shape = new MathShape<RotBox>;
    } else if (type == "box") {
        new_shape = new MathShape<AxisBox>;
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

template class MathShape<AxisBox, 2>;
template class MathShape<Ball, 2>;
template class MathShape<Line, 2>;
template class MathShape<Point, 2>;
template class MathShape<Polygon, 2>;
template class MathShape<RotBox, 2>;
template class MathShape<LinearCourse, 2>;
