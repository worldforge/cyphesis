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

#include "AiClientPropertyManager.h"

#include "rules/BBoxProperty.h"
#include "common/PropertyFactory.h"
#include "common/PropertyFactory_impl.h"

AiClientPropertyManager::AiClientPropertyManager()
{
    PropertyManager::installFactory(BBoxProperty::property_atlastype, std::make_unique<PropertyFactory<BBoxProperty>>());
}

PropertyBase* AiClientPropertyManager::addProperty(const std::string& name,
                                                   int type)
{
    assert(!name.empty());
    assert(name != "objtype");
    PropertyBase* p = nullptr;
    auto I = m_propertyFactories.find(name);
    if (I != m_propertyFactories.end()) {
        p = I->second->newProperty();
    }
    return p;
}
