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

#include "CyPy_Physics.h"
#include "CyPy_Location.h"
#include "CyPy_Vector3D.h"
#include "CyPy_Point3D.h"
#include "CyPy_Quaternion.h"
#include "CyPy_Axisbox.h"

CyPy_Physics::CyPy_Physics() : ExtensionModule("physics")
{
    CyPy_Vector3D::init_type();
    CyPy_Point3D::init_type();
    CyPy_Quaternion::init_type();
    CyPy_Axisbox::init_type();


    add_varargs_method("distance_between", &CyPy_Physics::distance_between, "The scalar distance between the edges of two locations.");
    add_varargs_method("distance_to", &CyPy_Physics::distance_to, "");
    add_varargs_method("square_distance", &CyPy_Physics::square_distance, "");
    add_varargs_method("square_horizontal_distance", &CyPy_Physics::square_horizontal_distance, "");
    add_varargs_method("square_horizontal_edge_distance", &CyPy_Physics::square_horizontal_edge_distance, "");


    initialize("Physics");

    Py::Dict d(moduleDictionary());

    d["Vector3D"] = CyPy_Vector3D::type();
    d["Point3D"] = CyPy_Point3D::type();
    d["Quaternion"] = CyPy_Quaternion::type();
    d["BBox"] = CyPy_Axisbox::type();

}

Py::Object CyPy_Physics::distance_to(const Py::Tuple& args)
{
    args.verify_length(2, 2);

    if (!CyPy_Location::check(args[0]) || !CyPy_Location::check(args[1])) {
        throw Py::TypeError("Arg Location required");
    }
    Location& firstLoc = Py::PythonClassObject<CyPy_Location>(args[0]).getCxxObject()->m_value;
    Location& secondLoc = Py::PythonClassObject<CyPy_Location>(args[1]).getCxxObject()->m_value;

    return CyPy_Vector3D::wrap(distanceTo(firstLoc, secondLoc));
}

Py::Object CyPy_Physics::distance_between(const Py::Tuple& args)
{
    args.verify_length(2, 2);

    if (!CyPy_Location::check(args[0]) || !CyPy_Location::check(args[1])) {
        throw Py::TypeError("Arg Location required");
    }

    Location& firstLoc = Py::PythonClassObject<CyPy_Location>(args[0]).getCxxObject()->m_value;
    Location& secondLoc = Py::PythonClassObject<CyPy_Location>(args[1]).getCxxObject()->m_value;

    auto distVector = distanceTo(firstLoc, secondLoc);
    if (distVector.isValid()) {
        return Py::Float(std::max(0.f, distVector.mag() - firstLoc.radius() - secondLoc.radius()));
    } else {
        return Py::None();
    }
}


Py::Object CyPy_Physics::square_distance(const Py::Tuple& args)
{
    args.verify_length(2, 2);

    if (!CyPy_Location::check(args[0]) || !CyPy_Location::check(args[1])) {
        throw Py::TypeError("Arg Location required");
    }
    Location& firstLoc = Py::PythonClassObject<CyPy_Location>(args[0]).getCxxObject()->m_value;
    Location& secondLoc = Py::PythonClassObject<CyPy_Location>(args[1]).getCxxObject()->m_value;

    return Py::Float(squareDistance(firstLoc, secondLoc));
}

Py::Object CyPy_Physics::square_horizontal_distance(const Py::Tuple& args)
{
    args.verify_length(2, 2);

    if (!CyPy_Location::check(args[0]) || !CyPy_Location::check(args[1])) {
        throw Py::TypeError("Arg Location required");
    }
    Location& firstLoc = Py::PythonClassObject<CyPy_Location>(args[0]).getCxxObject()->m_value;
    Location& secondLoc = Py::PythonClassObject<CyPy_Location>(args[1]).getCxxObject()->m_value;

    return Py::Float(squareHorizontalDistance(firstLoc, secondLoc));
}

/**
 * Measures the horizontal distance between the edges of two entities.
 */
Py::Object CyPy_Physics::square_horizontal_edge_distance(const Py::Tuple& args)
{
    args.verify_length(2, 2);

    if (!CyPy_Location::check(args[0]) || !CyPy_Location::check(args[1])) {
        throw Py::TypeError("Arg Location required");
    }
    Location& firstLoc = Py::PythonClassObject<CyPy_Location>(args[0]).getCxxObject()->m_value;
    Location& secondLoc = Py::PythonClassObject<CyPy_Location>(args[1]).getCxxObject()->m_value;

    return Py::Float(squareHorizontalDistance(firstLoc, secondLoc) -
                     boxSquareHorizontalBoundingRadius(firstLoc.m_bBox) -
                     boxSquareHorizontalBoundingRadius(secondLoc.m_bBox));
}

std::string CyPy_Physics::init()
{
    PyImport_AppendInittab("physics", []() {
        static auto module = new CyPy_Physics();
        return module->module().ptr();
    });
    return "physics";
}
