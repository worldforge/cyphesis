// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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


#ifndef PHYSICS_COURSE_H
#define PHYSICS_COURSE_H

#include <wfmath/const.h>

#include <cstring>

/// Class describing the course of a world feature
///
/// Roads, rivers and related features have a course described
/// by a linear path, and additional variable information
/// such as the width.
template<int dim, template <int> class PathT>
class Course {
  protected:
    PathT<dim> m_path;
  public:
        /// generic: check if two classes are equal, up to a given tolerance
        bool isEqualTo(const Course& c, WFMath::CoordType epsilon =
        WFMath::numeric_constants<WFMath::CoordType>::epsilon()) const;

    ///
    Course();
    ///
    Course(const Course& l);
    ///
    explicit Course(const PathT<dim> & l);
    ///
    ~Course();

    /// Create an Atlas object from the course
    WFMath::AtlasOutType toAtlas() const;
    /// Set the course's value to that given by an Atlas object
    void fromAtlas(const WFMath::AtlasInType & a);

    ///
    Course& operator=(const Course& l);


    ///
    bool operator==(const Course& s) const {return isEqualTo(s);}
    ///
    bool operator!=(const Course& s) const {return !isEqualTo(s);}

    bool isValid() const {return m_path.isValid();}

    size_t numCorners() const {return m_path.numCorners();}
    WFMath::Point<dim> getCorner(size_t i) const {return m_path.getCorner(i);}
    WFMath::Point<dim> getCenter() const {return m_path.getCenter();}

    bool addCorner(size_t i, const WFMath::Point<dim>& p, WFMath::CoordType e =
          WFMath::numeric_constants<WFMath::CoordType>::epsilon()) {
      return m_path.addCorner(i, p, e);
    }

    void removeCorner(size_t i) {
      m_path.removeCorner(i);
    }

    bool moveCorner(size_t i, const WFMath::Point<dim>& p, WFMath::CoordType e =
          WFMath::numeric_constants<WFMath::CoordType>::epsilon()) {
      return m_path.moveCorner(i, p, e);
    }

    Course& shift(const WFMath::Vector<dim>& v) {
      m_path.shift(v);
      return *this;
    }

    Course& moveCornerTo(const WFMath::Point<dim>& p, size_t corner) {
      m_path.moveCornerTo(p, corner);
      return *this;
    }

    Course& moveCenterTo(const WFMath::Point<dim>& p) {
      m_path.moveCenterTo(p);
      return *this;
    }

    Course& rotateCorner(const WFMath::RotMatrix<dim>& m, size_t corner);

    Course& rotateCenter(const WFMath::RotMatrix<dim>& m);

    Course& rotatePoint(const WFMath::RotMatrix<dim>& m,
                        const WFMath::Point<dim>& p);

    WFMath::AxisBox<dim> boundingBox() const;
    WFMath::Ball<dim> boundingSphere() const;
    WFMath::Ball<dim> boundingShapeSloppy() const;
};

template<int dim, template <int> class PathT>
inline Course<dim, PathT>::Course()
{
}

template<int dim, template <int> class PathT>
inline Course<dim, PathT>::Course(const Course<dim, PathT>& l) :
      m_path(l.m_path)
{
}

template<int dim, template <int> class PathT>
inline Course<dim, PathT>::Course(const PathT<dim> & l) : m_path(l)
{
}

template<int dim, template <int> class PathT>
inline Course<dim, PathT>::~Course()
{
}

template<int dim, template <int> class PathT>
inline Course<dim, PathT>& Course<dim, PathT>::operator=(const Course<dim, PathT>& rhs)
{
    m_path = rhs.m_path;
    return *this;
}

#include <iosfwd>

template<int dim, template <int> class PathT>
inline std::ostream& operator<<(std::ostream& os,
                                const Course<dim, PathT>& rhs)
{
    return os;
}

#endif // PHYSICS_COURSE_H
