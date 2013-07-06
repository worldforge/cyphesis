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


#ifndef PHYSICS_COURSE_IMPL_H
#define PHYSICS_COURSE_IMPL_H

#include "Course.h"

#include <Atlas/Message/Element.h>

#include <wfmath/atlasconv.h>

template<int dim, template <int> class PathT>
void Course<dim, PathT>::fromAtlas(const WFMath::AtlasInType& a)
{
    m_path.fromAtlas(a);
}

template<int dim, template <int> class PathT>
bool Course<dim, PathT>::isEqualTo(const Course& c,
                                   WFMath::CoordType epsilon) const
{
    return m_path == c.m_path;
}

template<int dim, template <int> class PathT>
WFMath::AxisBox<dim> Course<dim, PathT>::boundingBox() const
{
    WFMath::AxisBox<dim> box = m_path.boundingBox();
    // FIXME increase by the size of the path
    return box;
}

#endif // PHYSICS_COURSE_IMPL_H
