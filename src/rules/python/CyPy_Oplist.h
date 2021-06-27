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

#ifndef CYPHESIS_CYPY_OPLIST_H
#define CYPHESIS_CYPY_OPLIST_H

#include "pycxx/CXX/Objects.hxx"
#include "pycxx/CXX/Extensions.hxx"
#include "pythonbase/WrapperBase.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootOperation.h>

/**
 * \ingroup PythonWrappers
 */
class CyPy_Oplist : public WrapperBase<std::vector<Atlas::Objects::Operation::RootOperation>, CyPy_Oplist>
{
    public:
        CyPy_Oplist(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Oplist(Py::PythonClassInstance* self, std::vector<Atlas::Objects::Operation::RootOperation> value);

        static void init_type();

    protected:

        Py::Object append(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Oplist, append);


        static void appendToList(const Py::Object& op, std::vector<Atlas::Objects::Operation::RootOperation>& list);

        Py::Object number_add(const Py::Object& other) override;

        Py::Object sequence_inplace_concat(const Py::Object& other) override;

        PyCxx_ssize_t sequence_length() override;

};


#endif //CYPHESIS_CYPY_OPLIST_H
