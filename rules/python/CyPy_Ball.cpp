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

#include "CyPy_Ball.h"
#include "CyPy_Point3D.h"
#include "CyPy_Element.h"

#include <wfmath/atlasconv.h>

CyPy_Ball::CyPy_Ball(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    args.verify_length(2);
    m_value.center() = verifyObject<CyPy_Point3D>(args.front());
    m_value.radius() = verifyFloat(args[1]);
}

CyPy_Ball::CyPy_Ball(Py::PythonClassInstance* self, WFMath::Ball<3> value)
    : WrapperBase(self, std::move(value))
{

}

void CyPy_Ball::init_type()
{
    behaviors().name("Ball");
    behaviors().doc("");

    behaviors().supportRepr();
    behaviors().supportRichCompare();


    behaviors().readyType();
}

WFMath::Ball<3> CyPy_Ball::parse(const Py::Object& object)
{
    return WFMath::Ball<3>(verifyObject<CyPy_ElementMap>(object));
}

Py::Object CyPy_Ball::repr()
{
    char buf[64];
    ::snprintf(buf, 64, "(%f, %f, %f), %f", m_value.center().x(), m_value.center().y(), m_value.center().z(), m_value.radius());
    return Py::String(buf);
}

Py::Object CyPy_Ball::rich_compare(const Py::Object& other, int type)
{
    if (type == Py_EQ) {
        if (!CyPy_Ball::check(other)) {
            return Py::False();
        }
        return Py::Boolean(m_value == CyPy_Ball::value(other));
    } else if (type == Py_NE) {
        if (!CyPy_Ball::check(other)) {
            return Py::True();
        }
        return Py::Boolean(m_value != CyPy_Ball::value(other));
    }
    throw Py::NotImplementedError("Not implemented");
}

