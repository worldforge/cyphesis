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

#include "CyPy_LocatedEntity.h"

std::vector<LocatedEntityScriptProvider> CyPy_LocatedEntity::entityPythonProviders;


Py::Object wrapLocatedEntity(Ref<LocatedEntity> le)
{
    //If there's already a script entity use that (as a cache mechanism)
    if (!le->m_scriptEntity.empty()) {
        auto wrapper = boost::any_cast<Py::Object>(le->m_scriptEntity);
        if (!wrapper.isNone()) {
            auto object = PyWeakref_GetObject(wrapper.ptr());
            if (object) {
                Py::Object pythonObj(object);
                if (!pythonObj.isNone()) {
                    return pythonObj;
                }
            }
        }
    }
    for (auto& provider : CyPy_LocatedEntity::entityPythonProviders) {
        auto wrapped = provider.wrapFn(le);
        if (!wrapped.isNone()) {

            auto weakPtr = PyWeakref_NewRef(wrapped.ptr(), nullptr);
            le->m_scriptEntity = boost::any(Py::Object(weakPtr, true));

            return wrapped;
        }
    }
    throw Py::TypeError(String::compose("Tried to wrap located entity '%1' but could not find any wrapper provider.", le->describeEntity()));

}


Py::Object CyPy_LocatedEntity::wrap(Ref<LocatedEntity> value)
{
    return wrapLocatedEntity(std::move(value));
}

Ref<LocatedEntity>& CyPy_LocatedEntity::value(const Py::Object& object)
{
    for (auto& provider : entityPythonProviders) {
        auto value = provider.valueFn(object);
        if (value) {
            return *value;
        }
    }

    throw std::invalid_argument("Supplied value is not a LocatedEntity");
}

bool CyPy_LocatedEntity::check(const Py::Object& object)
{
    return check(object.ptr());
}

bool CyPy_LocatedEntity::check(PyObject* object)
{
    for (auto& provider : entityPythonProviders) {
        auto value = provider.checkFn(object);
        if (value) {
            return value;
        }
    }
    return false;
}

