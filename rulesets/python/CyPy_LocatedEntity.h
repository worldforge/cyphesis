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


#include "rulesets/LocatedEntity.h"
#include "external/pycxx/CXX/Objects.hxx"
#include "external/pycxx/CXX/Extensions.hxx"
#include <memory>


class CyPy_LocatedEntity : public Py::PythonClass<CyPy_LocatedEntity>
{
    public:

        CyPy_LocatedEntity(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        virtual ~CyPy_LocatedEntity();

        static void init_type();

        static LocatedEntity& value(const Py::Object& object);

        static Py::PythonClassObject<CyPy_LocatedEntity> wrap(LocatedEntity* entity);

        Py::Object getattro(const Py::String&) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object rich_compare(const Py::Object&, int) override;

        //TODO: should be a boost:variant
        LocatedEntity* m_value;

    protected:
        CyPy_LocatedEntity(Py::PythonClassInstance* self, LocatedEntity* value);

};

Py::PythonClassObject<CyPy_LocatedEntity> wrapLocatedEntity(LocatedEntity* locatedEntity);

#endif //CYPHESIS_CYPY_ENTITY_H
