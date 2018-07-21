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

#include <rulesets/entityfilter/Providers.h>
#include "external/pycxx/CXX/Objects.hxx"
#include "CyPy_EntityFilter.h"
#include "CyPy_LocatedEntity.h"


CyPy_Filter::CyPy_Filter(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : Py::PythonClass<CyPy_Filter>::PythonClass(self, args, kwds)
{
    args.verify_length(1);
    auto arg = args.front();
    if (arg.isString()) {
        EntityFilter::MindProviderFactory factory;
        m_value.reset(new EntityFilter::Filter(Py::String(arg).as_string(), &factory));
    }
}

void CyPy_Filter::init_type()
{
    behaviors().name("Filter");
    behaviors().doc("");

    PYCXX_ADD_VARARGS_METHOD(match_entity, match_entity, "");
    PYCXX_ADD_VARARGS_METHOD(search_contains, search_contains, "");
    behaviors().readyType();
}

Py::Boolean CyPy_Filter::match_entity(const Py::Tuple& args)
{
    args.verify_length(1);
    auto arg = args.front();
    if (CyPy_LocatedEntity::check(arg)) {
        return m_value->match(CyPy_LocatedEntity::value(arg));
    }
    throw Py::TypeError("First arg must be LocatedEntity.");
}

Py::List CyPy_Filter::search_contains(const Py::Tuple& args)
{
    Py::List list;

    args.verify_length(1);
    auto arg = args.front();
    if (CyPy_LocatedEntity::check(arg)) {
        auto& entity = CyPy_LocatedEntity::value(arg);
        if (entity.m_contains) {
            for (auto& contained : *entity.m_contains) {
                list.append(wrapLocatedEntity(contained));
            }
        }
    }
    return list;
}

CyPy_EntityFilter::CyPy_EntityFilter()
    : ExtensionModule("entity_filter")
{

    CyPy_Filter::init_type();

    add_varargs_method("get_filter", &CyPy_EntityFilter::get_filter, "");


    initialize("Entity filtering");

}

///\brief Create a new Filter object for a given query
Py::Object CyPy_EntityFilter::get_filter(const Py::Tuple& args)
{

    args.verify_length(1);
    try {
        auto arg = args.front();
        if (arg.isString()) {
            Py::Callable class_type(CyPy_Filter::type());
            Py::PythonClassObject<CyPy_Filter> filter_obj(class_type.apply(args));

            return filter_obj;
        }
    }
    catch (std::invalid_argument& e) {
        throw Py::TypeError(String::compose("Invalid query for Entity Filter: %1", e.what()).c_str());
    }
    return Py::None();
}
