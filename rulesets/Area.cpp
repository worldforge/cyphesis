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

bool Area::get(const std::string & aname, Element & attr) const
{
    if (aname == "segments") {
        attr = Element::ListType();
        idListAsObject(segments, attr.AsList());
        return true;
    }
    return Thing::get(aname, attr);
}

void Area::set(const std::string & aname, const Element & attr)
{
    if ((aname == "segments") && attr.IsList()) {
        idListFromAtlas(attr.AsList(), segments);
        update_flags |= a_area;
    } else {
        Thing::set(aname, attr);
    }
}

void Area::addToObject(Element::MapType & omap) const
{
    Element::ListType & si =
        (omap["segments"] = Element::ListType()).AsList();
    idListAsObject(segments, si);
}
