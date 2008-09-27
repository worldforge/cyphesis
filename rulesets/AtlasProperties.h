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

#ifndef RULESETS_ATLAS_PROPERTIES_H
#define RULESETS_ATLAS_PROPERTIES_H

#include "common/Property.h"

#include <set>

/// \brief Class to handle Entity id property
/// \ingroup PropertyClasses
class IdProperty : public PropertyBase {
  protected:
    const std::string & m_data;
  public:
    explicit IdProperty(const std::string & data);

    virtual bool get(Atlas::Message::Element & val) const;
    virtual void set(const Atlas::Message::Element & val);
    virtual void add(const std::string & key, Atlas::Message::MapType & map) const;
    virtual void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const;
};

/// \brief Class to handle Entity name property
/// \ingroup PropertyClasses
class NameProperty : public Property<std::string> {
  public:
    explicit NameProperty(unsigned int flags);

    virtual void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const;
};

class LocatedEntity;

typedef std::set<LocatedEntity *> LocatedEntitySet;

/// \brief Class to handle Entity contains property
/// \ingroup PropertyClasses
class ContainsProperty : public PropertyBase {
  protected:
    LocatedEntitySet & m_data;
  public:
    explicit ContainsProperty(LocatedEntitySet & data);

    virtual bool get(Atlas::Message::Element & val) const;
    virtual void set(const Atlas::Message::Element & val);
    virtual void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const;
};

#endif // RULESETS_ATLAS_PROPERTIES_H
