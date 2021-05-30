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

#include "AdminProperty.h"

#include "rules/LocatedEntity.h"

void AdminProperty::apply(LocatedEntity& entity)
{
    if (isTrue()) {
        entity.flags().addFlags(entity_admin);
    } else {
        entity.flags().removeFlags(entity_admin);
    }
}

AdminProperty * AdminProperty::copy() const
{
    return new AdminProperty(*this);
}

