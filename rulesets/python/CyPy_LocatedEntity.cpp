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

#include "CyPy_MemEntity.h"
#include "CyPy_BaseMind.h"
#include "CyPy_Entity.h"

Py::Object wrapLocatedEntity(Ref<LocatedEntity> le)
{
    if (!le->m_scriptEntity.empty()) {
        return boost::any_cast<Py::Object>(le->m_scriptEntity);
    } else {
        Py::Object wrapper;
        auto memEntity = dynamic_cast<MemEntity*>(le.get());
        if (memEntity) {
            wrapper = WrapperBase<Ref<MemEntity>, CyPy_MemEntity>::wrap(memEntity);
        } else {
            auto entity = dynamic_cast<Entity*>(le.get());
            wrapper = WrapperBase<Ref<Entity>, CyPy_Entity>::wrap(entity);
        }

        le->m_scriptEntity = boost::any(wrapper);
        return wrapper;
    }
}


Py::Object CyPy_LocatedEntity::wrap(Ref<LocatedEntity> value)
{
    return wrapLocatedEntity(std::move(value));
}

LocatedEntity& CyPy_LocatedEntity::value(const Py::Object& object)
{
    if (CyPy_Entity::check(object)) {
        return *CyPy_Entity::value(object);
    }
    if (CyPy_MemEntity::check(object)) {
        return *CyPy_MemEntity::value(object);
    }
    throw std::invalid_argument("Supplied value is not a LocatedEntity");
}

bool CyPy_LocatedEntity::check(const Py::Object& object)
{
    return check(object.ptr());
}

bool CyPy_LocatedEntity::check(PyObject* object)
{
    return CyPy_Entity::check(object)
           || CyPy_MemEntity::check(object)
           || CyPy_BaseMind::check(object);
}

