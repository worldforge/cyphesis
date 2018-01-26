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

#ifndef CYPHESIS_GEOMETRYFILEPROPERTY_H
#define CYPHESIS_GEOMETRYFILEPROPERTY_H

#include "common/Property.h"

#include <Atlas/Message/Element.h>

class GeometryFileProperty : public Property<Atlas::Message::StringType>
{
    public:
        static const std::string property_name;
        static const std::string property_atlastype;

        GeometryFileProperty() = default;

        ~GeometryFileProperty() override = default;

        void set(const Atlas::Message::Element&) override;

        void apply(LocatedEntity *) override;

        GeometryFileProperty* copy() const override;


    private:

        Atlas::Message::MapType m_geometryProperty;

};


#endif //CYPHESIS_GEOMETRYFILEPROPERTY_H
