// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2015 Erik Ogenvik
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

#ifndef STUBPLANTABLEPROPERTY_H_
#define STUBPLANTABLEPROPERTY_H_

#include "rulesets/PlantableProperty.h"

PlantableProperty::PlantableProperty() :
        m_offset(0.f) {
}

int PlantableProperty::get(Atlas::Message::Element & ent) const {
    return 0;
}

void PlantableProperty::set(const Atlas::Message::Element & ent) {
}

PlantableProperty * PlantableProperty::copy() const {
    return new PlantableProperty(*this);
}

float PlantableProperty::getOffset() const
{
    return 0;
}

const Quaternion& PlantableProperty::getOrientation() const
{
    return m_orientation;
}
#endif /* STUBPLANTABLEPROPERTY_H_ */
