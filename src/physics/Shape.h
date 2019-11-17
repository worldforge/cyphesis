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


#ifndef PHYSICS_SHAPE_H
#define PHYSICS_SHAPE_H

#include <wfmath/const.h>

#include <map>
#include <string>
#include <memory>

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::map<std::string, Element> MapType;
    }
}

/// \brief Shape interface for inheritance based use of wfmath shapes
class Shape {
  protected:
    Shape(const Shape &) = default;
    Shape & operator=(const Shape &) = default;

    Shape();
  public:
    virtual ~Shape() = 0;

    virtual size_t size() const = 0;
    virtual bool isValid() const = 0;
    virtual WFMath::CoordType area() const = 0;
    virtual WFMath::AxisBox<2> footprint() const = 0;
    virtual WFMath::Polygon<2> outline(WFMath::CoordType p = 1.f) const = 0;

    virtual void scale(WFMath::CoordType factor) = 0;

    virtual bool equal(const Shape & other) const = 0;

    virtual Shape * copy() const = 0;

    virtual void toAtlas(Atlas::Message::MapType &) const = 0;
    virtual int fromAtlas(const Atlas::Message::Element &) = 0;

    virtual void stream(std::ostream &) const = 0;

    /// \brief Name constructor
    static std::unique_ptr<Shape> newFromAtlas(const Atlas::Message::MapType &);
};

template <int dim>
class Form;

template <>
class Form<2> : public Shape {
  public:
    virtual WFMath::Point<2> getCorner(size_t) const = 0;
    virtual WFMath::Point<2> centre() const = 0;
    virtual WFMath::Point<2> lowCorner() const = 0;
    virtual WFMath::Point<2> highCorner() const = 0;
    virtual bool intersect(const WFMath::Point<2> &) const = 0;

    Form<2> * copy() const override = 0;
};

template <>
class Form<3> : public Shape {
  public:
    virtual WFMath::Point<3> getCorner(size_t) const = 0;
    virtual WFMath::Point<3> centre() const = 0;
    virtual WFMath::Point<3> lowCorner() const = 0;
    virtual WFMath::Point<3> highCorner() const = 0;
    virtual bool intersect(const WFMath::Point<3> &) const = 0;

    Form<2> * copy() const override = 0;
};

typedef Form<2> Area;
typedef Form<3> Body;

template<template <int> class ShapeT, int dim = 2>
class MathShape : public Form<dim> {
  protected:
    ShapeT<dim> m_shape;

    MathShape(const MathShape &) = default;
    MathShape & operator=(const MathShape &) = default;

    const char * getType() const;
  public:
    MathShape();

    explicit MathShape(const ShapeT<dim> &);

    virtual size_t size() const;
    virtual bool isValid() const;
    virtual WFMath::Point<dim> getCorner(size_t) const;
    virtual WFMath::CoordType area() const;
    virtual WFMath::Point<dim> centre() const;
    virtual WFMath::AxisBox<2> footprint() const;
    virtual WFMath::Polygon<2> outline(WFMath::CoordType precision) const;
    virtual WFMath::Point<dim> lowCorner() const;
    virtual WFMath::Point<dim> highCorner() const;
    virtual bool intersect(const WFMath::Point<dim> &) const;

    virtual void scale(WFMath::CoordType factor);

    virtual bool equal(const Shape & other) const;

    virtual MathShape<ShapeT, dim> * copy() const;

    virtual void toAtlas(Atlas::Message::MapType &) const;
    virtual int fromAtlas(const Atlas::Message::Element &);

    virtual void stream(std::ostream &) const;

    const ShapeT<dim> & shape() { return m_shape; }
};


template<template <int> class ShapeT, int dim>
inline MathShape<ShapeT, dim>::MathShape() = default;

template<template <int> class ShapeT, int dim>
inline MathShape<ShapeT, dim>::MathShape(const ShapeT<dim> & s) : m_shape(s)
{
}

inline bool operator==(const Shape & lhs, const Shape & rhs)
{
    return lhs.equal(rhs);
}

inline bool operator!=(const Shape & lhs, const Shape & rhs)
{
    return !lhs.equal(rhs);
}

inline std::ostream & operator<<(std::ostream& os, const Shape & s)
{
    s.stream(os);
    return os;
}

#endif // PHYSICS_SHAPE_H
