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

#ifndef CYPHESIS_CYPY_VECTOR3D_H
#define CYPHESIS_CYPY_VECTOR3D_H

#include "pythonbase/WrapperBase.h"
#include <wfmath/vector.h>

/**
 * \ingroup PythonWrappers
 */
class CyPy_Vector3D : public WrapperBase<WFMath::Vector<3>, CyPy_Vector3D>
{
    public:
        CyPy_Vector3D(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Vector3D(Py::PythonClassInstance* self, WFMath::Vector<3> value);

        static WFMath::Vector<3> parse(const Py::Object& object);

        static void init_type();

        Py::Object getattro(const Py::String& name) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object repr() override;

        Py::Object iter() override;

        Py::Object rich_compare(const Py::Object&, int) override;

        PyCxx_ssize_t sequence_length() override;

        Py::Object sequence_item(Py_ssize_t) override;

        int sequence_ass_item(Py_ssize_t, const Py::Object&) override;

        Py::Object number_add(const Py::Object& other) override;

        Py::Object number_subtract(const Py::Object& other) override;

        Py::Object number_multiply(const Py::Object& other) override;

        Py::Object number_floor_divide(const Py::Object& other) override;

        Py::Object number_true_divide(const Py::Object& other) override;

        Py::Object number_negative() override;


    protected:


        Py::Object dot(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Vector3D, dot);

        Py::Object cross(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Vector3D, cross);

        Py::Object rotatex(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Vector3D, rotatex);

        Py::Object rotatey(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Vector3D, rotatey);

        Py::Object rotatez(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Vector3D, rotatez);

        Py::Object rotate(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Vector3D, rotate);

        Py::Object angle(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Vector3D, angle);

        Py::Object normalize(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Vector3D, normalize);

        Py::Object sqr_mag();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Vector3D, sqr_mag);

        Py::Object mag();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Vector3D, mag);

        Py::Object is_valid();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Vector3D, is_valid);

        Py::Object unit_vector();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Vector3D, unit_vector);

        Py::Object unit_vector_to(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Vector3D, unit_vector_to);
};


#endif //CYPHESIS_CYPY_VECTOR3D_H
