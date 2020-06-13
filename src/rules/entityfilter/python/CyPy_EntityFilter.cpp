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

#include "CyPy_EntityFilter.h"
#include "rules/entityfilter/ProviderFactory.h"
#include "pycxx/CXX/Objects.hxx"
#include "rules/python/CyPy_LocatedEntity.h"


CyPy_Filter::CyPy_Filter(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
        : WrapperBase(self, args, kwds)
{
    args.verify_length(1);
    auto filterString = verifyString(args.front());
    EntityFilter::ProviderFactory factory;
    try {
        m_value.reset(new EntityFilter::Filter(filterString, factory));
    } catch (const std::exception& e) {
        throw Py::TypeError(String::compose("Error when parsing query: %1", e.what()));
    }
}

void CyPy_Filter::init_type()
{
    behaviors().name("Filter");
    behaviors().doc("");

    behaviors().supportStr();

    behaviors().readyType();
}

CyPy_Filter::CyPy_Filter(Py::PythonClassInstance* self, std::shared_ptr<EntityFilter::Filter> value)
        : WrapperBase(self, std::move(value))
{

}

Py::Object CyPy_Filter::str()
{
    return Py::String(String::compose("Entity filter: %1", this->m_value->getDeclaration()));
}

CyPy_EntityFilter::CyPy_EntityFilter()
        : ExtensionModule("entity_filter")
{

    CyPy_Filter::init_type();


    initialize("Entity filtering");

    Py::Dict d(moduleDictionary());
    d["Filter"] = CyPy_Filter::type();
}

std::string CyPy_EntityFilter::init()
{
    PyImport_AppendInittab("entity_filter", []() {
        static auto module = new CyPy_EntityFilter();
        return module->module().ptr();
    });
    return "entity_filter";
}
