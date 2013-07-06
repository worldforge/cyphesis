// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#ifndef RULESETS_SPAWN_PROPERTY_H
#define RULESETS_SPAWN_PROPERTY_H

#include "common/Property.h"

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::map<std::string, Element> MapType;
    }
}

/// \brief Class to handle Entity where new characters can be spawned.
/// \ingroup PropertyClasses
class SpawnProperty : public Property<Atlas::Message::MapType> {
  public:
    SpawnProperty();
    virtual ~SpawnProperty();

    virtual SpawnProperty * copy() const;

    virtual void apply(LocatedEntity *);
};

#endif // RULESETS_SPAWN_PROPERTY_H
