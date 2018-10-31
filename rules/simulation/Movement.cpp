// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#include "Movement.h"

#include "rules/Location.h"

Movement::Movement(LocatedEntity & body) : m_body(body),
                                           m_serialno(0)
{
}

bool Movement::updateNeeded(const Location & location) const
{
    return (location.velocity().isValid() && location.velocity() != Vector3D::ZERO());
}

void Movement::reset()
{
    ++m_serialno;
    m_targetPos.setValid(false);
}
