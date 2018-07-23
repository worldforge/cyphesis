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

#include <rulesets/Py_Task.h>
#include <rulesets/Py_Oplist.h>
#include "CyPy_Character.h"
#include "CyPy_Task.h"
#include "CyPy_Operation.h"
#include "CyPy_Oplist.h"

CyPy_Character::CyPy_Character(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : CyPy_LocatedEntityBase(self, args, kwds)
{
    args.verify_length(1);

    auto arg = args.front();
    if (arg.isString()) {
        auto id = verifyString(args.front());

        long intId = integerId(id);
        if (intId == -1L) {
            throw Py::TypeError("Character() requires string/int ID");
        }
        m_value = new Character(id, intId);
    } else if (CyPy_Character::check(arg)) {
        m_value = CyPy_Character::value(arg);
    } else {
        throw Py::TypeError("Character() requires string ID or Character");
    }
    m_value->incRef();
}

CyPy_Character::~CyPy_Character()
{
}

void CyPy_Character::init_type()
{
    behaviors().name("Character");
    behaviors().doc("");

    behaviors().supportRichCompare();

    PYCXX_ADD_NOARGS_METHOD(as_entity, as_entity, "");
    PYCXX_ADD_VARARGS_METHOD(is_reachable_for_other_entity, is_reachable_for_other_entity, "");
    PYCXX_ADD_NOARGS_METHOD(describe_entity, describe_entity, "");

    PYCXX_ADD_VARARGS_METHOD(send_world, send_world, "");

    PYCXX_ADD_VARARGS_METHOD(start_task, start_task, "");
    PYCXX_ADD_VARARGS_METHOD(mind2body, mind2body, "");

    //behaviors().type_object()->tp_base = base;

    behaviors().readyType();

}

CyPy_Character::CyPy_Character(Py::PythonClassInstance* self, Character* value)
    : CyPy_LocatedEntityBase(self, value)
{

}

Py::Object CyPy_Character::start_task(const Py::Tuple& args)
{
    args.verify_length(3);

    auto task = verifyObject<CyPy_Task>(args[0]);
    auto op = verifyObject<CyPy_Operation>(args[1]);
    auto res = verifyObject<CyPy_Oplist>(args[2]);
    m_value->startTask(task, op, res);
    return Py::None();
}

Py::Object CyPy_Character::mind2body(const Py::Tuple& args)
{
    args.verify_length(1);
    auto op = verifyObject<CyPy_Operation>(args.front());
    OpVector res;
    m_value->mind2body(op, res);
    if (res.empty()) {
        return Py::None();
    } else if (res.size() == 1) {
        return CyPy_Operation::wrap(std::move(res.front()));
    } else {
        return CyPy_Oplist::wrap(std::move(res));
    }
}

//TODO: This is copied from Entity, perhaps we should consolidate the code?
Py::Object CyPy_Character::send_world(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->sendWorld(verifyObject<CyPy_Operation>(args.front()));
    return Py::None();
}
