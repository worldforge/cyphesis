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

#include "Persistence.h"

#include "common/OperationRouter.h"
#include "modules/WeakEntityRef.h"

#include <sigc++/trackable.h>

#include <deque>
#include <string>
#include <map>
#include <set>
#include "modules/Ref.h"

class Entity;
class EntityBuilder;
class WorldRouter;
class Database;
class PropertyBase;

/// \brief StorageManager represents the subsystem which stores world storage
///
/// This class has one instance which is the core of the world's persistent
/// storage in whatever data store is being used.
class StorageManager : public sigc::trackable
{
    protected:
        typedef std::deque<WeakEntityRef> Entitystore;
        typedef std::deque<long> Idstore;

        Database& m_db;
        EntityBuilder& m_entityBuilder;

        /// \brief Queue of references to entities yet to be stored.
        Entitystore m_unstoredEntities;

        /// \brief Queue of references to entities with modifications.
        Entitystore m_dirtyEntities;

        /// \brief Queue of IDs of entities that are destroyed
        Idstore m_destroyedEntities;

        std::deque<Persistence::AddCharacterData> m_addedCharacters;

        std::deque<std::string> m_deletedCharacters;

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

        std::array<int, 32> m_insertQpsRing;
        std::array<int, 32> m_updateQpsRing;

        void entityInserted(LocatedEntity*);

        void entityUpdated(LocatedEntity*);

        void encodeProperty(PropertyBase*, std::string&);

        void restorePropertiesRecursively(LocatedEntity*);

        void insertEntity(LocatedEntity*);

        void updateEntity(LocatedEntity*);

        void restoreChildren(LocatedEntity*);

        bool persistance_characterAdded(const Persistence::AddCharacterData& data);

        bool persistance_characterDeleted(const std::string& entityId);

    public:
        explicit StorageManager(WorldRouter&, Database& db, EntityBuilder& entityBuilder);

        virtual ~StorageManager();

        void tick();

        int initWorld(const Ref<LocatedEntity>& ent);

        int restoreWorld(const Ref<LocatedEntity>& ent);

        /// \brief Called when shutting down.
        ///
        /// It's expected that the storage manager attempts to persist entity state.
        int shutdown(bool& exit_flag, const std::map<long, Ref<LocatedEntity>>& entites);

};

#endif // SERVER_STORAGE_MANAGER_H
