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

// $Id$

#include "PropertyManager.h"

#include "PropertyFactory.h"

#include <cassert>

PropertyManager * PropertyManager::m_instance = 0;

/// \brief PropertyManager constructor
///
/// Installs this instance as the singleton instance for this base class
PropertyManager::PropertyManager()
{
    assert(m_instance == 0);
    m_instance = this;
}

PropertyManager::~PropertyManager()
{
    std::map<std::string, PropertyKit *>::const_iterator I = m_propertyFactories.begin();
    std::map<std::string, PropertyKit *>::const_iterator Iend = m_propertyFactories.end();
    for (; I != Iend; ++I) {
        assert(I->second != 0);
        delete I->second;
    }
    m_instance = 0;
}

PropertyKit * PropertyManager::getPropertyFactory(const std::string & name) const
{
    auto I = m_propertyFactories.find(name);
    if (I != m_propertyFactories.end()) {
        assert(I->second != 0);
        return I->second;
    }
    return 0;
}

void PropertyManager::installFactory(const std::string & name,
                                     PropertyKit * factory)
{
    m_propertyFactories.insert(std::make_pair(name, factory));
}
