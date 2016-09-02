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

#include "rulesets/TerrainProperty.h"

TerrainProperty::TerrainProperty() :
      m_data(*(Mercator::Terrain*)0),
      m_tileShader(nullptr)
{
}

TerrainProperty::~TerrainProperty()
{
}

int TerrainProperty::get(Atlas::Message::Element & ent) const
{
    return 0;
}

void TerrainProperty::set(const Atlas::Message::Element & ent)
{
}

TerrainProperty * TerrainProperty::copy() const
{
    return 0;
}

int TerrainProperty::getSurface(const Point3D & pos, int & material)
{
    return 0;
}

void TerrainProperty::install(LocatedEntity*, std::string const&) {
}

void TerrainProperty::remove(LocatedEntity*, std::string const&) {
}

HandlerResult TerrainProperty::operation(LocatedEntity *,
                                const Operation &,
                                OpVector &)
{
    return OPERATION_BLOCKED;
}

bool TerrainProperty::getHeightAndNormal(float x,
                                         float y,
                                         float & height,
                                         Vector3D & normal) const
{
    return true;
}

void TerrainProperty::apply(LocatedEntity* entity) {

}

