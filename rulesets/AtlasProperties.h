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

// $Id: AtlasProperties.h,v 1.1 2007-01-03 21:55:31 alriddoch Exp $

#ifndef RULESETS_ATLAS_PROPERTIES_H
#define RULESETS_ATLAS_PROPERTIES_H

#include "common/Property.h"

#include "common/types.h"

/// \brief Class to handle Entity id property
/// \ingroup PropertyClasses
class IdProperty : public ImmutableProperty<std::string> {
  public:
    explicit IdProperty(const std::string & data);

    virtual void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const;
};

/// \brief Class to handle Entity name property
/// \ingroup PropertyClasses
class NameProperty : public Property<std::string> {
  public:
    explicit NameProperty(std::string & data,
                          unsigned int flags);

    virtual void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const;
};

/// \brief Class to handle Entity contains property
/// \ingroup PropertyClasses
class ContainsProperty : public ImmutableProperty<EntitySet> {
  public:
    explicit ContainsProperty(EntitySet & data);

    virtual void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const;
};

#endif // RULESETS_ATLAS_PROPERTIES_H
