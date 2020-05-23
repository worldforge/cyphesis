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
#include "rules/python/CyPy_LocatedEntity.h"
#include "rules/python/CyPy_EntityLocation.h"
#include "rules/entityfilter/python/CyPy_EntityFilter.h"
#include "rules/python/CyPy_Operation.h"
#include "modules/Variant.h"
#include "rules/python/CyPy_Point3D.h"
#include "rules/python/CyPy_Vector3D.h"


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

    PYCXX_ADD_VARARGS_METHOD(get_arg, getArg, "");

    behaviors().readyType();

    UsageInstance::scriptCreator = [](UsageInstance&& usageInstance){
        return wrap(std::move(usageInstance));
    };
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
        return Py::TupleN(Py::Boolean(res.first), Py::String(res.second));
    }

    return Py::TupleN(Py::Boolean(res.first), Py::None());
}

Py::Object CyPy_UsageInstance::getArg(const Py::Tuple& args)
{
    return getArg(m_value, args);
}

Py::Object CyPy_UsageInstance::getArg(const UsageInstance& usageInstance, const Py::Tuple& args)
{
    args.verify_length(2);
    auto key = verifyString(args[0]);
    auto index = verifyLong(args[1]);

    auto I = usageInstance.args.find(key);
    if (I == usageInstance.args.end()) {
        return Py::None();
    }

    auto& vector = I->second;
    if (static_cast<unsigned long>(index) >= vector.size()) {
        return Py::None();
    }

    auto& vector_instance = vector[index];

    Py::Object returnObject;

    auto visitor = compose(
        [&](const EntityLocation& value) {
            returnObject = CyPy_EntityLocation::wrap(value);
        },
        [&](const WFMath::Point<3>& value) {
            returnObject = CyPy_Point3D::wrap(value);
        },
        [&](const WFMath::Vector<3>& value) {
            returnObject = CyPy_Vector3D::wrap(value);
        }
    );

    boost::apply_visitor(visitor, vector_instance);
    return returnObject;
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
