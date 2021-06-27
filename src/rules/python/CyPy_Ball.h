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

#ifndef CYPHESIS_CYPY_BALL_H
#define CYPHESIS_CYPY_BALL_H

#include "pycxx/CXX/Objects.hxx"
#include "pycxx/CXX/Extensions.hxx"
#include "pythonbase/WrapperBase.h"
#include <wfmath/ball.h>

/**
 * \ingroup PythonWrappers
 */
class CyPy_Ball : public WrapperBase<WFMath::Ball<3>, CyPy_Ball>
{
    public:
        CyPy_Ball(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Ball(Py::PythonClassInstance* self, WFMath::Ball<3> value);

        static WFMath::Ball<3> parse(const Py::Object& object);

        static void init_type();

        Py::Object repr() override;

        Py::Object rich_compare(const Py::Object& other, int type) override;

    protected:


//Py::Object mag();
//PYCXX_NOARGS_METHOD_DECL(CyPy_Point3D, mag);
//
//Py::Object unit_vector_to(const Py::Tuple& args);
//PYCXX_VARARGS_METHOD_DECL(CyPy_Point3D, unit_vector_to);
//
//Py::Object is_valid();
//PYCXX_NOARGS_METHOD_DECL(CyPy_Point3D, is_valid);
//
//Py::Object distance(const Py::Tuple& args);
//PYCXX_VARARGS_METHOD_DECL(CyPy_Point3D, distance);
};


#endif //CYPHESIS_CYPY_BALL_H
