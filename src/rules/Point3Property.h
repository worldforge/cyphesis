/*
 Copyright (C) 2021 Erik Ogenvik

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

#ifndef CYPHESIS_POINT3PROPERTY_H
#define CYPHESIS_POINT3PROPERTY_H

#include "common/Property.h"
#include <wfmath/point.h>

/**
 * \brief A property containing a WFMath::Point<3>.
 *
 * \ingroup PropertyClasses
 */
class Point3Property : public PropertyBase
{
    public:

        static constexpr const char* property_atlastype = "list";

        explicit Point3Property() = default;

        Point3Property(const Point3Property& rhs);

        ~Point3Property() override = default;

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element&) override;

        void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const override;

        const WFMath::Point<3>& data() const
        { return m_data; }

        WFMath::Point<3>& data()
        { return m_data; }

        Point3Property* copy() const override;

    protected:
        WFMath::Point<3> m_data;
};


#endif //CYPHESIS_POINT3PROPERTY_H
