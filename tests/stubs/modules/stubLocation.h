/*
 Copyright (C) 2014 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include "modules/Location.h"

Location::Location() : m_loc(0)
{
}

Location::Location(LocatedEntity * rf, const Point3D & pos)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Location::addToMessage(Atlas::Message::MapType & omap) const
{
}

const Vector3D distanceTo(const Location & self, const Location & other)
{
    return *((Vector3D*)nullptr);
}

const Point3D relativePos(const Location & self, const Location & other)
{
    return *((Point3D*)nullptr);
}

float squareDistance(const Location & self, const Location & other)
{
    return 0.0f;
}
float squareHorizontalDistance(const Location & self, const Location & other)
{
    return 0.0f;
}
