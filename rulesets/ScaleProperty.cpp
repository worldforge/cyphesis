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

#include "ScaleProperty.h"
#include "BBoxProperty.h"

#include "LocatedEntity.h"
#include <wfmath/atlasconv.h>


void ScaleProperty::apply(LocatedEntity * ent)
{
    auto bboxProp = ent->getPropertyClassFixed<BBoxProperty>();
    if (bboxProp) {
        bboxProp->updateBboxOnEntity(ent);
        //ent->propertyApplied(BBoxProperty::property_name, *bboxProp);
    }
}

int ScaleProperty::get(Atlas::Message::Element& val) const
{
    if (m_data.isValid()) {
        if (m_data.x() == m_data.y() && m_data.x() == m_data.z()) {
            val = Atlas::Message::ListType{m_data.x()};
        } else {
            val = m_data.toAtlas();
        }
        return 0;
    }
    return 1;

}

void ScaleProperty::set(const Atlas::Message::Element& val)
{
    if (val.isList()) {
        if (val.List().size() == 1) {
            if (val.List().front().isNum()) {
                auto num = val.List().front().asNum();
                m_data = WFMath::Vector<3>(num, num, num);
            }
        } else {
            m_data.fromAtlas(val.List());
        }
    }
}

ScaleProperty* ScaleProperty::copy() const
{
    return new ScaleProperty(*this);
}
