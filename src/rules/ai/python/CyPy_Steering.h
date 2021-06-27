/*
 Copyright (C) 2020 Erik Ogenvik

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

#ifndef CYPHESIS_CYPY_STEERING_H
#define CYPHESIS_CYPY_STEERING_H

#include "rules/ai/AwareMind.h"
#include "pythonbase/WrapperBase.h"

/**
 * Wrapper for Steering. The object is attached to a Ref to an AwareMind.
 *
 * \ingroup PythonWrappers
 */
class CyPy_Steering : public WrapperBase<Ref<AwareMind>, CyPy_Steering>
{
    public:

        CyPy_Steering(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Steering(Py::PythonClassInstance* self, Ref<AwareMind> value);

        static void init_type();

        Py::Object getattro(const Py::String& name) override;

    private:

        Py::Object refreshPath();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Steering, refreshPath);

        Py::Object setDestination(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Steering, setDestination);

        Py::Object setSpeed(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Steering, setSpeed);

        Py::Object queryDestination(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Steering, queryDestination);

//        Py::Object isAtLocation(const Py::Tuple& args);
//
//        PYCXX_VARARGS_METHOD_DECL(CyPy_Steering, isAtLocation);

        Py::Object distanceTo(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Steering, distanceTo);

        Py::Object direction_to(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Steering, direction_to);

        Py::Object isAtCurrentDestination();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Steering, isAtCurrentDestination);


};

#endif //CYPHESIS_CYPY_STEERING_H
