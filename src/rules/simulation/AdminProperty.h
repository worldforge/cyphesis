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

#ifndef CYPHESIS_ADMINPROPERTY_H
#define CYPHESIS_ADMINPROPERTY_H

#include "common/Property.h"

/**
 * Marks an entity as "admin", allowing it some extra capabilities.
 * 
 * This sets the LocatedEntity::entity_admin flag on the entity.
 * \ingroup PropertyClasses
 */
class AdminProperty : public BoolProperty
{
    public:
        static constexpr const char* property_name = "is_admin";

        explicit AdminProperty() = default;

        void apply(LocatedEntity& entity) override;
        AdminProperty * copy() const override;

    protected:
        AdminProperty(const AdminProperty& rhs) = default;

};


#endif //CYPHESIS_ADMINPROPERTY_H
