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


#ifndef SERVER_STORAGE_MANAGER_H
#define SERVER_STORAGE_MANAGER_H

#include <modules/EntityRef.h>

#include <deque>
#include <string>
#include <map>

class Entity;
class WorldRouter;
class PropertyBase;

/// \brief StorageManager represents the subsystem which stores world storage
///
/// This class has one instance which is the core of the world's persistent
/// storage in whatever data store is being used.
class StorageManager {
  protected:
    typedef std::deque<EntityRef> Entitystore;
    typedef std::deque<long> Idstore;

    /// \brief Queue of references to entities yet to be stored.
    Entitystore m_unstoredEntities;

    /// \brief Queue of references to entities with modifications.
    Entitystore m_dirtyEntities;

    /// \brief Queue of IDs of entities that are destroyed
    Idstore m_destroyedEntities;

    int m_insertEntityCount;
    int m_updateEntityCount;

    int m_insertPropertyCount;
    int m_updatePropertyCount;

    int m_insertQps;
    int m_updateQps;

    int m_insertQpsNow;
    int m_updateQpsNow;

    int m_insertQpsAvg;
    int m_updateQpsAvg;

    int m_insertQpsIndex;
    int m_updateQpsIndex;

    int m_insertQpsRing[32];
    int m_updateQpsRing[32];

    void entityInserted(LocatedEntity *);
    void entityUpdated(LocatedEntity *);
    void entityContainered(const LocatedEntity *oldLocation, LocatedEntity *entity);

    void encodeProperty(PropertyBase *, std::string &);
    void restoreProperties(LocatedEntity *);

    void restoreThoughts(LocatedEntity *);
    void storeThoughts(LocatedEntity *);

    void insertEntity(LocatedEntity *);
    void updateEntity(LocatedEntity *);
    void restoreChildren(LocatedEntity *);

  public:
    StorageManager(WorldRouter &);

    void tick();
    int initWorld();
    int restoreWorld();

    /// \brief Called when shutting down.
    ///
    /// It's expected that the storage manager attempts to persist entity state.
    int shutdown(bool& exit_flag, const std::map<long, LocatedEntity *>& entites);

};

#endif // SERVER_STORAGE_MANAGER_H
