// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#include "Line.h"

using Atlas::Message::Object;

Line::Line()
{
    // Default to a 0.1m cube
    location.bBox = BBox(Vector3D(0.1, 0.1, 0.1));
}

Line::~Line()
{
}

const Object & Line::operator[](const std::string & aname)
{
    // FIXME: write code to convert vector to Object
    if (aname == "start_intersections") {
        attributes[aname] = Object::ListType(1,0.0);
    } else if (aname == "end_intersections") {
        attributes[aname] = Object::ListType(1,0.0);
    }
    return Thing::operator[](aname);
}

void Line::set(const std::string & aname, const Object & attr)
{
    // FIXME: write code to convert Object to vector
    if ((aname == "start_intersections") && attr.IsList()) {
        startIntersections = std::vector<Vector3D>();
    } else if ((aname == "end_intersections") && attr.IsList()) {
        startIntersections = std::vector<Vector3D>();
    } else {
        Thing::set(aname, attr);
    }
}

void Line::addToObject(Atlas::Message::Object::MapType & omap) const
{
    // FIXME: write code to convert vector to Object
    omap["start_intersections"] = Object::ListType(1,0.0);
    omap["end_intersections"] = Object::ListType(1,0.0);
}
