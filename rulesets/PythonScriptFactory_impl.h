// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef RULESETS_PYTHON_SCRIPT_FACTORY_IMPL_H
#define RULESETS_PYTHON_SCRIPT_FACTORY_IMPL_H

#include <rulesets/python/CyPy_BaseMind.h>
#include "rulesets/python/CyPy_LocatedEntity.h"
#include "rulesets/python/CyPy_Task.h"
#include "rulesets/BaseMind.h"
#include "PythonScriptFactory.h"

#include "PythonEntityScript.h"
#include "Python_Script_Utils.h"

/// \brief PythonScriptFactory constructor
///
/// @param package Name of the script package where the script type is
/// @param type Name of the script types instanced by this factory

Py::Object wrapPython(LocatedEntity* value) {
    return CyPy_LocatedEntity::wrap(value);
}
Py::Object wrapPython(BaseMind* value) {
    return CyPy_BaseMind::wrap(value);
}
Py::Object wrapPython(Task* value) {
    return CyPy_Task::wrap(value);
}

template <class T>
int PythonScriptFactory<T>::setup()
{
    return load();
}

template <class T>
const std::string & PythonScriptFactory<T>::package() const
{
    return m_package;
}

template<class T>
Py::Object PythonScriptFactory<T>::createScript(T* entity) const
{
    if (!this->m_class || this->m_class->isNull()) {
        return Py::Null();
    }
    auto wrapper = wrapPython(entity);
    if (wrapper.isNull()) {
        return Py::Null();
    }
    try {
        return this->m_class->apply(Py::TupleN(wrapper));
    } catch (...) {
        log(ERROR, String::compose("Error when creating script '%1.%2'.", this->m_package, this->m_type));
        if (PyErr_Occurred() != nullptr) {
            PyErr_Print();
        }
        return Py::None();
    }
}

template <class T>
int PythonScriptFactory<T>::addScript(T * entity) const
{
    auto script = createScript(entity);
    if (!script.isNone()) {
        auto scriptInstance = new PythonEntityScript(script);
        entity->setScript(scriptInstance);
    }

    return (script.isNull()) ? -1 : 0;
}

template <class T>
int PythonScriptFactory<T>::refreshClass()
{
    return refresh();
}

#endif // RULESETS_PYTHON_SCRIPT_FACTORY_IMPL_H
