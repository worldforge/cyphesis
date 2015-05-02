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


#ifndef SERVER_SPAWN_H
#define SERVER_SPAWN_H

#include "common/OperationRouter.h"

#include <map>
#include <string>

class LocatedEntity;
class Location;

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::map<std::string, Element> MapType;
    }
}

/**
 * \brief Defines a spawn point.
 *
 * A spawn point is a place where new entities can be spawned.
 * Mainly used for creating new character controlled entities.
 */
class Spawn {
  public:
    virtual ~Spawn(){}

    /**
     * \brief Fills the supplied entity with information used for creating a new entity.
     *
     * \param type The name of the spawn type.
     * \param dsc An entity description which will be filled with entity data.
     * \return 0 if successful.
     */
    virtual int spawnEntity(const std::string & type,
                            const Atlas::Objects::Entity::RootEntity & dsc) const = 0;

    /**
     * \brief Provides information about the types available.
     *
     * If there are entities defined the supplied map will have an enty
     * named 'character_types' which will contain a list of strings. These
     * are the types of spawnable entities that are available.
     * @param msg A map, to be filled.
     * @return 0 if successful.
     */
    virtual int addToMessage(Atlas::Message::MapType & msg) const = 0;

    /*
     * \brief Places a location in the spawn.
     */
    virtual int placeInSpawn(Location& location) const = 0;

};

#endif // SERVER_SPAWN_H
