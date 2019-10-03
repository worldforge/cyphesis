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

#ifndef CYPHESIS_CYPY_WORLDTIME_H
#define CYPHESIS_CYPY_WORLDTIME_H

#include "modules/Ref.h"
#include "modules/WorldTime.h"
#include "rules/python/WrapperBase.h"

/**
 * \ingroup PythonWrappers
 */
class CyPy_WorldTime : public WrapperBase<std::shared_ptr<WorldTime>, CyPy_WorldTime>
{
    public:
        CyPy_WorldTime(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_WorldTime(Py::PythonClassInstance* self, std::shared_ptr<WorldTime> value);

        ~CyPy_WorldTime() override;

        static void init_type();

        Py::Object getattro(const Py::String&) override;

        const WorldTime& get_value();

        Py::Object seconds();

        PYCXX_NOARGS_METHOD_DECL(CyPy_WorldTime, seconds);

        Py::Object is_now(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_WorldTime, is_now);

};


#endif //CYPHESIS_CYPY_WORLDTIME_H
