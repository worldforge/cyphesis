// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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


#include "AreaProperty.h"

#include <common/log.h>

#include "physics/Shape.h"


using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

AreaProperty::AreaProperty(const AreaProperty& other)
    : TerrainEffectorProperty(other),
      m_layer(other.m_layer)
{
    if (other.m_shape) {
        m_shape.reset(other.m_shape->copy());
    }
}

/// \brief AreaProperty constructor
///
/// @param flags Flags used to persist this property
AreaProperty::AreaProperty() : TerrainEffectorProperty(),
    m_layer(0)
{
}

AreaProperty::~AreaProperty() = default;

void AreaProperty::apply(LocatedEntity& owner)
{
    if (!m_shape) {
        log(ERROR, "Terrain area has no shape to apply");
        return;
    }

    const TerrainProperty* terrain = getTerrain(owner);

    if (!terrain) {
        log(ERROR, "Terrain area could not find terrain");
        return;
    }

    /// TODO: Write the code to handle all the Mercator stuff
}

void AreaProperty::set(const Element& ent)
{
    if (!ent.isMap()) {
        return;
    }
    m_data = ent.Map();

    MapType::const_iterator I = m_data.find("shape");
    MapType::const_iterator Iend = m_data.end();

    if (I == m_data.end() || !I->second.isMap()) {
        log(ERROR, "Area shape data missing or is not map");
        return;
    }
    auto shape = Shape::newFromAtlas(I->second.Map());
    auto formShape =dynamic_cast<Form<2>*>(shape.get());
    if (!formShape) {
        //Make sure we reset the data.
        m_shape.reset();
    } else {
        shape.release();
        m_shape.reset(formShape);
    }

    I = m_data.find("layer");
    if (I != Iend && I->second.isInt()) {
        m_layer = static_cast<int>(I->second.Int());
    } else {
        m_layer = 0;
    }
}

AreaProperty* AreaProperty::copy() const
{
    return new AreaProperty(*this);
}
