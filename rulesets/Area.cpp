// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#include "Area.h"

#include "common/type_utils.h"

Area::Area(const std::string & id) : Thing(id)
{
    // Default to a 0.1m cube
    location.m_bBox = BBox(Vector3D(0.1, 0.1, 0.1));
}

Area::~Area()
{
}

const Element Area::get(const std::string & aname) const
{
    if (aname == "segments") {
        return idListAsObject(segments);
    }
    return Thing::get(aname);
}

void Area::set(const std::string & aname, const Element & attr)
{
    if ((aname == "segments") && attr.IsList()) {
        segments = idListFromAtlas(attr);
        update_flags |= a_area;
    } else {
        Thing::set(aname, attr);
    }
}

void Area::addToObject(Element::MapType & omap) const
{
    omap["segments"] = idListAsObject(segments);
}
