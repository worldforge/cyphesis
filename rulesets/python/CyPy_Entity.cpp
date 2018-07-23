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

#include "CyPy_Entity.h"
#include "CyPy_Operation.h"

CyPy_Entity::CyPy_Entity(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : CyPy_LocatedEntityBase(self, args, kwds)
{
    args.verify_length(1);

    auto arg = args.front();
    if (arg.isString()) {
        auto id = verifyString(args.front());

        long intId = integerId(id);
        if (intId == -1L) {
            throw Py::TypeError("Entity() requires string/int ID");
        }
        m_value = new Entity(id, intId);
    } else if (CyPy_Entity::check(arg)) {
        m_value = CyPy_Entity::value(arg);
    } else {
        throw Py::TypeError("Entity() requires string ID or Entity");
    }
    m_value->incRef();
}

CyPy_Entity::~CyPy_Entity() = default;

void CyPy_Entity::init_type()
{
    behaviors().name("Entity");
    behaviors().doc("");

    behaviors().supportRichCompare();

    PYCXX_ADD_NOARGS_METHOD(as_entity, as_entity, "");
    PYCXX_ADD_VARARGS_METHOD(is_reachable_for_other_entity, is_reachable_for_other_entity, "");
    PYCXX_ADD_NOARGS_METHOD(describe_entity, describe_entity, "");

    PYCXX_ADD_VARARGS_METHOD(send_world, send_world, "");

    //behaviors().type_object()->tp_base = base;

    behaviors().readyType();
}


CyPy_Entity::CyPy_Entity(Py::PythonClassInstance* self, Entity* value)
    : CyPy_LocatedEntityBase(self, value)
{

}

Py::Object CyPy_Entity::send_world(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->sendWorld(verifyObject<CyPy_Operation>(args.front()));
    return Py::None();
}
