// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#include "Area.h"

#include "common/type_utils.h"

Area::Area(const std::string & id) : Area_parent(id)
{
    std::cout << "Instanced Area" << std::endl << std::flush;

    // Default to a 0.1m cube
    m_location.m_bBox = BBox(WFMath::Point<3>(0.f, 0.f, 0.f),
                             WFMath::Point<3>(0.1f, 0.1f, 0.1f));
    m_location.setSolid(false);
}

Area::~Area()
{
}

bool Area::get(const std::string & aname, Element & attr) const
{
    if (aname == "segments") {
        attr = Element::ListType();
        idListasObject(m_segments, attr.asList());
        return true;
    }
    return Area_parent::get(aname, attr);
}

void Area::set(const std::string & aname, const Element & attr)
{
    if ((aname == "segments") && attr.isList()) {
        idListFromAtlas(attr.asList(), m_segments);
        m_update_flags |= a_area;
    } else {
        Area_parent::set(aname, attr);
    }
}

void Area::addToObject(Element::MapType & omap) const
{
    Element::ListType & si =
        (omap["segments"] = Element::ListType()).asList();
    idListasObject(m_segments, si);
    Area_parent::addToObject(omap);
}
