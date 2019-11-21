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

#include "ClientPropertyManager.h"

#include "rules/ScaleProperty.h"
#include "rules/SolidProperty.h"
#include "rules/BBoxProperty.h"
#include "common/PropertyFactory.h"
#include "common/PropertyFactory_impl.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
ClientPropertyManager::ClientPropertyManager()
{
    installFactory(BBoxProperty::property_name, std::make_unique<PropertyFactory<BBoxProperty>>());
    installFactory(ScaleProperty::property_name, std::make_unique<PropertyFactory<ScaleProperty>>());
    installFactory(SolidProperty::property_name, std::make_unique<PropertyFactory<SolidProperty>>());
}

std::unique_ptr<PropertyBase> ClientPropertyManager::addProperty(const std::string& name,
                                                                 int type)
{
    auto I = m_propertyFactories.find(name);
    if (I == m_propertyFactories.end()) {
        switch (type) {
            case Element::TYPE_INT:
                return std::make_unique<Property<int>>();
            case Element::TYPE_FLOAT:
                return std::make_unique<Property<double>>();
            case Element::TYPE_STRING:
                return std::make_unique<Property<std::string>>();
            default:
                return std::make_unique<SoftProperty>();
        }
    } else {
        return I->second->newProperty();
    }
}
