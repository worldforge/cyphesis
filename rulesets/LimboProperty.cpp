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

#ifdef HAVE_CONFIG_H
#endif

#include "LimboProperty.h"
#include "common/BaseWorld.h"

LimboProperty::LimboProperty()
{
}

LimboProperty * LimboProperty::copy() const
{
    return new LimboProperty(*this);
}

void LimboProperty::install(LocatedEntity * ent, const std::string & name)
{
    BaseWorld::instance().setLimboLocation(ent);
}

void LimboProperty::remove(LocatedEntity * ent, const std::string & name)
{
    if (BaseWorld::instance().getLimboLocation() == ent) {
        BaseWorld::instance().setLimboLocation(nullptr);
    }
}

