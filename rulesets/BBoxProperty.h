// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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


#ifndef RULESETS_B_BOX_PROPERTY_H
#define RULESETS_B_BOX_PROPERTY_H

#include "common/Property.h"

#include "physics/BBox.h"

#include <wfmath/axisbox.h>
#include <wfmath/point.h>

/// \brief Class to handle Entity bbox property
/// \ingroup PropertyClasses
class BBoxProperty : public PropertyBase {
  protected:
    BBox m_data;
  public:
    explicit BBoxProperty();

    const BBox & data() const { return m_data; }
    BBox & data() { return m_data; }

    virtual void apply(LocatedEntity *);
    virtual int get(Atlas::Message::Element & val) const;
    virtual void set(const Atlas::Message::Element & val);
    virtual void add(const std::string & key,
                     Atlas::Message::MapType & map) const;
    virtual void add(const std::string & key,
                     const Atlas::Objects::Entity::RootEntity & ent) const;
    virtual BBoxProperty * copy() const;
};

#endif // RULESETS_B_BOX_PROPERTY_H
