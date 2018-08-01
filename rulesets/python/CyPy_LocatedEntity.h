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

#ifndef CYPHESIS_CYPY_LOCATEDENTITY_H
#define CYPHESIS_CYPY_LOCATEDENTITY_H


#include "rulesets/LocatedEntity.h"
#include "WrapperBase.h"

#include <common/id.h>
#include "rulesets/Character.h"
#include "rulesets/MemEntity.h"
#include "CyPy_Props.h"
#include "common/TypeNode.h"
#include "common/Inheritance.h"
#include "CyPy_Location.h"
#include "CyPy_Element.h"

template<typename TValue, typename TPythonClass>
class CyPy_LocatedEntityBase : public WrapperBase<TValue, TPythonClass>
{
    public:
        CyPy_LocatedEntityBase(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        ~CyPy_LocatedEntityBase() override;

        Py::Object getattro(const Py::String&) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object rich_compare(const Py::Object&, int) override;

        static Py::Object wrap(LocatedEntity* value);


    protected:
        CyPy_LocatedEntityBase(Py::PythonClassInstance* self, TValue value);

        Py::Object as_entity();

        PYCXX_NOARGS_METHOD_DECL(CyPy_LocatedEntityBase, as_entity);

        Py::Object can_reach(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, can_reach);

        Py::Object describe_entity();

        PYCXX_NOARGS_METHOD_DECL(CyPy_LocatedEntityBase, describe_entity);

        Py::Object client_error(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, client_error);
};

class CyPy_LocatedEntity
{
    public:

        typedef LocatedEntity value_type;
        struct type_def
        {
            std::string tp_name = "LocatedEntity";
        };

        static type_def* type_object()
        {
            static type_def type;
            return &type;
        }

        static bool check(PyObject* object);

        static bool check(const Py::Object& object);

        static Py::Object wrap(LocatedEntity* value);

        static LocatedEntity& value(const Py::Object& object);


};

Py::Object wrapLocatedEntity(LocatedEntity* locatedEntity);



#endif //CYPHESIS_CYPY_LOCATEDENTITY_H
