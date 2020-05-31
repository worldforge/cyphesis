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

#include <rules/simulation/TerrainProperty.h>
#include "CyPy_TerrainProperty.h"
#include "rules/python/CyPy_Vector3D.h"
#include "rules/python/CyPy_LocatedEntity.h"

CyPy_TerrainProperty::CyPy_TerrainProperty(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
        : WrapperBase(self, args, kwds)
{

}

CyPy_TerrainProperty::CyPy_TerrainProperty(Py::PythonClassInstance* self, Ref<LocatedEntity> value)
        : WrapperBase(self, std::move(value))
{

}

CyPy_TerrainProperty::~CyPy_TerrainProperty() = default;

void CyPy_TerrainProperty::init_type()
{
    behaviors().name("TerrainProperty");
    behaviors().doc("");

    PYCXX_ADD_VARARGS_METHOD(get_height, getHeight, "");
    PYCXX_ADD_VARARGS_METHOD(get_surface, getSurface, R"(Gets the numerical index of the surface that's most prevalent at the location.
Parameters:
x : The x position
y : The y position)");
    PYCXX_ADD_VARARGS_METHOD(get_surface_name, getSurfaceName, R"(Gets the name of the surface that's most prevalent at the location.
Parameters:
x : The x position
y : The y position)");
    PYCXX_ADD_VARARGS_METHOD(get_normal, getNormal, "");
    PYCXX_ADD_VARARGS_METHOD(find_mods, findMods, "");

    behaviors().readyType();
}

const TerrainProperty& CyPy_TerrainProperty::getTerrainProperty()
{
    auto prop = m_value->getPropertyClassFixed<TerrainProperty>();
    if (!prop) {
        throw Py::RuntimeError("This entity has no terrain.");
    }
    return *prop;
}


Py::Object CyPy_TerrainProperty::getHeight(const Py::Tuple& args)
{
    args.verify_length(2);
    float h = 0;
    getTerrainProperty().getHeight(*m_value, verifyNumeric(args[0]), verifyNumeric(args[1]), h);
    return Py::Float(h);
}

Py::Object CyPy_TerrainProperty::getSurface(const Py::Tuple& args)
{
    args.verify_length(2);
    auto surface = getTerrainProperty().getSurface(*m_value, verifyNumeric(args[0]), verifyNumeric(args[1]));
    if (surface) {
        return Py::Long(*surface);
    }
    return Py::None();
}

Py::Object CyPy_TerrainProperty::getSurfaceName(const Py::Tuple& args)
{
    args.verify_length(2);
    auto surface = getTerrainProperty().getSurface(*m_value, verifyNumeric(args[0]), verifyNumeric(args[1]));
    if (surface) {
        auto& surfaceNames = getTerrainProperty().getSurfaceNames(*m_value);
        if (*surface >= 0 && *surface < (int) surfaceNames.size()) {
            return Py::String(surfaceNames[*surface]);
        }
    }
    return Py::None();
}

Py::Object CyPy_TerrainProperty::getNormal(const Py::Tuple& args)
{
    args.verify_length(2);
    float h = 0;
    Vector3D normal(0, 1, 0);
    getTerrainProperty().getHeightAndNormal(*m_value, verifyNumeric(args[0]), verifyNumeric(args[1]), h, normal);
    return CyPy_Vector3D::wrap(normal);
}

Py::Object CyPy_TerrainProperty::findMods(const Py::Tuple& args)
{
    args.verify_length(2);

    Py::List list;
    auto entities = getTerrainProperty().findMods(*m_value, verifyNumeric(args[0]), verifyNumeric(args[1]));
    if (entities) {
        for (auto& entity : *entities) {
            list.append(CyPy_LocatedEntity::wrap(entity));
        }
    }
    return list;
}
