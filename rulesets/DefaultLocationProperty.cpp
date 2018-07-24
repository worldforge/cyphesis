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

#include "DefaultLocationProperty.h"
#include "BaseWorld.h"


DefaultLocationProperty * DefaultLocationProperty::copy() const
{
    return new DefaultLocationProperty(*this);
}

void DefaultLocationProperty::install(LocatedEntity * ent, const std::string & name)
{
    BaseWorld::instance().setDefaultLocation(ent);
}

void DefaultLocationProperty::remove(LocatedEntity * ent, const std::string & name)
{
    if (&BaseWorld::instance().getDefaultLocation() == ent) {
        BaseWorld::instance().setDefaultLocation(nullptr);
    }
}
