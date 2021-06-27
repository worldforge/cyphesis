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
#include "rules/BBoxProperty.h"

#include "rules/LocatedEntity.h"
#include <wfmath/atlasconv.h>


void ScaleProperty::apply(LocatedEntity& ent)
{
    auto bboxProp = ent.getPropertyClassFixed<BBoxProperty>();
    if (bboxProp) {
        bboxProp->updateBboxOnEntity(ent);
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
    } else {
        val = Atlas::Message::Element();
    }
    return 0;

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
        //Don't allow negative scale
        if (m_data.isValid()) {
            m_data.x() = std::max(0.0, m_data.x());
            m_data.y() = std::max(0.0, m_data.y());
            m_data.z() = std::max(0.0, m_data.z());
        }
    } else if (val.isNone()) {
        m_data.setValid(false);
    } else if (val.isNum()) {
        //Warn here. We don't accept single numbers. We could, but it would be confusing since the 'get' method always returns a list.
        //So we instead want to keep the API less confusing and forcing rule writers to supply lists.
        log(WARNING, "Trying to set a 'scale' property to a number. It only accepts list of numbers (even if it only contains one entry).");
    }
}

ScaleProperty* ScaleProperty::copy() const
{
    return new ScaleProperty(*this);
}

WFMath::AxisBox<3> ScaleProperty::scaledBbox(const LocatedEntity& entity)
{
    auto bboxProperty = entity.getPropertyClassFixed<BBoxProperty>();
    if (bboxProperty) {
        return scaledBbox(entity, *bboxProperty);
    }
    return {};
}

WFMath::AxisBox<3> ScaleProperty::scaledBbox(const LocatedEntity& entity, const BBoxProperty& bboxProperty)
{
    if (bboxProperty.data().isValid()) {
        auto scaleProperty = entity.getPropertyClassFixed<ScaleProperty>();
        if (scaleProperty && scaleProperty->data().isValid()) {
            auto& scale = scaleProperty->data();
            auto bbox = bboxProperty.data();
            bbox.lowCorner().x() *= scale.x();
            bbox.lowCorner().y() *= scale.y();
            bbox.lowCorner().z() *= scale.z();
            bbox.highCorner().x() *= scale.x();
            bbox.highCorner().y() *= scale.y();
            bbox.highCorner().z() *= scale.z();
            return bbox;
        } else {
            return bboxProperty.data();
        }
    }
    return {};
}

WFMath::AxisBox<3> ScaleProperty::scaledBbox(const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& scale)
{
    if (scale.isValid()) {
        auto scaledBbox = bbox;
        scaledBbox.lowCorner().x() *= scale.x();
        scaledBbox.lowCorner().y() *= scale.y();
        scaledBbox.lowCorner().z() *= scale.z();
        scaledBbox.highCorner().x() *= scale.x();
        scaledBbox.highCorner().y() *= scale.y();
        scaledBbox.highCorner().z() *= scale.z();
        return scaledBbox;
    } else {
        return bbox;
    }

}
