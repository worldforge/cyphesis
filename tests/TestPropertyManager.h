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


#ifndef TESTS_TEST_PROPERTY_MANAGER_H
#define TESTS_TEST_PROPERTY_MANAGER_H

#include "common/PropertyManager.h"

#include <map>

class PropertyKit;

typedef std::map<std::string, PropertyKit *> PropertyFactoryDict;

class TestPropertyManager : public PropertyManager {
  private:
    std::map<std::string, PropertyKit *> m_propertyFactories;
  public:
    TestPropertyManager();
    virtual ~TestPropertyManager();

    virtual PropertyBase * addProperty(const std::string & name, int type);

    void installPropertyFactory(const std::string &, PropertyKit *);
};

#endif // TESTS_TEST_PROPERTY_MANAGER_H
