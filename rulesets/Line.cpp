// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2002 Alistair Riddoch
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

// $Id: Line.cpp,v 1.28 2006-10-26 00:48:09 alriddoch Exp $

#include "Line.h"

#include "LineProperty.h"

Line::Line(const std::string & id, long intId) : Line_parent(id, intId)
{
    // Default to a 0.1m cube
    m_location.setBBox(BBox(WFMath::Point<3>(0.f, 0.f, 0.f),
                            WFMath::Point<3>(0.1f, 0.1f, 0.1f)));

    m_properties["coords"] = new LineProperty(m_coords, a_line);
    m_properties["start_intersections"] = new Property<IdList>(m_startIntersections, a_line);
    m_properties["end_intersections"] = new Property<IdList>(m_endIntersections, a_line);
}

Line::~Line()
{
}
