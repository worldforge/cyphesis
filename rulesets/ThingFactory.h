// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

#ifndef RULESETS_THING_FACTORY_H
#define RULESETS_THING_FACTORY_H

#error This file has been removed from the build

#include <Atlas/Message/Element.h>

class Thing;

/// \brief Base class for factories to create entities
///
/// Stores information about default attributes, script language and class
/// name.
class FactoryBase {
  public:
    std::string m_script;
    std::string m_language;
    Atlas::Message::Element::MapType m_attributes;

    virtual ~FactoryBase() { }

    virtual Thing * newThing() = 0;
    virtual FactoryBase * dupFactory() = 0;
};

/// \brief Class Template for factories to create entities of the given class
template <typename T>
class ThingFactory : public FactoryBase {
  public:
    ThingFactory() { }
    Thing * newThing() { return new T(); }
    FactoryBase * dupFactory() { return new ThingFactory<T>(); }
};

#endif // RULESETS_THING_FACTORY_H
