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

// $Id$

#ifndef SERVER_STORAGE_MANAGER_H
#define SERVER_STORAGE_MANAGER_H

#include <modules/EntityRef.h>

#include <deque>

class Entity;
class WorldRouter;

/// \brief StorageManager represents the subsystem which stores world storage
///
/// This class has one instance which is the core of the world's persistent
/// storage in whatever data store is being used.
class StorageManager {
  protected:
    typedef std::deque<EntityRef> Entitystore;

    /// \brief Queue of references to entities yet to be stored.
    Entitystore m_unstoredEntities;

    /// \brief Queue of references to entities with modifications.
    Entitystore m_dirtyEntities;

    void entityInserted(Entity *);
    void entityUpdated(Entity *);

    void insertEntity(Entity *);
    void updateEntity(Entity *);

  public:
    StorageManager(WorldRouter &);

    void tick();
    int initWorld();
};

#endif // SERVER_STORAGE_MANAGER_H
