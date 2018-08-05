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

#include <rulesets/TasksProperty.h>
#include "CyPy_Entity.h"
#include "CyPy_Operation.h"
#include "CyPy_Character.h"
#include "CyPy_Task.h"
#include "CyPy_Oplist.h"

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
}

CyPy_Entity::~CyPy_Entity() = default;

void CyPy_Entity::init_type()
{
    behaviors().name("Entity");
    behaviors().doc("");

    behaviors().supportRichCompare();

    PYCXX_ADD_NOARGS_METHOD(as_entity, as_entity, "");
    PYCXX_ADD_VARARGS_METHOD(can_reach, can_reach, "");
    PYCXX_ADD_NOARGS_METHOD(describe_entity, describe_entity, "");
    PYCXX_ADD_VARARGS_METHOD(client_error, client_error, "");

    PYCXX_ADD_VARARGS_METHOD(send_world, send_world, "");

    //behaviors().type_object()->tp_base = base;

    behaviors().readyType();
}


CyPy_Entity::CyPy_Entity(Py::PythonClassInstance* self, Ref<Entity> value)
    : CyPy_LocatedEntityBase(self, std::move(value))
{

}

Py::Object CyPy_Entity::start_task(const Py::Tuple& args)
{
    return CyPy_Entity::start_task(m_value, args);
}

Py::Object CyPy_Entity::send_world(const Py::Tuple& args)
{
    return CyPy_Entity::send_world(m_value, args);
}

bool CyPy_Entity::check(const Py::Object& object)
{
    return check(object.ptr());
}

bool CyPy_Entity::check(PyObject* object)
{
    //FIXME: Hack to fake inheritance
    return WrapperBase::check(object)
           || CyPy_Character::check(object);
}

Ref<Entity> CyPy_Entity::value(const Py::Object& object)
{
    //FIXME: Hack to fake inheritance
    if (CyPy_Character::check(object)) {
        return Py::PythonClassObject<CyPy_Character>(object).getCxxObject()->m_value;
    }
    return Py::PythonClassObject<CyPy_Entity>(object).getCxxObject()->m_value;

}

Py::Object CyPy_Entity::send_world(const Ref<Entity>& entity, const Py::Tuple& args)
{
    args.verify_length(1);
    entity->sendWorld(verifyObject<CyPy_Operation>(args.front()));
    return Py::None();
}

Py::Object CyPy_Entity::start_task(const Ref<Entity>& entity, const Py::Tuple& args)
{
    OpVector res;
    args.verify_length(2);

    auto tp = entity->requirePropertyClassFixed<TasksProperty>();
    tp->startTask(verifyString(args[0]), verifyObject<CyPy_Task>(args[1]), entity.get(), res);

    return CyPy_Oplist::wrap(std::move(res));
}
