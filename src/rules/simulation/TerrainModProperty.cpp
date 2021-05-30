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

#include "TerrainModProperty.h"

#include "rules/LocatedEntity.h"
#include "TerrainModTranslator.h"
#include "TerrainProperty.h"
#include "rules/Domain.h"

#include "common/debug.h"

#include "modules/TerrainContext.h"

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Operation.h>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Root;

/// \brief TerrainModProperty constructor
///
TerrainModProperty::TerrainModProperty() = default;

TerrainModProperty::TerrainModProperty(const TerrainModProperty& rhs) : TerrainEffectorProperty(rhs)
{
    if (rhs.m_translator) {
        m_translator = std::make_unique<TerrainModTranslator>(*rhs.m_translator);
    }
}


TerrainModProperty::~TerrainModProperty() = default;

TerrainModProperty* TerrainModProperty::copy() const
{
    // This is for instantiation of a class property.
    // This is complex here, as is it not yet clear if this
    // class can be a class property.
    return new TerrainModProperty(*this);
}

void TerrainModProperty::apply(LocatedEntity& owner)
{
    m_translator = std::make_unique<TerrainModTranslator>(m_data);
}

std::unique_ptr<Mercator::TerrainMod> TerrainModProperty::parseModData(const WFMath::Point<3>& pos, const WFMath::Quaternion& orientation) const
{
    if (m_translator) {
        return m_translator->parseData(pos, orientation);
    }
    return nullptr;
}

int TerrainModProperty::getAttr(const std::string& name, Element& val) const
{
    auto I = m_data.find(name);
    if (I != m_data.end()) {
        val = I->second;
        return 0;
    }
    return -1;
}

void TerrainModProperty::setAttr(const std::string& name, const Element& val)
{
    m_data[name] = val;
}

