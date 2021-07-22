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

#ifndef CYPHESIS_WORLDTIMEPROPERTY_H
#define CYPHESIS_WORLDTIMEPROPERTY_H

#include "common/Property.h"

/**
 * Keeps track of the world time.
 *
 * \ingroup PropertyClasses
 *
 */
class WorldTimeProperty : public PropertyBase
{
    public:
        static constexpr const char* property_name = "world_time";
        static constexpr const char* property_atlastype = "float";

        WorldTimeProperty();

        ~WorldTimeProperty() override;

        WorldTimeProperty* copy() const override;

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element& val) override;
    protected:
        WorldTimeProperty(const WorldTimeProperty& rhs) = default;

};


#endif //CYPHESIS_WORLDTIMEPROPERTY_H
