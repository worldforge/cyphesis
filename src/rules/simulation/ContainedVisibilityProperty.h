/*
 Copyright (C) 2019 Erik Ogenvik

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

#ifndef CYPHESIS_CONTAINEDVISIBILITYPROPERTY_H
#define CYPHESIS_CONTAINEDVISIBILITYPROPERTY_H

#include "common/Property.h"


/**
 * Marks the entity using the entity_contained_visible flag, which means that it's meant
 * to be visible when contained, even if the normal contain rules would disallow it.
 * This is to be used on things such as fires and effects.
 * \ingroup PropertyClasses
 */
class ContainedVisibilityProperty : public BoolProperty
{
    public:
        static constexpr const char* property_name = "contained_visibility";

        explicit ContainedVisibilityProperty() = default;

        ContainedVisibilityProperty* copy() const override;

        void apply(LocatedEntity&) override;

        void remove(LocatedEntity&, const std::string&) override;

    protected:
        ContainedVisibilityProperty(const ContainedVisibilityProperty& rhs) = default;
};


#endif //CYPHESIS_CONTAINEDVISIBILITYPROPERTY_H
