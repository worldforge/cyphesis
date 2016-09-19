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


const std::string TerrainModProperty::property_name = "terrainmod";
const std::string TerrainModProperty::property_atlastype = "map";

TerrainModProperty::TerrainModProperty()
{
}

TerrainModProperty::~TerrainModProperty()
{
}

TerrainModProperty * TerrainModProperty::copy() const
{
    return 0;
}

void TerrainModProperty::apply(LocatedEntity * owner)
{
}

void TerrainModProperty::install(LocatedEntity * ent, const std::string & name)
{
}

void TerrainModProperty::remove(LocatedEntity * ent, const std::string & name)
{
}

HandlerResult TerrainModProperty::operation(LocatedEntity *, const Operation &, OpVector &)
{
    return OPERATION_IGNORED;
}

void TerrainModProperty::move(LocatedEntity* owner)
{
}

void TerrainModProperty::remove(LocatedEntity * owner)
{
}



Mercator::TerrainMod * TerrainModProperty::parseModData(const WFMath::Point<3>&, const WFMath::Quaternion&) const
{
    return 0;
}

int TerrainModProperty::getAttr(const std::string & name,
                                Element & val)
{
    return 0;
}

void TerrainModProperty::setAttr(const std::string & name,
                                 const Element & val)
{
}

HandlerResult TerrainModProperty::move_handler(LocatedEntity * e,
                                               const Operation & op,
                                               OpVector & res)
{
    return OPERATION_IGNORED;
}

HandlerResult TerrainModProperty::delete_handler(LocatedEntity * e,
                                                 const Operation & op,
                                                 OpVector & res)
{
    return OPERATION_IGNORED;
}
