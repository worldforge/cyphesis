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

#ifndef CYPHESIS_CYPY_TASK_H
#define CYPHESIS_CYPY_TASK_H


#include "rules/simulation/Task.h"
#include "modules/Ref.h"
#include "pythonbase/WrapperBase.h"

/**
 * \ingroup PythonWrappers
 */
class CyPy_Task : public WrapperBase<Ref<Task>, CyPy_Task>
{
    public:
        CyPy_Task(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Task(Py::PythonClassInstance* self, Ref<Task> value);

        ~CyPy_Task() override;

        static void init_type();

        Py::Object getattro(const Py::String& name) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object irrelevant(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Task, irrelevant);

        Py::Object start_action(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Task, start_action);

        Py::Object stop_action(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Task, stop_action);

        Py::Object obsolete();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Task, obsolete);

        Py::Object rich_compare(const Py::Object& other, int type) override;

        Py::Object getArg(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Task, getArg);

        /**
         * Allow storing random Python objects in the "temporaries" map.
         * Otherwise only Atlas::Message::Element types are allowed to be stored.
         * This is meant to be used for instance specific temporary data which can be recreated at instance creation time.
         */
        Py::Dict mTemporaries;

};


#endif //CYPHESIS_CYPY_TASK_H
