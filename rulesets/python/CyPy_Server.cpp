/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "CyPy_Server.h"
#include "CyPy_LocatedEntity.h"
#include "CyPy_MemMap.h"
#include "CyPy_Character.h"
#include "CyPy_Entity.h"
#include "CyPy_BaseMind.h"
#include "CyPy_MemEntity.h"
#include "CyPy_Task.h"
#include "CyPy_World.h"
#include "CyPy_WorldTime.h"


CyPy_Server::CyPy_Server() : ExtensionModule("server")
{

    CyPy_Props::init_type();
    CyPy_Character::init_type();
    CyPy_Entity::init_type();
    CyPy_BaseMind::init_type();
    CyPy_MemEntity::init_type();
    CyPy_MemMap::init_type();
    CyPy_Task::init_type();
    CyPy_World::init_type();
    CyPy_WorldTime::init_type();

    initialize("server");

    Py::Dict d(moduleDictionary());
    //d["LocatedEntity"] = CyPy_LocatedEntity::type();
    d["Character"] = CyPy_Character::type();
    d["Entity"] = CyPy_Entity::type();
    //FIXME: Remove "Thing"
    d["Thing"] = CyPy_Entity::type();
    d["Mind"] = CyPy_BaseMind::type();
    d["MemEntity"] = CyPy_MemEntity::type();
    //FIXME: Rename to "MemMap"
    //TODO: should this really be exposed?
    d["Map"] = CyPy_MemMap::type();
    d["Task"] = CyPy_Task::type();
    d["WorldTime"] = CyPy_WorldTime::type();

    d["OPERATION_IGNORED"] = Py::Long(OPERATION_IGNORED);
    d["OPERATION_HANDLED"] = Py::Long(OPERATION_HANDLED);
    d["OPERATION_BLOCKED"] = Py::Long(OPERATION_BLOCKED);


}

void CyPy_Server::registerWorld(BaseWorld* world)
{
    Py::Dict d(moduleDictionary());
    d["world"] = CyPy_World::wrap(world);
}
