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

#include <physics/Quaternion.h>
#include "CyPy_Quaternion.h"
#include "CyPy_Vector3D.h"
#include "CoordHelper.h"

CyPy_Quaternion::CyPy_Quaternion(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    switch (args.size()) {
        case 0:
            break;
        case 1: {
            auto arg = args.front();
            if (!arg.isSequence()) {
                throw Py::TypeError("Quaternion() from single value must a list");
            }
            Py::Sequence seq(arg);
            if (seq.size() != 4) {
                throw Py::ValueError("Quaternion() from a list must be 4 long");
            }
            {
                float quaternion[4];
                for (int i = 0; i < 4; i++) {
                    auto item = seq.getItem(i);
                    if (!item.isNumeric()) {
                        throw Py::TypeError("Quaternion() must take list of floats, or ints");
                    } else {
                        quaternion[i] = (WFMath::CoordType) Py::Float(item).as_double();
                    }
                }
                m_value = WFMath::Quaternion(quaternion[3], quaternion[0],
                                             quaternion[1], quaternion[2]);
            }
        }
            break;
        case 2: {
            if (!CyPy_Vector3D::check(args[0])) {
                throw Py::TypeError("Quaternion(a,b) must take a vector as first argument");
            }
            if (Py::Object(args[1]).isFloat()) {
                m_value.rotation(CyPy_Vector3D::value(args[0]), Py::Float(args[1]));
            } else {
                throw Py::TypeError("Quaternion(a,b) must take a float as second argument");
            }
        }
            break;
        case 3: {
            if (!CyPy_Vector3D::check(args[0])) {
                throw Py::TypeError("Quaternion(a,b,fallbackAxis) must take a vector as first argument");
            }
            if (CyPy_Vector3D::check(args[1])) {
                if (!CyPy_Vector3D::check(args[2])) {
                    throw Py::TypeError("Quaternion(a,b,fallbackAxis) must take a vector as third argument");
                }

                m_value = ::quaternionFromTo(CyPy_Vector3D::value(args[0]), CyPy_Vector3D::value(args[1]), CyPy_Vector3D::value(args[2]));
            } else {
                throw Py::TypeError("Quaternion(a,b,fallbackAxis) must take a vector as second argument");
            }
        }
            break;
        case 4: {
            float quaternion[4];
            for (int i = 0; i < 4; i++) {
                if (args[i].isNumeric()) {
                    quaternion[i] = static_cast<float>(Py::Float(args[i]));
                } else {
                    throw Py::TypeError("Quaternion() must take list of floats, or ints");
                }
            }
            m_value = WFMath::Quaternion(quaternion[3], quaternion[0],
                                         quaternion[1], quaternion[2]);
        }
            break;
        default:
            throw Py::TypeError("Quaternion must take list of floats, or ints, 4 ints or 4 floats");
    }

}

CyPy_Quaternion::CyPy_Quaternion(Py::PythonClassInstance* self, WFMath::Quaternion value)
    : WrapperBase(self, std::move(value))
{

}

void CyPy_Quaternion::init_type()
{
    behaviors().name("Quaternion");
    behaviors().doc("");

    behaviors().supportRepr();
    behaviors().supportRichCompare();


    behaviors().supportNumberType(Py::PythonType::support_number_multiply);

    PYCXX_ADD_VARARGS_METHOD(rotation, rotation, "");

    PYCXX_ADD_NOARGS_METHOD(as_list, as_list, "");
    PYCXX_ADD_NOARGS_METHOD(is_valid, is_valid, "");


    behaviors().readyType();
}


Py::Object CyPy_Quaternion::as_list()
{
    Py::List list;
    list.append(Py::Float(m_value.vector().x()));
    list.append(Py::Float(m_value.vector().y()));
    list.append(Py::Float(m_value.vector().z()));
    list.append(Py::Float(m_value.scalar()));
    return list;
}

Py::Object CyPy_Quaternion::is_valid()
{
    return CoordHelper::is_valid(m_value);
}

Py::Object CyPy_Quaternion::rotation(const Py::Tuple& args)
{
    args.verify_length(2, 2);
    if (!CyPy_Vector3D::check(args[0])) {
        throw Py::TypeError("First argument must be a Vector3D");
    }
    if (!args[1].isNumeric()) {
        throw Py::TypeError("Second argument must be a number");
    }

    m_value.rotation(CyPy_Vector3D::value(args[0]), Py::Float(args[0]));

    return self();
}

Py::Object CyPy_Quaternion::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "x") { return Py::Float(m_value.vector().x()); }
    if (nameStr == "y") { return Py::Float(m_value.vector().y()); }
    if (nameStr == "z") { return Py::Float(m_value.vector().z()); }
    if (nameStr == "w") { return Py::Float(m_value.scalar()); }

    return PythonExtensionBase::getattro(name);
}

Py::Object CyPy_Quaternion::repr()
{
    char buf[128];
    ::snprintf(buf, 128, "(%f, (%f, %f, %f))", m_value.scalar(),
               m_value.vector().x(), m_value.vector().y(),
               m_value.vector().z());
    return Py::String(buf);
}

Py::Object CyPy_Quaternion::rich_compare(const Py::Object& other, int type)
{
    return CoordHelper::rich_compare<decltype(m_value), CyPy_Quaternion>(m_value, other, type);
}

Py::Object CyPy_Quaternion::number_multiply(const Py::Object& other)
{
    if (!CyPy_Quaternion::check(other)) {
        throw Py::TypeError("Quaternion must be multiplied by Quaternion");
    }
    return CyPy_Quaternion::wrap(m_value * CyPy_Quaternion::value(other));
}
