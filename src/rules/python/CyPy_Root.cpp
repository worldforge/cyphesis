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

#include "CyPy_Root.h"
#include "CyPy_Element.h"
#include <Atlas/Objects/Root.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;

CyPy_Root::CyPy_Root(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    m_value = Root();
    if (args.size() == 1) {
        m_value->setId(verifyString(args.front()));
    }
    if (kwds.size() > 0) {
        for (auto key : kwds.keys()) {
            auto keyStr = key.str().as_string();
            auto value = kwds.getItem(key);
            m_value->setAttr(key.str(), CyPy_Element::asElement(value));
        }
    }
}

CyPy_Root::CyPy_Root(Py::PythonClassInstance* self, Atlas::Objects::Root value)
    : WrapperBase(self, std::move(value))
{

}

void CyPy_Root::init_type()
{
    behaviors().name("Root");
    behaviors().doc("");

    behaviors().supportMappingType(Py::PythonType::support_mapping_ass_subscript
                                   | Py::PythonType::support_mapping_subscript);
    behaviors().supportSequenceType(Py::PythonType::support_sequence_contains);

    PYCXX_ADD_NOARGS_METHOD(get_name, get_name, "");

    behaviors().readyType();
}

Py::Object CyPy_Root::get_name()
{
    return Py::String(m_value->getName());
}


Py::Object CyPy_Root::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "name") {
        return Py::String(m_value->getName());
    } else if (nameStr == "id") {
        return Py::String(m_value->getId());
    } else {
        Element attr;
        if (m_value->copyAttr(name, attr) == 0) {
            if (attr.isPtr()) {
                return Py::Object((PyObject*) attr.Ptr());
            }
            return CyPy_Element::asPyObject(attr, false);
        }
    }

    return PythonExtensionBase::getattro(name);
}

int CyPy_Root::setattro(const Py::String& name, const Py::Object& attr)
{
    auto nameStr = name.as_string();

    if (nameStr == "name") {
        m_value->setName(verifyString(attr));
    }

    m_value->setAttr(nameStr, CyPy_Element::asElement(attr));
    return 0;
}

Py::Object CyPy_Root::mapping_subscript(const Py::Object& key)
{
    Element attr;
    if (m_value->copyAttr(verifyString(key), attr) == 0) {
        if (attr.isPtr()) {
            return Py::Object((PyObject*) attr.Ptr());
        }
        return CyPy_Element::asPyObject(attr, false);
    }
    return Py::None();
}

int CyPy_Root::mapping_ass_subscript(const Py::Object& key, const Py::Object& value)
{
    m_value->setAttr(verifyString(key), CyPy_Element::asElement(value));
    return 0;
}

int CyPy_Root::sequence_contains(const Py::Object& key)
{
    auto keyStr = verifyString(key);
    if (m_value->hasAttr(keyStr)) {
        return 1;
    }
    return 0;
}
