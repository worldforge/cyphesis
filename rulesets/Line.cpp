// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

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
