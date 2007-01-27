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

// $Id: SolidProperty.h,v 1.1 2007-01-27 16:35:37 alriddoch Exp $

#ifndef RULESETS_SOLID_PROPERTY_H
#define RULESETS_SOLID_PROPERTY_H

#include "common/Property.h"

#include "physics/Vector3D.h"

#include <set>

class Entity;

/// \brief Class to handle Entity terrain property
/// \ingroup PropertyClasses
class SolidProperty : public PropertyBase {
  protected:
    Entity * m_owner;
  public:
    explicit SolidProperty(Entity * owner);

    virtual bool get(Atlas::Message::Element & val) const;
    virtual void set(const Atlas::Message::Element & val);
};

#endif // RULESETS_LINE_PROPERTY_H
