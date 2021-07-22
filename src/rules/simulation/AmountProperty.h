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

#ifndef CYPHESIS_AMOUNTPROPERTY_H
#define CYPHESIS_AMOUNTPROPERTY_H

#include "common/Property.h"

/**
 * Used together with the "stackable" domain to handle amount of stacked entities.
 * \ingroup PropertyClasses
 */
class AmountProperty : public Property<int>
{
    public:
        static constexpr const char* property_name = "amount";

        AmountProperty() = default;

        AmountProperty* copy() const override;

        void apply(LocatedEntity& entity) override;

    protected:
        AmountProperty(const AmountProperty& rhs) = default;

};


#endif //CYPHESIS_AMOUNTPROPERTY_H
