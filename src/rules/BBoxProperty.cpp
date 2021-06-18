// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "BBoxProperty.h"

#include "rules/LocatedEntity.h"
#include "ScaleProperty.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

void BBoxProperty::apply(LocatedEntity& ent)
{
    updateBboxOnEntity(ent);
}

int BBoxProperty::get(Element& val) const
{
    if (m_data.isValid()) {
        val = m_data.toAtlas();
        return 0;
    }
    return -1;
}

void BBoxProperty::set(const Element& val)
{
    try {
        m_data.fromAtlas(val.asList());
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "BBoxProperty::set: Box bbox data");
    }
}

void BBoxProperty::add(const std::string& key,
                       MapType& map) const
{
    if (m_data.isValid()) {
        map[key] = m_data.toAtlas();
    }
}

void BBoxProperty::add(const std::string& key,
                       const RootEntity& ent) const
{
    if (m_data.isValid()) {
        ent->setAttr(key, m_data.toAtlas());
    }
}

BBoxProperty* BBoxProperty::copy() const
{
    return new BBoxProperty(*this);
}

void BBoxProperty::updateBboxOnEntity(LocatedEntity& entity) const
{
//    auto scaleProp = entity.getPropertyClassFixed<ScaleProperty>();
//    if (scaleProp && scaleProp->data().isValid()) {
//        auto& scale = scaleProp->data();
//        auto bbox = m_data;
//        bbox.lowCorner().x() *= scale.x();
//        bbox.lowCorner().y() *= scale.y();
//        bbox.lowCorner().z() *= scale.z();
//        bbox.highCorner().x() *= scale.x();
//        bbox.highCorner().y() *= scale.y();
//        bbox.highCorner().z() *= scale.z();
//        entity.m_location.setBBox(bbox);
//    } else {
//        entity.m_location.setBBox(m_data);
//    }
}
