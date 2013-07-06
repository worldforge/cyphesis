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

#include <Atlas/Objects/ObjectsFwd.h>

#include <map>
#include <string>

class PropertyBase;
class PropertyKit;

typedef std::map<std::string, PropertyKit *> PropertyFactoryDict;

/// \brief Base class for classes that handle creating Entity properties.
class PropertyManager {
  protected:
    // Data structure for factories and the like?
    std::map<std::string, PropertyKit *> m_propertyFactories;

    /// \brief Singleton instance pointer for any subclass
    static PropertyManager * m_instance;

    PropertyManager();

    void installFactory(const std::string &,
                        PropertyKit *);
  public:
    virtual ~PropertyManager();

    /// \brief Add a new named property to an Entity
    ///
    /// @param name a string giving the name of the property.
    virtual PropertyBase * addProperty(const std::string & name,
                                       int type) = 0;

    virtual int installFactory(const std::string & type_name,
                               const Atlas::Objects::Root & type_desc,
                               PropertyKit * factory);

    PropertyKit * getPropertyFactory(const std::string &) const;

    /// \brief Return the registered singleton instance of any subclass
    static PropertyManager * instance() {
        return m_instance;
    }

    friend class PropertyManagertest;
};

#endif // COMMON_PROPERTY_MANAGER_H
