// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#include "Area.h"

#include "common/type_utils.h"

Area::Area(const std::string & id) : Area_parent(id)
{
    // Default to a 0.1m cube
    m_location.m_bBox = BBox(WFMath::Point<3>(0.f, 0.f, 0.f),
                             WFMath::Point<3>(0.1f, 0.1f, 0.1f));
}

Area::~Area()
{
}

bool Area::get(const std::string & aname, Element & attr) const
{
    if (aname == "segments") {
        attr = Element::ListType();
        idListAsObject(m_segments, attr.AsList());
        return true;
    }
    return Area_parent::get(aname, attr);
}

void Area::set(const std::string & aname, const Element & attr)
{
    if ((aname == "segments") && attr.IsList()) {
        idListFromAtlas(attr.AsList(), m_segments);
        m_update_flags |= a_area;
    } else {
        Area_parent::set(aname, attr);
    }
}

void Area::addToObject(Element::MapType & omap) const
{
    Element::ListType & si =
        (omap["segments"] = Element::ListType()).AsList();
    idListAsObject(m_segments, si);
    Area_parent::addToObject(omap);
}
