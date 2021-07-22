/*
 Copyright (C) 2020 Erik Ogenvik

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

#ifndef CYPHESIS_ALIASPROPERTY_H
#define CYPHESIS_ALIASPROPERTY_H

#include "common/Property.h"

/**
 * Registers the entity to a certain "alias" in the system, allow rules and scripts to refer to it.
 *
 * The alias is global for the whole system.
 * \ingroup PropertyClasses
 */
class AliasProperty : public Property<std::string>
{
    public:
        static constexpr const char* property_name = "__alias";

        AliasProperty() = default;

        AliasProperty* copy() const override;

        void apply(LocatedEntity& entity) override;

        void remove(LocatedEntity&, const std::string& name) override;

        protected:
            AliasProperty(const AliasProperty& rhs) = default;

};


#endif //CYPHESIS_ALIASPROPERTY_H
