// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#include "Area.h"

#include "common/Property.h"
#include "common/debug.h"

static const bool debug_flag = false;

Area::Area(const std::string & id, long intId) : Area_parent(id, intId)
{
    debug(std::cout << "Instanced Area" << std::endl << std::flush;);

    // Default to a 0.1m cube
    m_location.setBBox(BBox(WFMath::Point<3>(0.f, 0.f, 0.f),
                            WFMath::Point<3>(0.1f, 0.1f, 0.1f)));
    m_location.setSolid(false);

    m_properties["segments"] = new Property<IdList>(m_segments, a_area);
}

Area::~Area()
{
}
