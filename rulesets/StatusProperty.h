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

// $Id: StatusProperty.h,v 1.1 2007-11-29 01:13:27 alriddoch Exp $

#ifndef RULESETS_STATUS_PROPERTY_H
#define RULESETS_STATUS_PROPERTY_H

#include "common/Property.h"

#include <set>

class Entity;

/// \brief Class to handle whether or not an entity is solid for collisions.
/// \ingroup PropertyClasses
class StatusProperty : public PropertyBase {
  protected:
    /// The owner of the property.
    Entity * m_owner;
    /// The value of the property.
    float m_value;
  public:
    /// \brief Constructor
    ///
    /// @param owner the owner of the property.
    explicit StatusProperty(Entity * owner);

    virtual bool get(Atlas::Message::Element & val) const;
    virtual void set(const Atlas::Message::Element & val);
};

#endif // RULESETS_STATUS_PROPERTY_H
