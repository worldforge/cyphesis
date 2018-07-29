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

#ifndef CYPHESIS_CYPY_ENTITY_H
#define CYPHESIS_CYPY_ENTITY_H

#include "rulesets/Entity.h"
#include "CyPy_LocatedEntity_impl.h"

class CyPy_Entity : public CyPy_LocatedEntityBase<Ref<Entity>, CyPy_Entity>
{
    public:
        CyPy_Entity(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);
        CyPy_Entity(Py::PythonClassInstance* self, Ref<Entity> value);

        ~CyPy_Entity() override;

        static void init_type();

        static bool check(PyObject* object);

        static bool check(const Py::Object& object);

        static Ref<Entity> value(const Py::Object& object);

    protected:

        Py::Object send_world(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_Entity, send_world);

};

#endif //CYPHESIS_CYPY_ENTITY_H
