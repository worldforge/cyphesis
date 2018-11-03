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

#ifndef CYPHESIS_SIMPLEPROPERTY_H
#define CYPHESIS_SIMPLEPROPERTY_H

#include "common/Property.h"

/// \brief Class to handle whether or not an entity is simple for collisions.
/// \ingroup PropertyClasses
class SimpleProperty : public BoolProperty {
    public:
        static constexpr const char* property_name = "simple";

        /// \brief Constructor
        ///
        /// @param owner the owner of the property.
        explicit SimpleProperty() = default;

        SimpleProperty * copy() const override;

        void apply(LocatedEntity *) override;
};



#endif //CYPHESIS_SIMPLEPROPERTY_H
