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

#include <modules/WorldTime.h>
#include "CyPy_World.h"
#include "CyPy_WorldTime.h"
#include "CyPy_LocatedEntity.h"

CyPy_World::CyPy_World(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    throw Py::RuntimeError("Can not create World instances.");
}

CyPy_World::CyPy_World(Py::PythonClassInstance* self, BaseWorld* value)
    : WrapperBase(self, value)
{

}

void CyPy_World::init_type()
{
    behaviors().name("World");
    behaviors().doc("");

    PYCXX_ADD_VARARGS_METHOD(get_object, get_object, "");
    PYCXX_ADD_VARARGS_METHOD(get_object_ref, get_object_ref, "");

    PYCXX_ADD_NOARGS_METHOD(get_time, get_time, "");

    behaviors().readyType();
}

Py::Object CyPy_World::get_time()
{
    return Py::Float(m_value->getTime());
}

Py::Object CyPy_World::get_object(const Py::Tuple& args)
{
    args.verify_length(1);
    auto id = verifyString(args.front());
    auto ent = m_value->getEntity(id);
    if (!ent) {
        return Py::None();
    }
    return CyPy_LocatedEntity::wrap(std::move(ent));
}

//FIXME: remove this one
Py::Object CyPy_World::get_object_ref(const Py::Tuple& args)
{
    return get_object(args);
}
