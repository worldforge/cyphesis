// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2002 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: Area.cpp,v 1.27 2006-10-26 00:48:07 alriddoch Exp $

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
