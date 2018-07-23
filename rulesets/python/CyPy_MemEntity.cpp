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

#include "CyPy_MemEntity.h"

CyPy_MemEntity::CyPy_MemEntity(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : CyPy_LocatedEntityBase(self, args, kwds)
{
    args.verify_length(1);

    auto arg = args.front();
    if (arg.isString()) {
        auto id = verifyString(args.front());

        long intId = integerId(id);
        if (intId == -1L) {
            throw Py::TypeError("MemEntity() requires string/int ID");
        }
        m_value = new MemEntity(id, intId);
    } else if (CyPy_MemEntity::check(arg)) {
        m_value = CyPy_MemEntity::value(arg);
    } else {
        throw Py::TypeError("MemEntity() requires string ID or MemEntity");
    }
    m_value->incRef();
}

CyPy_MemEntity::~CyPy_MemEntity()
{

}

void CyPy_MemEntity::init_type()
{
    behaviors().name("MemEntity");
    behaviors().doc("");

    behaviors().supportRichCompare();

    PYCXX_ADD_NOARGS_METHOD(as_entity, as_entity, "");
    PYCXX_ADD_VARARGS_METHOD(is_reachable_for_other_entity, is_reachable_for_other_entity, "");
    PYCXX_ADD_NOARGS_METHOD(describe_entity, describe_entity, "");

    //behaviors().type_object()->tp_base = base;

    behaviors().readyType();
}


CyPy_MemEntity::CyPy_MemEntity(Py::PythonClassInstance* self, MemEntity* value)
    : CyPy_LocatedEntityBase(self, value)
{

}
