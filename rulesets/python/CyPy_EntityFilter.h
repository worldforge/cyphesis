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

#ifndef CYPHESIS_CYPY_ENTITYFILTER_H
#define CYPHESIS_CYPY_ENTITYFILTER_H

#include <rulesets/entityfilter/Filter.h>
#include "external/pycxx/CXX/Objects.hxx"
#include "external/pycxx/CXX/Extensions.hxx"
#include <memory>


class CyPy_Filter : public Py::PythonClass<CyPy_Filter>
{
    public:

        typedef EntityFilter::Filter value_type;
        CyPy_Filter(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        static void init_type();
        static EntityFilter::Filter& value(const Py::Object& object);

        std::unique_ptr<EntityFilter::Filter> m_value;

        ///\brief Match a single entity using a filter that called this method.
        Py::Boolean match_entity(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Filter, match_entity);

        ///\This method is used to search the "contains" property of an entity,
        ///returning a list of entities that match a given filter
        ///
        ///This is useful when the client code knows that the query always requires
        ///to search within a contains property (i.e. inventory of a given entity)
        ///
        ///@param py_entity - an entity whose "contains" property to search
        Py::List search_contains(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_Filter, search_contains);


    private:

};

class CyPy_EntityFilter : public Py::ExtensionModule<CyPy_EntityFilter>
{
    public:
        CyPy_EntityFilter();

    private:
        Py::Object get_filter(const Py::Tuple& args);


};


#endif //CYPHESIS_CYPY_ENTITYFILTER_H
