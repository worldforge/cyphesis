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

#ifndef CYPHESIS_CYPY_OPERATION_H
#define CYPHESIS_CYPY_OPERATION_H

#include "pycxx/CXX/Objects.hxx"
#include "pycxx/CXX/Extensions.hxx"
#include "pythonbase/WrapperBase.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

/**
 * \ingroup PythonWrappers
 */
class CyPy_Operation : public WrapperBase<Atlas::Objects::Operation::RootOperation, CyPy_Operation>
{
    public:
        CyPy_Operation(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Operation(Py::PythonClassInstance* self, Atlas::Objects::Operation::RootOperation value);

        static void init_type();

        PyCxx_ssize_t sequence_length() override;

        Py::Object sequence_item(Py_ssize_t) override;

        Py::Object number_add(const Py::Object&) override;

        Py::Object mapping_subscript(const Py::Object&) override;

        int sequence_contains(const Py::Object&) override;

        int mapping_ass_subscript(const Py::Object&, const Py::Object&) override;

        Py::Object getattro(const Py::String& name) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

    protected:

        Py::Object setSerialno(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Operation, setSerialno);

        Py::Object setRefno(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Operation, setRefno);

        Py::Object setFrom(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Operation, setFrom);

        Py::Object setTo(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Operation, setTo);

        Py::Object setSeconds(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Operation, setSeconds);

        Py::Object setFutureSeconds(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Operation, setFutureSeconds);

        Py::Object setName(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Operation, setName);

        Py::Object setArgs(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Operation, setArgs);

        Py::Object getSerialno();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Operation, getSerialno);

        Py::Object isDefaultSerialno();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Operation, isDefaultSerialno);

        Py::Object getRefno();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Operation, getRefno);

        Py::Object getFrom();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Operation, getFrom);

        Py::Object getTo();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Operation, getTo);

        Py::Object getSeconds();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Operation, getSeconds);

        Py::Object getFutureSeconds();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Operation, getFutureSeconds);

        Py::Object getName();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Operation, getName);

        Py::Object getArgs();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Operation, getArgs);

        Py::Object get_name();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Operation, get_name);

        Py::Object copy();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Operation, copy);

        void addToArgs(std::vector<Atlas::Objects::Root>& args, const Py::Object& arg);
};


#endif //CYPHESIS_CYPY_OPERATION_H
