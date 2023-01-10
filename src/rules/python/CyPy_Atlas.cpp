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

#include "CyPy_Atlas.h"
#include "CyPy_Element.h"
#include "CyPy_Operation.h"
#include "CyPy_Oplist.h"
#include "CyPy_RootEntity.h"
#include "CyPy_Location.h"
#include "CyPy_EntityLocation.h"
#include "CyPy_Root.h"

CyPy_Atlas::CyPy_Atlas() : ExtensionModule("atlas")
{

    CyPy_ElementMap::init_type();
    CyPy_ElementList::init_type();
    CyPy_Root::init_type();
    CyPy_Operation::init_type();
    CyPy_Oplist::init_type();
    CyPy_RootEntity::init_type();


    initialize("Atlas bindings, allowing the Atlas library to be used from Python.");

    Py::Dict d(moduleDictionary());

    d["Operation"] = CyPy_Operation::type();
    d["Root"] = CyPy_Root::type();
    d["Oplist"] = CyPy_Oplist::type();
    d["Entity"] = CyPy_RootEntity::type();
    d["ElementList"] = CyPy_ElementList::type();
    d["ElementMap"] = CyPy_ElementMap::type();
}

std::string CyPy_Atlas::init()
{
    PyImport_AppendInittab("atlas", []() {
        static auto module = new CyPy_Atlas();
        return module->module().ptr();
    });
    return "atlas";
}
