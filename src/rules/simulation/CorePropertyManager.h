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


#ifndef SERVER_CORE_PROPERTY_MANAGER_H
#define SERVER_CORE_PROPERTY_MANAGER_H

#include <common/Inheritance.h>
#include "common/PropertyFactory.h"
#include "common/PropertyManager.h"
#include "common/Property.h"

/// \brief Property manager for the core server. Handles assigning properties
/// to entity instances in the world.
class CorePropertyManager : public PropertyManager {
  protected:
    template<typename T>
    PropertyFactory<Property<T>>* installBaseProperty(const std::string & type_name,
                                                      const std::string & parent);

    template<class PropertyT>
    PropertyFactory<PropertyT>* installProperty(const std::string & type_name,
                                                const std::string & parent);

    template<class PropertyT>
    PropertyFactory<PropertyT>* installProperty(const std::string & type_name);

    template<class PropertyT>
    PropertyFactory<PropertyT>* installProperty();

    Inheritance& m_inheritance;

  public:
    explicit CorePropertyManager(Inheritance& inheritance);
    ~CorePropertyManager() override;

    std::unique_ptr<PropertyBase> addProperty(const std::string & name) const override;

    int installFactory(const std::string& type_name,
                       const Atlas::Objects::Root& type_desc,
                       std::unique_ptr<PropertyKit> factory) override;

};


template<typename T>
PropertyFactory<Property<T>>* CorePropertyManager::installBaseProperty(const std::string& type_name,
                                                                       const std::string& parent)
{
    return this->installProperty<Property<T>>(type_name, parent);
}

template<typename PropertyT>
PropertyFactory<PropertyT>* CorePropertyManager::installProperty(const std::string& type_name,
                                                                 const std::string& parent)
{
    auto factory = new PropertyFactory<PropertyT>{};
    //Attach visibility flags. Properties that starts with "__" are private, "_" are protected and the rest are public.
    factory->m_flags = PropertyUtil::flagsForPropertyName(type_name);
    installFactory(type_name,
                   atlasType(type_name, parent),
                   std::unique_ptr<PropertyFactory<PropertyT>>(factory));
    return factory;
}

template<typename PropertyT>
PropertyFactory<PropertyT>* CorePropertyManager::installProperty(const std::string& type_name)
{
    return this->installProperty<PropertyT>(type_name, PropertyT::property_atlastype);
}

template<typename PropertyT>
PropertyFactory<PropertyT>* CorePropertyManager::installProperty()
{
    return this->installProperty<PropertyT>(PropertyT::property_name, PropertyT::property_atlastype);
}



#endif // SERVER_CORE_PROPERTY_MANAGER_H
