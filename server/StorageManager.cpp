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

// $Id: StorageManager.cpp,v 1.4 2008-08-22 19:35:09 alriddoch Exp $

#include "StorageManager.h"

#include "WorldRouter.h"

#include "rulesets/Entity.h"

#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/mem_fun.h>

#include <iostream>

StorageManager:: StorageManager(WorldRouter & world)
{
    world.inserted.connect(sigc::mem_fun(this, &StorageManager::entityInserted));
}

/// \brief Called when a new Entity is inserted in the world
void StorageManager::entityInserted(Entity * ent)
{
    m_unstoredEntities.push_back(EntityRef(ent));
    ent->setFlags(entity_queued);
    ent->updated.connect(sigc::bind(sigc::mem_fun(this, &StorageManager::entityUpdated), ent));
}

/// \brief Called when an Entity is modified
void StorageManager::entityUpdated(Entity * ent)
{
    // Is it already in the dirty Entities queue?
    // Perhaps we need to modify the semantics of the updated signal
    // so it is only emitted if the entity was not marked as dirty.
    if (ent->getFlags() & entity_queued) {
        // std::cout << "Already queued " << ent->getId() << std::endl << std::flush;
        return;
    }
    m_dirtyEntities.push_back(EntityRef(ent));
    // std::cout << "Updated fired " << ent->getId() << std::endl << std::flush;
    ent->setFlags(entity_queued);
}

void StorageManager::tick()
{
    while (!m_unstoredEntities.empty()) {
        const EntityRef & ent = m_unstoredEntities.front();
        if (ent.get() != 0) {
            std::cout << "storing " << ent->getId() << std::endl << std::flush;
            ent->resetFlags(entity_queued);
            ent->setFlags(entity_clean);
        } else {
            std::cout << "deleted" << std::endl << std::flush;
        }
        m_unstoredEntities.pop_front();
    }

    while (!m_dirtyEntities.empty()) {
        const EntityRef & ent = m_dirtyEntities.front();
        if (ent.get() != 0) {
            std::cout << "updating " << ent->getId() << std::endl << std::flush;
            ent->resetFlags(entity_queued);
            ent->setFlags(entity_clean);
        } else {
            std::cout << "deleted" << std::endl << std::flush;
        }
        m_dirtyEntities.pop_front();
    }

}
