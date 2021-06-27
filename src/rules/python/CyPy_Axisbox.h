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

#ifndef CYPHESIS_CYPY_AXISBOX_H
#define CYPHESIS_CYPY_AXISBOX_H

#include <wfmath/axisbox.h>
#include "pycxx/CXX/Objects.hxx"
#include "pycxx/CXX/Extensions.hxx"
#include "pythonbase/WrapperBase.h"

/**
 * \ingroup PythonWrappers
 */
class CyPy_Axisbox : public WrapperBase<WFMath::AxisBox<3>, CyPy_Axisbox>
{
    public:
        CyPy_Axisbox(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Axisbox(Py::PythonClassInstance* self, WFMath::AxisBox<3> value);

        static void init_type();

    protected:


        Py::Object sqr_bounding_radius();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Axisbox, sqr_bounding_radius);

        Py::Object sqr_horizontal_bounding_radius();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Axisbox, sqr_horizontal_bounding_radius);

        Py::Object as_sequence();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Axisbox, as_sequence);

        Py::Object getattro(const Py::String& name) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object rich_compare(const Py::Object& other, int type) override;
};


#endif //CYPHESIS_CYPY_AXISBOX_H
