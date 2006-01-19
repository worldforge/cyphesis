// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#ifndef RULESETS_LINE_PROPERTY_H
#define RULESETS_LINE_PROPERTY_H

#include "common/Property.h"

#include "physics/Vector3D.h"

#include <set>

/// \brief Class to handle Entity terrain property
class LineProperty : public PropertyBase {
  protected:
    CoordList & m_data;
    // PointSet & m_modifiedTerrain; // FIXME We will need this for persistence
    // PointSet & m_createdTerrain;
  public:
    explicit LineProperty(CoordList & data,
                          unsigned int flags);

    virtual void get(Atlas::Message::Element &) const;
    virtual void set(const Atlas::Message::Element &);
    virtual void add(const std::string &, Atlas::Message::MapType & map) const;
};

#endif // RULESETS_LINE_PROPERTY_H
