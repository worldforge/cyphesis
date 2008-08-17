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

// $Id: StorageManager.cpp,v 1.1 2008-08-17 21:12:50 alriddoch Exp $

#include "StorageManager.h"

#include "WorldRouter.h"

#include "rulesets/Entity.h"

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
}

/// \brief Called when an Entity is modified
void StorageManager::entityUpdated(Entity * ent)
{
    // Is it already in the dirty Entities queue?
    // Perhaps we need to modify the semantics of the updated signal
    // so it is only emitted if the entity was not marked as dirty.
    m_dirtyEntities.push_back(EntityRef(ent));
}
