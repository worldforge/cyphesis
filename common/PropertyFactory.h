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

// $Id: PropertyFactory.h,v 1.7 2007-12-07 01:19:16 alriddoch Exp $

#ifndef COMMON_PROPERTY_FACTORY_H
#define COMMON_PROPERTY_FACTORY_H

#include <string>

class Entity;
class PropertyBase;

/// \brief Factory interface for factories to create Property objects.
class PropertyFactory {
  public:
    virtual ~PropertyFactory();

    /// \brief Create a new Property instance
    virtual PropertyBase * newProperty(Entity *) = 0;
};

/// \brief Factory template to create standard Property objects.
template <class T>
class PropertyBuilder : public PropertyFactory {
  public:
    virtual PropertyBase * newProperty(Entity *);
};

#endif // COMMON_PROPERTY_FACTORY_H
