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

#include "CyPy_Task.h"
#include "CyPy_Operation.h"
#include "CyPy_LocatedEntity.h"

CyPy_Task::CyPy_Task(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    args.verify_length(1);
    auto arg = args.front();
    if (CyPy_Task::check(arg)) {
        m_value = CyPy_Task::value(arg);
    } else if (CyPy_LocatedEntity::check(arg)) {
        m_value = new Task(CyPy_LocatedEntity::value(arg));
    } else {
        throw Py::TypeError("Task requires a Task, or Entity");
    }
}

CyPy_Task::CyPy_Task(Py::PythonClassInstance* self, Ref<Task> value)
    : WrapperBase(self, std::move(value))
{

}


CyPy_Task::~CyPy_Task() = default;

void CyPy_Task::init_type()
{
    behaviors().name("Task");
    behaviors().doc("");

    behaviors().supportRichCompare();

    PYCXX_ADD_NOARGS_METHOD(irrelevant, irrelevant, "");

    PYCXX_ADD_NOARGS_METHOD(obsolete, obsolete, "");

    PYCXX_ADD_NOARGS_METHOD(count, count, "");

    PYCXX_ADD_NOARGS_METHOD(new_tick, newtick, "");

    PYCXX_ADD_VARARGS_METHOD(next_tick, nexttick, "");

    behaviors().readyType();
}


Py::Object CyPy_Task::irrelevant()
{
    m_value->irrelevant();
    return Py::None();
}

Py::Object CyPy_Task::obsolete()
{
    return Py::Boolean(m_value->obsolete());
}

Py::Object CyPy_Task::count()
{
    return Py::Long(m_value->count());
}

Py::Object CyPy_Task::newtick()
{
    return Py::Long(m_value->newTick());
}

Py::Object CyPy_Task::nexttick(const Py::Tuple& args)
{
    double interval = verifyNumeric(args.front());
    return CyPy_Operation::wrap(m_value->nextTick(interval));
}

Py::Object CyPy_Task::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "character") {
        return CyPy_LocatedEntity::wrap(&m_value->owner());
    }
    if (nameStr == "progress") {
        return Py::Float(m_value->progress());
    }
    if (nameStr == "rate") {
        return Py::Float(m_value->rate());
    }
    Atlas::Message::Element val;
    if (m_value->getAttr(name, val) == 0) {
        if (val.isNone()) {
            return Py::None();
        } else {
            return CyPy_Element::wrap(val);
        }
    }

    return PythonExtensionBase::getattro(name);
}

int CyPy_Task::setattro(const Py::String& name, const Py::Object& attr)
{

    auto nameStr = name.as_string();
    if (nameStr == "progress") {
        m_value->progress() = verifyNumeric(attr);
        return 0;
    }
    if (nameStr == "rate") {
        m_value->rate() = verifyNumeric(attr);
        return 0;
    }

    auto element = CyPy_Element::asElement(attr);
    m_value->setAttr(nameStr, element);
    return 0;
}

Py::Object CyPy_Task::rich_compare(const Py::Object& other, int type)
{
    if (type == Py_EQ) {
        if (CyPy_Task::check(other)) {
            return Py::Boolean(m_value == CyPy_Task::value(other));
        }
        return Py::False();
    }
    if (type == Py_NE) {
        if (CyPy_Task::check(other)) {
            return Py::Boolean(m_value != CyPy_Task::value(other));
        }
        return Py::True();
    }
    throw Py::NotImplementedError("Not implemented");
}



