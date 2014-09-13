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


#ifndef RULESETS_ENTITY_PROPERTY_H
#define RULESETS_ENTITY_PROPERTY_H

#include "common/Property.h"

#include "modules/EntityRef.h"

/// \brief Class to handle an Entity property that references another entity
/// \ingroup PropertyClasses
class EntityProperty : public PropertyBase {
  protected:
    EntityRef m_data;
  public:
    explicit EntityProperty();

    const EntityRef & data() const { return m_data; }
    EntityRef & data() { return m_data; }

    virtual int get(Atlas::Message::Element & val) const;
    virtual void set(const Atlas::Message::Element & val);
    virtual void add(const std::string & val, Atlas::Message::MapType & map) const;
    virtual void add(const std::string & val, const Atlas::Objects::Entity::RootEntity & ent) const;
    virtual EntityProperty * copy() const;
};

#endif // RULESETS_ENTITY_PROPERTY_H
