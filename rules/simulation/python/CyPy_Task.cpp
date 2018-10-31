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
#include "rules/python/CyPy_Operation.h"
#include "rules/python/CyPy_LocatedEntity.h"
#include "CyPy_UsageInstance.h"
#include "rules/python/CyPy_EntityLocation.h"
#include "rules/python/CyPy_Element.h"

#include "rules/python/PythonWrapper.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

Py::Object wrapPython(Task* value) {
    return CyPy_Task::wrap(value);
}

CyPy_Task::CyPy_Task(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    args.verify_length(1);
    auto arg = args.front();
    if (CyPy_Task::check(arg)) {
        m_value = CyPy_Task::value(arg);
    } else if (CyPy_UsageInstance::check(arg)) {
        m_value = new Task(CyPy_UsageInstance::value(arg), this->self());
    } else {
        throw Py::TypeError("Task requires a Task, or UsageInstance");
    }

    for (auto entry : kwds) {
        if (entry.first.isString()) {
            setattro(entry.first.as_string(), entry.second);
        }
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

    PYCXX_ADD_VARARGS_METHOD(irrelevant, irrelevant, "");
    PYCXX_ADD_VARARGS_METHOD(get_arg, getArg, "");

    PYCXX_ADD_NOARGS_METHOD(obsolete, obsolete, "");

    behaviors().readyType();
}


Py::Object CyPy_Task::irrelevant(const Py::Tuple& args)
{
    m_value->irrelevant();
    if (args.size() > 0) {
        args.verify_length(1);
        Atlas::Objects::Operation::Error e;
        Atlas::Objects::Entity::Anonymous arg;
        arg->setAttr("message", verifyString(args.front()));
        e->modifyArgs().push_back(arg);
        e->setTo(m_value->m_usageInstance.actor->getId());
        return CyPy_Operation::wrap(e);
    }
    return Py::None();
}

Py::Object CyPy_Task::obsolete()
{
    return Py::Boolean(m_value->obsolete());
}

Py::Object CyPy_Task::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "usage") {
        return CyPy_UsageInstance::wrap(m_value->m_usageInstance);
    }
    if (nameStr == "actor") {
        return CyPy_LocatedEntity::wrap(m_value->m_usageInstance.actor);
    }
    if (nameStr == "tool") {
        return CyPy_LocatedEntity::wrap(m_value->m_usageInstance.tool);
    }
//    if (nameStr == "targets") {
//        Py::List list(m_value->m_usageInstance.targets.size());
//        for (size_t i = 0; i < m_value->m_usageInstance.targets.size(); ++i) {
//            list[i] = CyPy_EntityLocation::wrap(m_value->m_usageInstance.targets[i]);
//        }
//        return list;
//    }
//    if (nameStr == "consumed") {
//        Py::List list(m_value->m_usageInstance.consumed.size());
//        for (size_t i = 0; i < m_value->m_usageInstance.consumed.size(); ++i) {
//            list[i] = CyPy_EntityLocation::wrap(m_value->m_usageInstance.consumed[i]);
//        }
//        return list;
//    }
    if (nameStr == "definition") {
        return CyPy_Usage::wrap(m_value->m_usageInstance.definition);
    }
    if (nameStr == "op") {
        return CyPy_Operation::wrap(m_value->m_usageInstance.op);
    }


    if (nameStr == "progress") {
        return Py::Float(m_value->progress());
    }
    if (nameStr == "duration") {
        if (!m_value->m_duration) {
            return Py::None();
        }
        return Py::Float(*m_value->m_duration);
    }
    if (nameStr == "tick_interval") {
        if (!m_value->m_tick_interval) {
            return Py::None();
        }
        return Py::Float(*m_value->m_tick_interval);
    }
    if (nameStr == "name") {
        return Py::String(m_value->name());
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
    if (nameStr == "duration") {
        m_value->m_duration = verifyNumeric(attr);
        return 0;
    }
    if (nameStr == "tick_interval") {
        m_value->m_tick_interval = verifyNumeric(attr);
        return 0;
    }
    if (nameStr == "name") {
        m_value->name() = verifyString(attr);
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

Py::Object CyPy_Task::getArg(const Py::Tuple& args)
{
    return CyPy_UsageInstance::getArg(m_value->m_usageInstance, args);
}



