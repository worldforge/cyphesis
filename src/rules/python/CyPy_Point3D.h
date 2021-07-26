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

#ifndef CYPHESIS_CYPY_POINT3D_H
#define CYPHESIS_CYPY_POINT3D_H

#include "pycxx/CXX/Objects.hxx"
#include "pycxx/CXX/Extensions.hxx"
#include "pythonbase/WrapperBase.h"
#include <wfmath/point.h>

/**
 * \ingroup PythonWrappers
 */
class CyPy_Point3D : public WrapperBase<WFMath::Point<3>, CyPy_Point3D>
{
    public:
        CyPy_Point3D(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Point3D(Py::PythonClassInstance* self, WFMath::Point<3> value);

        static WFMath::Point<3> parse(const Py::Object& object);

        static void init_type();

        Py::Object getattro(const Py::String&) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object repr() override;

        Py::Object rich_compare(const Py::Object& other, int type) override;

        PyCxx_ssize_t sequence_length() override;

        Py::Object sequence_item(Py_ssize_t pos) override;

        int sequence_ass_item(Py_ssize_t pos, const Py::Object& other) override;

        Py::Object number_add(const Py::Object& other) override;

        Py::Object number_subtract(const Py::Object& other) override;

        Py::Object iter() override;

    protected:


        Py::Object mag();
        PYCXX_NOARGS_METHOD_DECL(CyPy_Point3D, mag);

        Py::Object unit_vector_to(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_Point3D, unit_vector_to);

        Py::Object is_valid();
        PYCXX_NOARGS_METHOD_DECL(CyPy_Point3D, is_valid);

        Py::Object distance(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_Point3D, distance);


};


#endif //CYPHESIS_CYPY_POINT3D_H
