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

#include <rulesets/Py_Location.h>
#include <rulesets/Py_Thing.h>

#include "CyPy_MemEntity.h"
#include "CyPy_Character.h"
#include "CyPy_BaseMind.h"
#include "CyPy_Entity.h"

Py::Object wrapLocatedEntity(LocatedEntity* le)
{
    if (!le->m_scriptEntity.empty()) {
        return boost::any_cast<Py::Object>(le->m_scriptEntity);
    } else {
        Py::Object wrapper;
        auto baseMind = dynamic_cast<BaseMind*>(le);
        if (baseMind) {
            wrapper = WrapperBase<BaseMind*, CyPy_BaseMind>::wrap(baseMind);
        } else {
            auto character = dynamic_cast<Character*>(le);
            if (character) {
                wrapper = WrapperBase<Character*, CyPy_Character>::wrap(character);
            } else {

                auto memEntity = dynamic_cast<MemEntity*>(le);
                if (memEntity) {
                    wrapper = WrapperBase<MemEntity*, CyPy_MemEntity>::wrap(memEntity);
                } else {
                    auto entity = dynamic_cast<Entity*>(le);
                    wrapper = WrapperBase<Entity*, CyPy_Entity>::wrap(entity);
                }
            }
        }

        le->m_scriptEntity = boost::any(wrapper);
        return wrapper;
    }
}


Py::Object CyPy_LocatedEntity::wrap(LocatedEntity* value)
{
    return wrapLocatedEntity(value);
}

LocatedEntity& CyPy_LocatedEntity::value(const Py::Object& object)
{
    if (CyPy_Entity::check(object)) {
        return *CyPy_Entity::value(object);
    }
    if (CyPy_MemEntity::check(object)) {
        return *CyPy_MemEntity::value(object);
    }
    if (CyPy_BaseMind::check(object)) {
        return *CyPy_BaseMind::value(object);
    }
    return *CyPy_Character::value(object);

}

bool CyPy_LocatedEntity::check(const Py::Object& object)
{
    return check(object.ptr());
}

bool CyPy_LocatedEntity::check(PyObject* object)
{
    return CyPy_Entity::check(object)
           || CyPy_MemEntity::check(object)
           || CyPy_BaseMind::check(object)
           || CyPy_Character::check(object);
}

