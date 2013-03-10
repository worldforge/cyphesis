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

#include "PropertyFactory_impl.h"

#include "Property.h"

PropertyKit::~PropertyKit()
{
}

/// \brief Create a new Property instance
PropertyBase * PropertyKit::newProperty()
{
    return 0;
}

/// \brief Create a copy of this factory.
///
/// This is typically used when installing a new data driven property type
/// to create a factory which inherits from a previously installed type.
/// @return a new property factory object
PropertyKit * PropertyKit::duplicateFactory() const
{
    return 0;
}

/// \brief create factory which extends this to support scripts
///
/// This is typically used when installing a new data driven property type
/// to create a factory which inherits from a previously installed type, and
/// adds the facility to allow the property to have a script associated with
/// it.
/// @return a new property factory object
PropertyKit * PropertyKit::scriptPropertyFactory() const
{
    return 0;
}
