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

#include "CyPy_Location.h"
#include "CyPy_LocatedEntity.h"
#include "CyPy_Point3D.h"
#include "CyPy_Vector3D.h"
#include "CyPy_Quaternion.h"
#include "CyPy_Axisbox.h"

CyPy_Location::CyPy_Location(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{

    if (args.length() > 0) {

        if (!CyPy_LocatedEntity::check(args[0])) {
            throw Py::TypeError("First arg must be a LocatedEntity.");
        }
        m_value.m_loc = Py::PythonClassObject<CyPy_LocatedEntity>(args[0]).getCxxObject()->m_value;

        if (args.length() == 2) {
            m_value.m_pos = CyPy_Point3D::parse(args[1]);
        }
    }
}

CyPy_Location::CyPy_Location(Py::PythonClassInstance* self, Location value)
    : WrapperBase(self, std::move(value))
{

}

void CyPy_Location::init_type()
{
    behaviors().name("Location");
    behaviors().doc("");
    PYCXX_ADD_NOARGS_METHOD(copy, copy, "");

    behaviors().supportNumberType(Py::PythonType::support_number_subtract);

    behaviors().readyType();

}

Py::Object CyPy_Location::copy()
{
    return wrap(m_value);
}

Py::Object CyPy_Location::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    //FIXME: rename to "loc"
    if ("parent" == nameStr) {
        if (!m_value.m_loc) {
            return Py::None();
        }
        return wrapLocatedEntity(m_value.m_loc);
    }
    //FIXME: rename to "pos"
    if ("coordinates" == nameStr) {
        return CyPy_Point3D::wrap(m_value.m_pos);
    }
    if ("velocity" == nameStr) {
        return CyPy_Vector3D::wrap(m_value.m_velocity);
    }
    if ("orientation" == nameStr) {
        return CyPy_Quaternion::wrap(m_value.orientation());
    }
    if ("bbox" == nameStr) {
        return CyPy_Axisbox::wrap(m_value.bBox());
    }
    return PythonExtensionBase::getattro(name);
}

int CyPy_Location::setattro(const Py::String& name, const Py::Object& attr)
{

    auto nameStr = name.as_string();
    //FIXME: rename to "loc"
    if (nameStr == "parent") {
        if (!CyPy_LocatedEntity::check(attr)) {
            throw Py::TypeError("parent must be an entity");
        }
        m_value.m_loc = Py::PythonClassObject<CyPy_LocatedEntity>(attr).getCxxObject()->m_value;

        return 0;
    }
    if (nameStr == "bbox" && CyPy_Axisbox::check(attr)) {
        m_value.setBBox(Py::PythonClassObject<CyPy_Axisbox>(attr).getCxxObject()->m_value);
        return 0;
    }
    if (nameStr == "orientation" && CyPy_Quaternion::check(attr)) {
        m_value.m_orientation = Py::PythonClassObject<CyPy_Quaternion>(attr).getCxxObject()->m_value;
        return 0;
    }
    Vector3D vector;
    if (CyPy_Vector3D::check(attr)) {
        vector = Py::PythonClassObject<CyPy_Vector3D>(attr).getCxxObject()->m_value;
    } else if (CyPy_Point3D::check(attr)) {
        vector = WFMath::Vector<3>(Py::PythonClassObject<CyPy_Point3D>(attr).getCxxObject()->m_value);
    } else if (attr.isSequence()) {
        Py::Sequence seq(attr);
        if (seq.length() != 3) {
            throw Py::ValueError("value must be sequence of 3");
        }
        for (int i = 0; i < 3; i++) {
            if (seq[i].isNumeric()) {
                vector[i] = Py::Float(seq[i]).as_double();
            } else {
                throw Py::TypeError("value must be tuple of floats, or ints");
            }
        }
        vector.setValid();
    } else {
        throw Py::TypeError("value must be a vector");
    }
    //FIXME: rename to "pos"
    if (nameStr == "coordinates") {
        m_value.m_pos = Point3D(vector.x(), vector.y(), vector.z());
        return 0;
    }
    if (nameStr == "velocity") {
        m_value.m_velocity = vector;
        return 0;
    }
    if (nameStr == "bbox") {
        m_value.setBBox(BBox(WFMath::Point<3>(0.f, 0.f, 0.f),
                             WFMath::Point<3>(vector.x(),
                                              vector.y(),
                                              vector.z())));
        return 0;
    }
    throw Py::AttributeError("unknown attribute");
}

Py::Object CyPy_Location::repr()
{
    std::stringstream r;
    r << m_value;
    return Py::String(r.str());
}

Py::Object CyPy_Location::number_subtract(const Py::Object& other)
{
    if (!CyPy_Location::check(other)) {
        throw Py::TypeError("Location must subtract Location");
    }

    auto& otherLocation = Py::PythonClassObject<CyPy_Location>(other).getCxxObject()->m_value;
    return CyPy_Vector3D::wrap(distanceTo(otherLocation, m_value));
}

