// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#include "Area.h"

#include "common/type_utils.h"
#include "common/debug.h"

static const bool debug_flag = false;

Area::Area(const std::string & id) : Area_parent(id)
{
    debug(std::cout << "Instanced Area" << std::endl << std::flush;);

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
        attr = ListType();
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

void Area::addToMessage(MapType & omap) const
{
    ListType & si =
        (omap["segments"] = ListType()).asList();
    idListasObject(m_segments, si);
    Area_parent::addToMessage(omap);
}
