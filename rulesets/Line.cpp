// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#include "Line.h"

#include "common/type_utils.h"
#include "common/debug.h"

static const bool debug_flag = true;

Line::Line(const std::string & id) : Thing(id)
{
    // Default to a 0.1m cube
    location.bBox = BBox(Vector3D(0.1, 0.1, 0.1));
}

Line::~Line()
{
}

const Fragment Line::get(const std::string & aname) const
{
    if (aname == "start_intersections") {
        return idListAsObject(startIntersections);
    } else if (aname == "end_intersections") {
        return idListAsObject(endIntersections);
    } else if (aname == "coords") {
        return coordListAsObject(coords);
    }
    return Thing::get(aname);
}

void Line::set(const std::string & aname, const Fragment & attr)
{
    debug( std::cout << "Setting " << aname << " in line" << std::endl
                     << std::flush;);
    if ((aname == "start_intersections") && attr.IsList()) {
        startIntersections = idListFromAtlas(attr);
        update_flags |= a_line;
    } else if ((aname == "end_intersections") && attr.IsList()) {
        startIntersections = idListFromAtlas(attr);
        update_flags |= a_line;
    } else if ((aname == "coords") && attr.IsList()) {
        coords = coordListFromAtlas(attr);
        update_flags |= a_line;
    } else {
        Thing::set(aname, attr);
    }
}

void Line::addToObject(Fragment::MapType & omap) const
{
    omap["start_intersections"] = idListAsObject(startIntersections);
    omap["end_intersections"] = idListAsObject(endIntersections);
    omap["coords"] = coordListAsObject(coords);
}
