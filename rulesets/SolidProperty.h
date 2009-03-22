// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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

#ifndef RULESETS_SOLID_PROPERTY_H
#define RULESETS_SOLID_PROPERTY_H

#include "common/Property.h"

#include "physics/Vector3D.h"

#include <set>

class Entity;

/// \brief Class to handle whether or not an entity is solid for collisions.
/// \ingroup PropertyClasses
class SolidProperty : public PropertyBase {
  public:
    /// \brief Constructor
    ///
    /// @param owner the owner of the property.
    explicit SolidProperty();

    virtual bool get(Atlas::Message::Element & val) const;
    virtual void set(const Atlas::Message::Element & val);

    virtual void apply(Entity *);
};

#endif // RULESETS_SOLID_PROPERTY_H
