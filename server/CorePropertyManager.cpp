// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

#include "CorePropertyManager.h"

#include "rulesets/LineProperty.h"

#include "common/types.h"
#include "common/PropertyFactory_impl.h"
#include "common/DynamicProperty_impl.h"

template class PropertyBuilder<Dynamic<LineProperty, CoordList> >;

CorePropertyManager::CorePropertyManager()
{
    m_propertyFactories["stamina"] = new PropertyBuilder<DynamicProperty<double> >;
    m_propertyFactories["coords"] = new PropertyBuilder<Dynamic<LineProperty, CoordList> >;
    m_propertyFactories["start_intersections"] = new PropertyBuilder<DynamicProperty<IdList> >;
    m_propertyFactories["end_intersections"] = new PropertyBuilder<DynamicProperty<IdList> >;
}

CorePropertyManager::~CorePropertyManager()
{
}

int CorePropertyManager::addProperty(Entity * entity, const std::string & name)
{
    return -1;
}
