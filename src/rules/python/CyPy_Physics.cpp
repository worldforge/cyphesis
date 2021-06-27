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
#include "CyPy_Ball.h"

CyPy_Physics::CyPy_Physics() : ExtensionModule("physics")
{
    CyPy_Vector3D::init_type();
    CyPy_Point3D::init_type();
    CyPy_Quaternion::init_type();
    CyPy_Axisbox::init_type();
    CyPy_Ball::init_type();

    initialize("Physics");

    Py::Dict d(moduleDictionary());

    d["Vector3D"] = CyPy_Vector3D::type();
    d["Point3D"] = CyPy_Point3D::type();
    d["Quaternion"] = CyPy_Quaternion::type();
    d["BBox"] = CyPy_Axisbox::type();
    d["Ball"] = CyPy_Ball::type();

}


std::string CyPy_Physics::init()
{
    PyImport_AppendInittab("physics", []() {
        static auto module = new CyPy_Physics();
        return module->module().ptr();
    });
    return "physics";
}
