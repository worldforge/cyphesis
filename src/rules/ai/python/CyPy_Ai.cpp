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

#include "CyPy_Ai.h"
#include "CyPy_BaseMind.h"
#include "CyPy_MemMap.h"
#include "rules/python/CyPy_MemEntity.h"

CyPy_Ai::CyPy_Ai() : ExtensionModule("ai")
{

    CyPy_BaseMind::init_type();
    CyPy_MemMap::init_type();


    initialize("ai");

    Py::Dict d(moduleDictionary());
    d["Mind"] = CyPy_BaseMind::type();
    d["MemMap"] = CyPy_MemMap::type();

}


std::string CyPy_Ai::init()
{
    PyImport_AppendInittab("ai", []() {
        static auto server = new CyPy_Ai();
        return server->module().ptr();
    });
    return "ai";
}
