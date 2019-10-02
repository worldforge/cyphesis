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

#include "CyPy_EntityLocation.h"
#include "CyPy_Location.h"
#include "CyPy_LocatedEntity.h"
#include "CyPy_Point3D.h"
#include "CyPy_Vector3D.h"
#include "CyPy_Quaternion.h"
#include "CyPy_Axisbox.h"

CyPy_EntityLocation::CyPy_EntityLocation(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{

    if (args.length() > 0) {

        m_value.m_parent = verifyObject<CyPy_LocatedEntity>(args[0]);

        if (args.length() == 2) {
            m_value.m_pos = CyPy_Point3D::parse(args[1]);
        }
    }
}

CyPy_EntityLocation::CyPy_EntityLocation(Py::PythonClassInstance* self, EntityLocation value)
    : WrapperBase(self, std::move(value))
{

}

void CyPy_EntityLocation::init_type()
{
    behaviors().name("EntityLocation");
    behaviors().doc("");
    PYCXX_ADD_NOARGS_METHOD(copy, copy, "");

    //behaviors().supportNumberType(Py::PythonType::support_number_subtract);

    behaviors().readyType();

}

Py::Object CyPy_EntityLocation::copy()
{
    return wrap(m_value);
}

Py::Object CyPy_EntityLocation::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if ("parent" == nameStr || "entity" == nameStr) {
        if (!m_value.m_parent) {
            return Py::None();
        }
        return CyPy_LocatedEntity::wrap(m_value.m_parent);
    }
    if ("pos" == nameStr) {
        return CyPy_Point3D::wrap(m_value.m_pos);
    }
    return PythonExtensionBase::getattro(name);
}

int CyPy_EntityLocation::setattro(const Py::String& name, const Py::Object& attr)
{

    auto nameStr = name.as_string();
    if ("parent" == nameStr || "entity" == nameStr) {
        if (!CyPy_LocatedEntity::check(attr)) {
            throw Py::TypeError("parent must be an entity");
        }
        m_value.m_parent = verifyObject<CyPy_LocatedEntity>(attr);

        return 0;
    }

    if ("pos" == nameStr) {
        m_value.m_pos = CyPy_Point3D::parse(attr);
        return 0;
    }
    throw Py::AttributeError("unknown attribute");
}

//Py::Object CyPy_EntityLocation::repr()
//{
//    std::stringstream r;
//    r << m_value;
//    return Py::String(r.str());
//}

//Py::Object CyPy_EntityLocation::number_subtract(const Py::Object& other)
//{
//    return CyPy_Vector3D::wrap(distanceTo(CyPy_EntityLocation::value(other), m_value));
//}
