// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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

// $Id: AreaProperty.h,v 1.5 2006-12-12 19:52:39 alriddoch Exp $

#ifndef RULESETS_AREA_PROPERTY_H
#define RULESETS_AREA_PROPERTY_H

#include "LineProperty.h"

#include "physics/Vector3D.h"

#include <set>

/// \brief Class to handle Entity terrain property
/// \ingroup PropertyClasses
class AreaProperty : public PropertyBase {
  protected:
    LineProperty m_line;
  public:
    explicit AreaProperty(unsigned int flags);

    virtual bool get(Atlas::Message::Element &);
    virtual void set(const Atlas::Message::Element &);
    virtual void add(const std::string &, Atlas::Message::MapType & map);
};

#endif // RULESETS_AREA_PROPERTY_H
