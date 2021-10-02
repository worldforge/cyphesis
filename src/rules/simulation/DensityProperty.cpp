// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2016 Erik Ogenvik
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

#include "DensityProperty.h"

#include "rules/LocatedEntity.h"

#include "common/debug.h"

#include <Atlas/Objects/Anonymous.h>

#include <iostream>
#include <rules/BBoxProperty.h>

using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Set;

static const bool debug_flag = false;


void DensityProperty::apply(LocatedEntity& entity)
{
    updateMass(entity);
}

void DensityProperty::updateMass(LocatedEntity& entity) const
{
    auto bboxProp = entity.getPropertyClassFixed<BBoxProperty>();
    if (bboxProp && bboxProp->data().isValid()) {
        auto& bbox = bboxProp->data();
        WFMath::Vector<3> volumeVector = bbox.highCorner() - bbox.lowCorner();
        float volume = volumeVector.x() * volumeVector.y() * volumeVector.z();

        if (!std::isnormal(volume) && volume != 0) {
            log(WARNING, String::compose("Volume of %1 is not a normal number.", volume));
        } else {
            double mass = volume * m_data;

            auto& massProp = entity.requirePropertyClass<Property<double>>("mass", mass);

            if (massProp.data() != mass) {
                massProp.set(mass);
                massProp.apply(entity);
                massProp.removeFlags(prop_flag_persistence_clean);
                massProp.addFlags(prop_flag_unsent);
                entity.propertyApplied("mass", massProp);
            }
        }
    }
}

DensityProperty* DensityProperty::copy() const
{
    return new DensityProperty(*this);
}

