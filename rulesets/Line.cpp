// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#include "Line.h"

#include "common/type_utils.h"
#include "common/debug.h"

static const bool debug_flag = true;

Line::Line(const std::string & id) : Line_parent(id)
{
    // Default to a 0.1m cube
    m_location.m_bBox = BBox(WFMath::Point<3>(0.f, 0.f, 0.f),
                             WFMath::Point<3>(0.1f, 0.1f, 0.1f));
}

Line::~Line()
{
}

bool Line::get(const std::string & aname, Element & attr) const
{
    if (aname == "start_intersections") {
        attr = Element::ListType();
        idListasObject(startIntersections, attr.asList());
        return true;
    } else if (aname == "end_intersections") {
        attr = Element::ListType();
        idListasObject(endIntersections, attr.asList());
        return true;
    } else if (aname == "coords") {
        attr = Element::ListType();
        objectListAsMessage(coords, attr.asList());
        return true;
    }
    return Line_parent::get(aname, attr);
}

void Line::set(const std::string & aname, const Element & attr)
{
    debug( std::cout << "Setting " << aname << " in line" << std::endl
                     << std::flush;);
    if ((aname == "start_intersections") && attr.isList()) {
        idListFromAtlas(attr.asList(), startIntersections);
        m_update_flags |= a_line;
    } else if ((aname == "end_intersections") && attr.isList()) {
        idListFromAtlas(attr.asList(), endIntersections);
        m_update_flags |= a_line;
    } else if ((aname == "coords") && attr.isList()) {
        objectListFromMessage<Vector3D, CoordList>(attr.asList(), coords);
        m_update_flags |= a_line;
    } else {
        Line_parent::set(aname, attr);
    }
}

void Line::addToObject(Element::MapType & omap) const
{
    Element::ListType & si =
        (omap["start_intersections"] = Element::ListType()).asList();
    idListasObject(startIntersections, si);
    Element::ListType & ei =
        (omap["end_intersections"] = Element::ListType()).asList();
    idListasObject(endIntersections, ei);
    Element::ListType & c = (omap["coords"] = Element::ListType()).asList();
    objectListAsMessage(coords, c);
    Line_parent::addToObject(omap);
}
