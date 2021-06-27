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

#ifndef CYPHESIS_CYPY_USAGEINSTANCE_H
#define CYPHESIS_CYPY_USAGEINSTANCE_H

#include "pythonbase/WrapperBase.h"
#include "rules/simulation/UsageInstance.h"

/**
 * \ingroup PythonWrappers
 */
class CyPy_Usage : public WrapperBase<Usage, CyPy_Usage>
{
    public:

        CyPy_Usage(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Usage(Py::PythonClassInstance* self, Usage value);

        static void init_type();

        Py::Object getattro(const Py::String& name) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

};


class CyPy_UsageInstance : public WrapperBase<UsageInstance, CyPy_UsageInstance>
{
    public:

        CyPy_UsageInstance(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_UsageInstance(Py::PythonClassInstance* self, UsageInstance value);

        static void init_type();

        Py::Object getattro(const Py::String& name) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object isValid();
        PYCXX_NOARGS_METHOD_DECL(CyPy_UsageInstance, isValid);

        Py::Object getArg(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_UsageInstance, getArg);

        static Py::Object getArg(const UsageInstance& instance, const Py::Tuple& args);

};


#endif //CYPHESIS_CYPY_USAGEINSTANCE_H
