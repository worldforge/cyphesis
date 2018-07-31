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

#include "CyPy_UsageInstance.h"
#include "CyPy_LocatedEntity.h"
#include "CyPy_EntityLocation.h"
#include "CyPy_EntityFilter.h"
#include "CyPy_Operation.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

CyPy_UsageInstance::CyPy_UsageInstance(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{

}

CyPy_UsageInstance::CyPy_UsageInstance(Py::PythonClassInstance* self, UsageInstance value)
    : WrapperBase(self, std::move(value))
{

}

void CyPy_UsageInstance::init_type()
{
    behaviors().name("UsageInstance");
    behaviors().doc("");

    PYCXX_ADD_NOARGS_METHOD(is_valid, isValid, "");

    behaviors().readyType();
}

Py::Object CyPy_UsageInstance::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "actor") {
        return CyPy_LocatedEntity::wrap(m_value.actor);
    }
    if (nameStr == "tool") {
        return CyPy_LocatedEntity::wrap(m_value.tool);
    }
    if (nameStr == "targets") {
        Py::List list(m_value.targets.size());
        for (size_t i = 0; i < m_value.targets.size(); ++i) {
            list[i] = CyPy_EntityLocation::wrap(m_value.targets[i]);
        }
        return list;
    }
    if (nameStr == "consumed") {
        Py::List list(m_value.consumed.size());
        for (size_t i = 0; i < m_value.consumed.size(); ++i) {
            list[i] = CyPy_EntityLocation::wrap(m_value.consumed[i]);
        }
        return list;
    }
    if (nameStr == "definition") {
        return CyPy_Usage::wrap(m_value.definition);
    }
    if (nameStr == "op") {
        return CyPy_Operation::wrap(m_value.op);
    }
    return PythonExtensionBase::getattro(name);
}

int CyPy_UsageInstance::setattro(const Py::String& name, const Py::Object& attr)
{
    return PythonExtensionBase::setattro(name, attr);
}

Py::Object CyPy_UsageInstance::isValid()
{
    auto res = m_value.isValid();
    if (!res.first) {
        Atlas::Objects::Operation::Error e;
        auto& args = e->modifyArgs();
        Atlas::Objects::Entity::Anonymous arg;
        arg->setAttr("message", res.second);
        args.push_back(arg);
        e->setTo(m_value.actor->getId());
        return Py::TupleN(Py::Boolean(res.first), CyPy_Operation::wrap(e));
    }

    return Py::TupleN(Py::Boolean(res.first), Py::None());
}

CyPy_Usage::CyPy_Usage(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{

}

CyPy_Usage::CyPy_Usage(Py::PythonClassInstance* self, Usage value)
    : WrapperBase(self, std::move(value))
{

}

void CyPy_Usage::init_type()
{
    behaviors().name("Usage");
    behaviors().doc("");

    behaviors().readyType();
}

Py::Object CyPy_Usage::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "targets") {
        Py::List list(m_value.targets.size());
        for (size_t i = 0; i < m_value.targets.size(); ++i) {
            list[i] = CyPy_Filter::wrap(m_value.targets[i]);
        }
        return list;
    }
    if (nameStr == "consumed") {
        Py::List list(m_value.consumed.size());
        for (size_t i = 0; i < m_value.consumed.size(); ++i) {
            list[i] = CyPy_Filter::wrap(m_value.consumed[i]);
        }
        return list;
    }
    if (nameStr == "description") {
        return Py::String(m_value.description);
    }
    if (nameStr == "constraint") {
        return CyPy_Filter::wrap(m_value.constraint);
    }

    return PythonExtensionBase::getattro(name);
}

int CyPy_Usage::setattro(const Py::String& name, const Py::Object& attr)
{
    return PythonExtensionBase::setattro(name, attr);
}
