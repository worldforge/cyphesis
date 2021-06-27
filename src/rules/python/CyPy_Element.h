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

#include "pycxx/CXX/Objects.hxx"
#include "pycxx/CXX/Extensions.hxx"
#include "pythonbase/WrapperBase.h"

#include <Atlas/Message/Element.h>

/**
 * \ingroup PythonWrappers
 */
class CyPy_ElementList : public WrapperBase<Atlas::Message::ListType, CyPy_ElementList> {
    public:
        CyPy_ElementList(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_ElementList(Py::PythonClassInstance* self, Atlas::Message::ListType element);

        static void init_type();

        Py::Object repr() override;

        Py::Object rich_compare(const Py::Object& other, int op) override;

        PyCxx_ssize_t sequence_length() override;

        Py::Object sequence_concat(const Py::Object&) override;

        Py::Object sequence_repeat(Py_ssize_t) override;

        Py::Object sequence_item(Py_ssize_t) override;

        int sequence_ass_item(Py_ssize_t, const Py::Object&) override;

        Py::Object sequence_inplace_concat(const Py::Object&) override;

        Py::Object sequence_inplace_repeat(Py_ssize_t) override;

        int sequence_contains(const Py::Object&) override;

        Py::Object iter() override;
};

/**
 * \ingroup PythonWrappers
 */
class CyPy_ElementMap : public WrapperBase<Atlas::Message::MapType, CyPy_ElementMap> {
    public:
        CyPy_ElementMap(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_ElementMap(Py::PythonClassInstance* self, Atlas::Message::MapType element);

        static void init_type();

        Py::Object repr() override;

        Py::Object rich_compare(const Py::Object& other, int op) override;

        Py::Object mapping_subscript(const Py::Object&) override;

        int sequence_contains(const Py::Object&) override;

        int mapping_ass_subscript(const Py::Object&, const Py::Object&) override;

        Py::Object getattro(const Py::String&) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object items();

        PYCXX_NOARGS_METHOD_DECL(CyPy_ElementMap, items);

};

/**
 * \ingroup PythonWrappers
 */
class CyPy_Element
{
    public:

        static Py::Object wrap(Atlas::Message::Element value);


        static Py::Object asPyObject(const Atlas::Message::Element& obj, bool useNativePythonType);

        static Atlas::Message::Element asElement(const Py::Object& o);

        static Atlas::Message::ListType listAsElement(const Py::List& list);

        static Atlas::Message::ListType sequenceAsElement(const Py::Sequence& list);

        static Atlas::Message::MapType dictAsElement(const Py::Dict& dict);

    protected:


        static Py::Object mapAsPyObject(const Atlas::Message::MapType& map, bool useNativePythonType);

        static Py::Object listAsPyObject(const Atlas::Message::ListType& list, bool useNativePythonType);


};


#endif //CYPHESIS_CYPY_ELEMENT_H
