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

#ifndef CYPHESIS_CYPY_BASEMIND_H
#define CYPHESIS_CYPY_BASEMIND_H


#include "rules/ai/BaseMind.h"
#include "pythonbase/WrapperBase.h"

/**
 * \ingroup PythonWrappers
 */
class CyPy_BaseMind : public WrapperBase<Ref<BaseMind>, CyPy_BaseMind>
{
    public:
        CyPy_BaseMind(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);
        CyPy_BaseMind(Py::PythonClassInstance* self, Ref<BaseMind> value);

        ~CyPy_BaseMind() override;

        static void init_type();

        Py::Object getattro(const Py::String&) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object str() override;

    protected:

        Py::Object addPropertyCallback(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_BaseMind, addPropertyCallback);

        Py::Object matchEntity(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_BaseMind, matchEntity);

        Py::Object matchEntities(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_BaseMind, matchEntities);

        Py::Object add_hook_set(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_BaseMind, add_hook_set);

        Py::Object update_hook_set(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_BaseMind, update_hook_set);

        Py::Object delete_hook_set(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_BaseMind, delete_hook_set);
};



#endif //CYPHESIS_CYPY_BASEMIND_H
