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

#ifndef CYPHESIS_CYPY_ROOTENTITY_H
#define CYPHESIS_CYPY_ROOTENTITY_H

#include "pycxx/CXX/Objects.hxx"
#include "pycxx/CXX/Extensions.hxx"
#include "pythonbase/WrapperBase.h"

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/SmartPtr.h>

/**
 * \ingroup PythonWrappers
 */
class CyPy_RootEntity : public WrapperBase<Atlas::Objects::Entity::RootEntity, CyPy_RootEntity>
{
    public:
        CyPy_RootEntity(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_RootEntity(Py::PythonClassInstance* self, Atlas::Objects::Entity::RootEntity value);

        static void init_type();

        Py::Object mapping_subscript(const Py::Object&) override;

        int sequence_contains(const Py::Object&) override;

        int mapping_ass_subscript(const Py::Object&, const Py::Object&) override;

        Py::Object get_name();

        PYCXX_NOARGS_METHOD_DECL(CyPy_RootEntity, get_name)


    protected:

        void setFromDict(const Py::Dict& dict);


        Py::Object getattro(const Py::String& name) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        static std::vector<double> sequence_asVector(const Py::Object& o);

};


#endif //CYPHESIS_CYPY_ROOTENTITY_H
