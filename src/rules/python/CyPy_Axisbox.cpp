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

#include <physics/BBox.h>
#include "CyPy_Axisbox.h"
#include "CyPy_Element.h"
#include "CyPy_Point3D.h"
#include "CoordHelper.h"

CyPy_Axisbox::CyPy_Axisbox(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    std::vector<float> val;

    switch (args.size()) {
        case 0:
            break;
        case 1: {
            auto arg = args.front();
            if (!arg.isSequence()) {
                throw Py::TypeError("BBox() from single value must a sequence 3 or 6 long");
            }
            Py::Sequence sequence(arg);
            auto list_size = sequence.size();
            if (list_size != 3 && list_size != 6) {
                throw Py::ValueError("BBox() from single value must a sequence 3 or 6 long");
            }
            val.resize(list_size);
            for (int i = 0; i < list_size; i++) {
                Py::Object item = sequence.getItem(i);
                if (item.isNumeric()) {
                    val[i] = Py::Float(item).as_double();
                } else {
                    throw Py::TypeError("BBox() must take list of floats, or ints");
                }
            }
        }
            break;
        case 3:
        case 6:
            val.resize(args.size());
            for (int i = 0; i < args.size(); i++) {
                Py::Object item = args.getItem(i);
                if (item.isNumeric()) {
                    val[i] = Py::Float(item).as_double();
                } else {
                    throw Py::TypeError("BBox() must take list of floats, or ints");
                }
            }
            break;
        default:
            throw Py::TypeError("BBox must take list of floats, or ints, 3 ints or 3 floats");
    }
    if (val.size() == 3) {
        m_value = WFMath::AxisBox<3>(WFMath::Point<3>(0.f, 0.f, 0.f),
                                     WFMath::Point<3>(val[0], val[1], val[2]));
    } else if (val.size() == 6) {
        m_value = WFMath::AxisBox<3>(WFMath::Point<3>(val[0], val[1], val[2]),
                                     WFMath::Point<3>(val[3], val[4], val[5]));
    }

}

CyPy_Axisbox::CyPy_Axisbox(Py::PythonClassInstance* self, WFMath::AxisBox<3> value)
    : WrapperBase(self, std::move(value))
{
}

void CyPy_Axisbox::init_type()
{
    behaviors().name("BBox");
    behaviors().doc("");

    behaviors().supportRichCompare();

    PYCXX_ADD_NOARGS_METHOD(square_bounding_radius, sqr_bounding_radius, "");
    PYCXX_ADD_NOARGS_METHOD(square_horizontal_bounding_radius, sqr_horizontal_bounding_radius, "");
    PYCXX_ADD_NOARGS_METHOD(as_sequence, as_sequence, "");

    behaviors().readyType();
}


Py::Object CyPy_Axisbox::sqr_bounding_radius()
{
    float square_radius = 0;
    if (m_value.isValid()) {
        square_radius = ::boxSquareBoundingRadius(m_value);
    }
    return Py::Float(square_radius);
}

Py::Object CyPy_Axisbox::sqr_horizontal_bounding_radius()
{
    float square_radius = 0;
    if (m_value.isValid()) {
        square_radius = ::boxSquareHorizontalBoundingRadius(m_value);
    }
    return Py::Float(square_radius);
}


Py::Object CyPy_Axisbox::as_sequence()
{
    Py::List list(6);
    list[0] = Py::Float(m_value.lowCorner().x());
    list[1] = Py::Float(m_value.lowCorner().y());
    list[2] = Py::Float(m_value.lowCorner().z());
    list[3] = Py::Float(m_value.highCorner().x());
    list[4] = Py::Float(m_value.highCorner().y());
    list[5] = Py::Float(m_value.highCorner().z());
    return list;
}

Py::Object CyPy_Axisbox::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "low_corner") {
        return CyPy_Point3D::wrap(m_value.lowCorner());
    }
    if (nameStr == "high_corner") {
        return CyPy_Point3D::wrap(m_value.highCorner());
    }
    if (nameStr == "center") {
        return CyPy_Point3D::wrap(m_value.getCenter());
    }

    return PythonExtensionBase::getattro(name);
}

int CyPy_Axisbox::setattro(const Py::String& name, const Py::Object& attr)
{
    auto nameStr = name.as_string();
    if (!CyPy_Point3D::check(attr)) {
        throw Py::TypeError("BBox setattr must take a Point");
    }
    auto& point = CyPy_Point3D::value(attr);
    if (!point.isValid()) {
        throw Py::ValueError("BBox setattr must take a valid Point");
    }
    if (nameStr == "low_corner") {
        m_value.lowCorner() = point;
        return 0;
    } else if (nameStr == "high_corner") {
        m_value.highCorner() = point;
        return 0;
    }

    return PythonExtensionBase::setattro(name, attr);
}

Py::Object CyPy_Axisbox::rich_compare(const Py::Object& other, int type)
{
    return CoordHelper::rich_compare<decltype(m_value), CyPy_Axisbox>(m_value, other, type);
}
