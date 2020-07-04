/*
 Copyright (C) 2018 Erik Ogenvik

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

#include "WorldTimeProperty.h"
#include "rules/simulation/BaseWorld.h"

WorldTimeProperty::WorldTimeProperty() = default;

WorldTimeProperty::~WorldTimeProperty() = default;


WorldTimeProperty * WorldTimeProperty::copy() const
{
    return new WorldTimeProperty(*this);
}

int WorldTimeProperty::get(Atlas::Message::Element& val) const
{
    val = BaseWorld::instance().getTimeAsSeconds();
    return 0;
}

void WorldTimeProperty::set(const Atlas::Message::Element& val)
{
    //Just ignore. Or should we allow the time for the base world to be set?
}
