// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#include "Area.h"

#include <common/type_utils.h>

using Atlas::Message::Object;

Area::Area()
{
    // Default to a 0.1m cube
    location.bBox = BBox(Vector3D(0.1, 0.1, 0.1));
}

Area::~Area()
{
}

const Object & Area::operator[](const std::string & aname)
{
    if (aname == "segments") {
        attributes[aname] = idListAsObject(segments);
    }
    return Thing::operator[](aname);
}

void Area::set(const std::string & aname, const Object & attr)
{
    if ((aname == "segments") && attr.IsList()) {
        segments = idListFromAtlas(attr);
    } else {
        Thing::set(aname, attr);
    }
}

void Area::addToObject(Atlas::Message::Object::MapType & omap) const
{
    omap["segments"] = idListAsObject(segments);
}
