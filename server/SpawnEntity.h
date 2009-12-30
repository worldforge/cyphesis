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

// $Id$

#ifndef SERVER_SPAWN_ENTITY_H
#define SERVER_SPAWN_ENTITY_H

#include "server/Spawn.h"

#include "modules/EntityRef.h"

#include <string>
#include <vector>

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::map<std::string, Element> MapType;
        typedef std::vector<Element> ListType;
    }
}

class SpawnEntity : public Spawn {
  protected:
    /// Reference to the entity that defines this spawn location
    EntityRef m_ent;
    /// List of character types available here
    Atlas::Message::ListType m_characterTypes;
  public:
    explicit SpawnEntity(Entity * e, const Atlas::Message::MapType & data);

    int spawnEntity(const std::string & type,
                    const Atlas::Objects::Entity::RootEntity & dsc);
    int populateEntity(Entity * ent,
                       const Atlas::Objects::Entity::RootEntity & dsc,
                       OpVector & res);
};

#endif // SERVER_SPAWN_ENTITY_H
