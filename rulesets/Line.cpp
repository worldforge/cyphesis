// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#include "Line.h"

#include <common/type_utils.h>
#include <common/debug.h>

static const bool debug_flag = true;

using Atlas::Message::Object;

Line::Line()
{
    // Default to a 0.1m cube
    location.bBox = BBox(Vector3D(0.1, 0.1, 0.1));
}

Line::~Line()
{
}

const Object Line::get(const std::string & aname) const
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

void Line::set(const std::string & aname, const Object & attr)
{
    debug( std::cout << "Setting " << aname << " in line" << std::endl
                     << std::flush;);
    if ((aname == "start_intersections") && attr.IsList()) {
        startIntersections = idListFromAtlas(attr);
    } else if ((aname == "end_intersections") && attr.IsList()) {
        startIntersections = idListFromAtlas(attr);
    } else if ((aname == "coords") && attr.IsList()) {
        coords = coordListFromAtlas(attr);
    } else {
        Thing::set(aname, attr);
    }
}

void Line::addToObject(Atlas::Message::Object::MapType & omap) const
{
    omap["start_intersections"] = idListAsObject(startIntersections);
    omap["end_intersections"] = idListAsObject(endIntersections);
    omap["coords"] = coordListAsObject(coords);
}
