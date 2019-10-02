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


#ifndef PHYSICS_SHAPE_IMPL_H
#define PHYSICS_SHAPE_IMPL_H

#include "Shape.h"

#include <Atlas/Message/Element.h>

#include <wfmath/atlasconv.h>

template<int dim, template <int> class PathT>
class Course;

// Placeholders, as WFMath hasn't filled these out for line yet.
namespace WFMath {

template<int dim>
bool Intersect(const Line<dim>&, const Point<dim>&, bool)
{
    return false;
}

template<int dim, template <int> class PathT>
bool Intersect(const Course<dim, PathT>&, const Point<dim>&, bool)
{
    return false;
}

}

template<template <int> class ShapeT, int dim>
const char * MathShape<ShapeT, dim>::getType() const
{
    return "unknown";
}

template<template <int> class ShapeT, int dim>
size_t MathShape<ShapeT, dim>::size() const
{
    return m_shape.numCorners();
}

template<template <int> class ShapeT, int dim>
bool MathShape<ShapeT, dim>::isValid() const
{
    return m_shape.isValid();
}

template<template <int> class ShapeT, int dim>
WFMath::Point<dim> MathShape<ShapeT, dim>::getCorner(size_t i) const
{
    return m_shape.getCorner(i);
}

template<template <int> class ShapeT, int dim>
WFMath::CoordType MathShape<ShapeT, dim>::area() const
{
    return 1.f;
}

template<template <int> class ShapeT, int dim>
WFMath::Point<dim> MathShape<ShapeT, dim>::centre() const
{
    return m_shape.getCenter();
}

template<template <int> class ShapeT, int dim>
WFMath::AxisBox<2> MathShape<ShapeT, dim>::footprint() const
{
    return m_shape.boundingBox();
}

template<template <int> class ShapeT, int dim>
WFMath::Polygon<2> MathShape<ShapeT, dim>::outline(WFMath::CoordType precision) const
{
    size_t n = m_shape.numCorners();
    WFMath::Polygon<2> shape_outline;
    for (size_t i = 0; i < n; ++i) {
        shape_outline.addCorner(i, getCorner(i));
    }
    return shape_outline;
}

template<template <int> class ShapeT, int dim>
WFMath::Point<dim> MathShape<ShapeT, dim>::lowCorner() const
{
    return m_shape.boundingBox().lowCorner();
}

template<template <int> class ShapeT, int dim>
WFMath::Point<dim> MathShape<ShapeT, dim>::highCorner() const
{
    return m_shape.boundingBox().highCorner();
}

template<template <int> class ShapeT, int dim>
bool MathShape<ShapeT, dim>::intersect(const WFMath::Point<dim> & p) const
{
    return WFMath::Intersect(m_shape, p, true);
}

template<template <int> class ShapeT, int dim>
void MathShape<ShapeT, dim>::scale(WFMath::CoordType)
{
}

template<template <int> class ShapeT, int dim>
bool MathShape<ShapeT, dim>::equal(const Shape & other) const
{
    auto rhs = dynamic_cast<const MathShape<ShapeT, dim> *>(&other);
    return rhs != nullptr && WFMath::Equal(this->m_shape, rhs->m_shape);
}

template<template <int> class ShapeT, int dim>
MathShape<ShapeT, dim> * MathShape<ShapeT, dim>::copy() const
{
    return new MathShape<ShapeT, dim>(*this);
}


template<template <int> class ShapeT, int dim>
void MathShape<ShapeT, dim>::toAtlas(Atlas::Message::MapType & data) const
{
    data["type"] = getType();
    size_t size = m_shape.numCorners();
    if (size > 0) {
        Atlas::Message::ListType points;
        for (size_t i = 0; i < size; ++i) {
            WFMath::Point<dim> corner = m_shape.getCorner(i);
            points.push_back(corner.toAtlas());
        }
        data["points"] = points;
    }
}

template<template <int> class ShapeT, int dim>
int MathShape<ShapeT, dim>::fromAtlas(const Atlas::Message::Element & data)
{
    int ret = -1;
    try {
        m_shape.fromAtlas(data);
        ret = 0;
    }
    catch (const Atlas::Message::WrongTypeException& e) {
    }
    return ret;
}

template<template <int> class ShapeT, int dim>
void MathShape<ShapeT, dim>::stream(std::ostream & o) const
{
    o << getType() << ": ";
    o << m_shape;
}

#endif // PHYSICS_SHAPE_IMPL_H
