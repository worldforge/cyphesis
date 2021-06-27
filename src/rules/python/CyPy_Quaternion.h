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

#ifndef CYPHESIS_CYPY_QUATERNION_H
#define CYPHESIS_CYPY_QUATERNION_H

#include <wfmath/quaternion.h>
#include "pycxx/CXX/Objects.hxx"
#include "pycxx/CXX/Extensions.hxx"
#include "pythonbase/WrapperBase.h"

/**
 * \ingroup PythonWrappers
 */
class CyPy_Quaternion : public WrapperBase<WFMath::Quaternion, CyPy_Quaternion>
{
    public:
        CyPy_Quaternion(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Quaternion(Py::PythonClassInstance* self, WFMath::Quaternion value);

        static void init_type();

    protected:


        Py::Object is_valid();
        PYCXX_NOARGS_METHOD_DECL(CyPy_Quaternion, is_valid);

        Py::Object as_list();
        PYCXX_NOARGS_METHOD_DECL(CyPy_Quaternion, as_list);

        Py::Object rotation(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_Quaternion, rotation);

        Py::Object getattro(const Py::String& name) override;

        Py::Object repr() override;

        Py::Object rich_compare(const Py::Object& other, int type) override;

        Py::Object number_multiply(const Py::Object& other) override;
};


#endif //CYPHESIS_CYPY_QUATERNION_H
