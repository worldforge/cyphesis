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

#include <modules/WorldTime.h>
#include "CyPy_World.h"
#include "rules/python/CyPy_LocatedEntity.h"
#include "rules/entityfilter/python/CyPy_EntityFilter.h"
#include "rules/entityfilter/Providers.h"
#include "common/Inheritance.h"

CyPy_World::CyPy_World(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    throw Py::RuntimeError("Can not create World instances.");
}

CyPy_World::CyPy_World(Py::PythonClassInstance* self, BaseWorld* value)
    : WrapperBase(self, value)
{

}

void CyPy_World::init_type()
{
    behaviors().name("World");
    behaviors().doc("");

    PYCXX_ADD_VARARGS_METHOD(get_entity, get_entity, "Gets the entity with the supplied id.");
    PYCXX_ADD_VARARGS_METHOD(match_entity, match_entity, "Matches a filter against an entity.");

    PYCXX_ADD_NOARGS_METHOD(get_time, get_time, "");

    behaviors().readyType();
}

Py::Object CyPy_World::get_time()
{
    return Py::Float(m_value->getTimeAsSeconds());
}

Py::Object CyPy_World::get_entity(const Py::Tuple& args)
{
    args.verify_length(1);
    auto id = verifyString(args.front());
    auto ent = m_value->getEntity(id);
    if (!ent) {
        return Py::None();
    }
    return CyPy_LocatedEntity::wrap(std::move(ent));
}

Py::Object CyPy_World::match_entity(const Py::Tuple& args)
{
    args.verify_length(2);
    auto& filter = verifyObject<CyPy_Filter>(args.front());
    auto entity = verifyObject<CyPy_LocatedEntity>(args[1]);


    EntityFilter::QueryContext queryContext{*entity};
//    queryContext.report_error_fn = [&](const std::string& error) { errors.push_back(error); };
    queryContext.entity_lookup_fn = [&](const std::string& id) { return m_value->getEntity(id); };
    queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };

    return Py::Boolean(filter->match(queryContext));
}

