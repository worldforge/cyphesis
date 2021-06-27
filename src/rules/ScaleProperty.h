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

#ifndef CYPHESIS_SCALEPROPERTY_H
#define CYPHESIS_SCALEPROPERTY_H


#include "rules/Vector3Property.h"

class BBoxProperty;

/**
 * \brief A property which defines the scaling of an entity, as a Vector<3>.
 *
 * If the list only contains one value the scaling is uniform.
 *
 * \ingroup PropertyClasses
 */
class ScaleProperty : public PropertyBase
{
    public:
        static constexpr const char* property_name = "scale";
        static constexpr const char* property_atlastype = "list";

        void apply(LocatedEntity&) override;

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element&) override;

        const WFMath::Vector<3>& data() const
        { return m_data; }

        WFMath::Vector<3>& data()
        { return m_data; }

        ScaleProperty* copy() const override;

        static WFMath::AxisBox<3> scaledBbox(const LocatedEntity& entity);

        static WFMath::AxisBox<3> scaledBbox(const LocatedEntity& entity, const BBoxProperty& bboxProperty);

        static WFMath::AxisBox<3> scaledBbox(const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& scale);

    protected:
        WFMath::Vector<3> m_data;


};


#endif //CYPHESIS_SCALEPROPERTY_H
