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

#include "rules/simulation/Entity.h"
#include "rules/python/CyPy_LocatedEntity_impl.h"

/**
 * \ingroup PythonWrappers
 */
class CyPy_Entity : public CyPy_LocatedEntityBase<Ref<Entity>, CyPy_Entity>
{
    public:
        CyPy_Entity(Py::PythonClassInstanceWeak* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Entity(Py::PythonClassInstanceWeak* self, Ref<Entity> value);

        ~CyPy_Entity() override;

        static void init_type();

        static Py::Object send_world(const Ref<Entity>& entity, const Py::Tuple& args);

        static Py::Object start_task(const Ref<Entity>& entity, const Py::Tuple& args);

        static Py::Object update_task(const Ref<Entity>& entity);

        static Py::Object start_action(const Ref<Entity>& entity, const Py::Tuple& args);

        static Py::Object mod_property(const Ref<Entity>& entity, const Py::Tuple& args);

        Py::Object getattro(const Py::String&) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

    protected:

        Py::Object send_world(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Entity, send_world);

        Py::Object start_task(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Entity, start_task);

        Py::Object start_action(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Entity, start_action);

        Py::Object update_task();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Entity, update_task);

        Py::Object mod_property(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Entity, mod_property);

        Py::Object find_in_contains(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Entity, find_in_contains);

        Py::Object get_parent_domain();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Entity, get_parent_domain);

        Py::Object create_new_entity(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Entity, create_new_entity);
};

#endif //CYPHESIS_CYPY_ENTITY_H
