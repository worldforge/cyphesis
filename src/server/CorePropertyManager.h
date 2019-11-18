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

#include "common/PropertyFactory.h"
#include "common/PropertyManager.h"
#include "common/Property.h"

/// \brief Property manager for the core server. Handles assigning properties
/// to entity instances in the world.
class CorePropertyManager : public PropertyManager {
  private:
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

  public:
    CorePropertyManager();
    ~CorePropertyManager() override;

    std::unique_ptr<PropertyBase> addProperty(const std::string & name, int type) override;

    int installFactory(const std::string& type_name,
                       const Atlas::Objects::Root& type_desc,
                       std::unique_ptr<PropertyKit> factory) override;

    friend class CorePropertyManagertest;
};

#endif // SERVER_CORE_PROPERTY_MANAGER_H
