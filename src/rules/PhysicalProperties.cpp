/*
 Copyright (C) 2021 Erik Ogenvik

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

#include "PhysicalProperties.h"
#include "rules/LocatedEntity.h"

PositionProperty* PositionProperty::copy() const
{
    return new PositionProperty(*this);
}

WFMath::Point<3> PositionProperty::extractPosition(const LocatedEntity& entity)
{
    auto posProp = entity.getPropertyClassFixed<PositionProperty>();
    if (posProp) {
        return posProp->data();
    }
    return {};
}

VelocityProperty* VelocityProperty::copy() const
{
    return new VelocityProperty(*this);
}

AngularVelocityProperty* AngularVelocityProperty::copy() const
{
    return new AngularVelocityProperty(*this);
}

OrientationProperty* OrientationProperty::copy() const
{
    return new OrientationProperty(*this);
}
