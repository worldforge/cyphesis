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

#ifndef CYPHESIS_CYPY_TERRAINPROPERTY_H
#define CYPHESIS_CYPY_TERRAINPROPERTY_H


#include "rules/LocatedEntity.h"
#include "modules/Ref.h"
#include "pythonbase/WrapperBase.h"

class TerrainProperty;
/**
 * \ingroup PythonWrappers
 */
class CyPy_TerrainProperty: public WrapperBase<Ref<LocatedEntity>, CyPy_TerrainProperty>
{
    public:
        CyPy_TerrainProperty(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_TerrainProperty(Py::PythonClassInstance* self, Ref<LocatedEntity> value);

        ~CyPy_TerrainProperty() override;

        static void init_type();

        const TerrainProperty& getTerrainProperty();

        Py::Object getHeight(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_TerrainProperty, getHeight);

        Py::Object getSurface(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_TerrainProperty, getSurface);

        Py::Object getSurfaceName(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_TerrainProperty, getSurfaceName);

        Py::Object getNormal(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_TerrainProperty, getNormal);

        Py::Object findMods(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_TerrainProperty, findMods);

};


#endif //CYPHESIS_CYPY_TERRAINPROPERTY_H
