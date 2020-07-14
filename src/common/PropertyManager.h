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


#ifndef COMMON_PROPERTY_MANAGER_H
#define COMMON_PROPERTY_MANAGER_H

#include "Singleton.h"
#include "Property.h"
#include <Atlas/Objects/ObjectsFwd.h>

#include <map>
#include <string>
#include <memory>

class PropertyKit;

/// \brief Base class for classes that handle creating Entity properties.
class PropertyManager : public Singleton<PropertyManager> {
  protected:
    // Data structure for factories and the like?
    std::map<std::string, std::unique_ptr<PropertyKit>> m_propertyFactories;

    PropertyManager();

    void installFactory(const std::string &,
                        std::unique_ptr<PropertyKit>);
  public:
    ~PropertyManager() override;

    /// \brief Add a new named property to an Entity
    ///
    /// @param name a string giving the name of the property.
    virtual std::unique_ptr<PropertyBase> addProperty(const std::string & name) const = 0;

    virtual int installFactory(const std::string & type_name,
                               const Atlas::Objects::Root & type_desc,
                               std::unique_ptr<PropertyKit> factory);

    PropertyKit * getPropertyFactory(const std::string &) const;

    const std::map<std::string, std::unique_ptr<PropertyKit>>& getPropertyFactories() const {
        return m_propertyFactories;
    }
};

#endif // COMMON_PROPERTY_MANAGER_H
