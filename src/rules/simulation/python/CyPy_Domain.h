/*
 Copyright (C) 2019 Erik Ogenvik

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

#ifndef CYPHESIS_CYPY_DOMAIN_H
#define CYPHESIS_CYPY_DOMAIN_H

#include "rules/Domain.h"
#include "modules/Ref.h"
#include "pythonbase/WrapperBase.h"

/**
 * \ingroup PythonWrappers
 */
class CyPy_Domain : public WrapperBase<Ref<LocatedEntity>, CyPy_Domain>
{
    public:
        CyPy_Domain(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Domain(Py::PythonClassInstance* self, Ref<LocatedEntity> value);

        ~CyPy_Domain() override;

        static void init_type();

        Py::Object query_collisions(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Domain, query_collisions);

};

struct CyPy_CollisionEntry : public WrapperBase<Domain::CollisionEntry, CyPy_CollisionEntry>
{
    CyPy_CollisionEntry(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

    CyPy_CollisionEntry(Py::PythonClassInstance* self, Domain::CollisionEntry value);

    ~CyPy_CollisionEntry() override;

    static void init_type();

    Py::Object getattro(const Py::String& name) override;

};


#endif //CYPHESIS_CYPY_DOMAIN_H
