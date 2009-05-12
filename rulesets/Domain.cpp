// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

// $Id$

#include "Domain.h"
#include "World.h"

#include "common/debug.h"

#include <iostream>

#include <cassert>

static const bool debug_flag = false;

Domain * Domain::m_instance = new Domain();

Domain::Domain() : m_refCount(0)
{
}

Domain::~Domain()
{
}

float Domain::constrainHeight(LocatedEntity * parent,
                              const Point3D & pos,
                              const std::string & mode)
{
    assert(parent != 0);
    World * wrld = dynamic_cast<World*>(parent);
    if (wrld != 0) {
        float h;
        h = wrld->getHeight(pos.x(), pos.y());
        // FIXME Use a virtual movement_domain function to get the constraints
        debug(std::cout << "Fix height " << pos.z() << " to " << h
                        << std::endl << std::flush;);
        return h;
    } else {
        static const Quaternion identity(Quaternion().identity());
        assert(parent->m_location.m_loc != 0);
        const Point3D & ppos = parent->m_location.pos();
        debug(std::cout << "parent " << parent->getId() << " of type "
                        << parent->getType() << " pos " << ppos.z()
                        << " my pos " << pos.z()
                        << std::endl << std::flush;);
        float h;
        const Quaternion & parent_orientation = parent->m_location.orientation().isValid() ? parent->m_location.orientation() : identity;
        h =  constrainHeight(parent->m_location.m_loc,
                             pos.toParentCoords(parent->m_location.pos(),
                                                parent_orientation),
                             mode) - ppos.z();
        debug(std::cout << "Correcting height from " << pos.z() << " to " << h
                        << std::endl << std::flush;);
        return h;
    }
}
