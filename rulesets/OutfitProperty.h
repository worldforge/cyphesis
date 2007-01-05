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

// $Id: OutfitProperty.h,v 1.6 2007-01-05 02:58:53 alriddoch Exp $

#ifndef RULESETS_OUTFIT_PROPERTY_H
#define RULESETS_OUTFIT_PROPERTY_H

#include "common/Property.h"

#include "modules/EntityRef.h"

#include <sigc++/trackable.h>

typedef std::map<std::string, EntityRef> EntityRefMap;

/// \brief Class to handle Character outfit property
///
/// OUTFIT is the property used to describe how entities in the CONTAINS
/// list of the entity are attached to it. Primarily intended for describing
/// the clothes and equipment attached to a Character, it could be potentially
/// used for all sorts of things.
/// \ingroup PropertyClasses
class OutfitProperty : public PropertyBase, virtual public sigc::trackable {
  protected:
    EntityRefMap m_data;

    void itemRemoved(Entity *);
  public:
    explicit OutfitProperty();
    virtual ~OutfitProperty();

    virtual bool get(Atlas::Message::Element & val) const;
    virtual void set(const Atlas::Message::Element & val);
    virtual void add(const std::string & key, Atlas::Message::MapType & map) const;
    virtual void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const;

    void cleanUp();
    void wear(const std::string & location, Entity * garment);
};

#endif // RULESETS_OUTFIT_PROPERTY_H
