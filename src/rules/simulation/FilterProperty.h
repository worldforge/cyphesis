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

#ifndef CYPHESIS_FILTERPROPERTY_H
#define CYPHESIS_FILTERPROPERTY_H

#include "rules/entityfilter/Filter.h"
#include "common/Property.h"

/**
 * @brief Adds an Entity Filter.
 *
 * This property creates a Filter which can be queried. It's not tied to a specific property name.
 *
 * \ingroup PropertyClasses
 */
class FilterProperty : public PropertyBase
{
    public:
        static constexpr const char* property_atlastype = "string";

        explicit FilterProperty() = default;

        FilterProperty* copy() const override;

        const EntityFilter::Filter* getData() const;

        int get(Atlas::Message::Element& val) const override;

        /// \brief Read the value of the property from an Atlas Message
        void set(const Atlas::Message::Element& val) override;

    protected:

        FilterProperty(const FilterProperty& rhs);

        std::unique_ptr<EntityFilter::Filter> m_data;
};


#endif //CYPHESIS_FILTERPROPERTY_H
