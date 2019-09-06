/*
 Copyright (C) 2019 Erik Ogenvik

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

#include <rules/python/CyPy_Ball.h>
#include <rules/python/CyPy_Vector3D.h>
#include <rules/python/CyPy_Point3D.h>
#include "CyPy_Domain.h"
#include "rules/LocatedEntity.h"
#include "CyPy_Entity.h"

CyPy_Domain::CyPy_Domain(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    throw Py::RuntimeError("Can not create Domain instances.");
}

CyPy_Domain::CyPy_Domain(Py::PythonClassInstance* self, Ref<LocatedEntity> value)
    : WrapperBase(self, std::move(value))
{

}

CyPy_Domain::~CyPy_Domain() = default;

void CyPy_Domain::init_type()
{
    behaviors().name("Domain");
    behaviors().doc("");

    PYCXX_ADD_VARARGS_METHOD(query_collisions, query_collisions, "");

    behaviors().readyType();

    CyPy_CollisionEntry::init_type();
}

Py::Object CyPy_Domain::query_collisions(const Py::Tuple& args)
{
    args.verify_length(1);
    auto sphere = verifyObject<CyPy_Ball>(args.front());
    Py::List list;
    auto domain = m_value->getDomain();
    if (domain) {
        auto collisions = domain->queryCollision(sphere);
        for (auto& collision : collisions) {
            list.append(CyPy_CollisionEntry::wrap(collision));
        }
    }
    return list;
}

CyPy_CollisionEntry::CyPy_CollisionEntry(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    throw Py::RuntimeError("Can not create Domain instances.");
}


CyPy_CollisionEntry::CyPy_CollisionEntry(Py::PythonClassInstance* self, Domain::CollisionEntry value)
    : WrapperBase(self, std::move(value))
{
}

void CyPy_CollisionEntry::init_type()
{
    behaviors().name("CollisionEntry");
    behaviors().doc("");

    behaviors().readyType();
}

Py::Object CyPy_CollisionEntry::getattro(const Py::String& name)
{
    auto as_string = name.as_string();
    if (as_string == "collisionPoint") {
        return CyPy_Point3D::wrap(m_value.collisionPoint);
    }
    if (as_string == "entity") {
        if (m_value.entity) {
            return CyPy_Entity::wrap(m_value.entity);
        }
        return Py::None();
    }
    if (as_string == "distance") {
        return Py::Float(m_value.distance);
    }
    return PythonExtensionBase::getattro(name);
}


CyPy_CollisionEntry::~CyPy_CollisionEntry() = default;

