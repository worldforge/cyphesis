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

#ifndef CYPHESIS_CYPY_ELEMENT_H
#define CYPHESIS_CYPY_ELEMENT_H

#include "external/pycxx/CXX/Objects.hxx"
#include "external/pycxx/CXX/Extensions.hxx"
#include "WrapperBase.h"

#include <Atlas/Message/Element.h>

class CyPy_Element : public WrapperBase<Atlas::Message::Element, CyPy_Element>
{
    public:
        CyPy_Element(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_Element(Py::PythonClassInstance* self, Atlas::Message::Element&& element);

        static void init_type();

        Py::Object repr() override;

        Py::Object getattro(const Py::String&) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object get_name();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Element, get_name)

        Py::Object pythonize();

        PYCXX_NOARGS_METHOD_DECL(CyPy_Element, pythonize)

        static Py::Object asPyObject(const Atlas::Message::Element& obj);

        static Atlas::Message::Element asElement(const Py::Object& o);

    protected:


        static Py::Object mapAsPyObject(const Atlas::Message::MapType& map);

        static Py::Object listAsPyObject(const Atlas::Message::ListType& list);

        static Atlas::Message::ListType listAsElement(const Py::List& list);

        static Atlas::Message::MapType dictAsElement(const Py::Dict& dict);

        Py::Object rich_compare(const Py::Object& other, int op) override;
};


#endif //CYPHESIS_CYPY_ELEMENT_H
