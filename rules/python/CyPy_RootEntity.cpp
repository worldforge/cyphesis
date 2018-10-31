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

#include <Atlas/Objects/Anonymous.h>
#include "CyPy_RootEntity.h"
#include "CyPy_Element.h"
#include "CyPy_Location.h"

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

CyPy_RootEntity::CyPy_RootEntity(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    m_value = Anonymous();
    if (args.size() == 1) {
        m_value->setId(verifyString(args.front()));
    }
    if (kwds.size() > 0) {
        for (auto key : kwds.keys()) {
            auto keyStr = key.str().as_string();
            auto value = kwds.getItem(key);
            if (keyStr == "location") {
                if (!CyPy_Location::check(value)) {
                    throw Py::TypeError("location must be a Location object");
                }
                CyPy_Location::value(value).addToEntity(m_value);
            } else if (keyStr == "pos") {
                m_value->setPos(sequence_asVector(value));
            } else if (keyStr == "parent") {
                m_value->setParent(verifyString(value));
            } else if (keyStr == "type") {
                m_value->setObjtype(verifyString(value));
            } else {
                m_value->setAttr(key.str(), CyPy_Element::asElement(value));
            }
        }
    }
}

CyPy_RootEntity::CyPy_RootEntity(Py::PythonClassInstance* self, Atlas::Objects::Entity::RootEntity value)
    : WrapperBase(self, std::move(value))
{

}

void CyPy_RootEntity::init_type()
{
    behaviors().name("RootEntity");
    behaviors().doc("");

    PYCXX_ADD_NOARGS_METHOD(get_name, get_name, "");

    behaviors().readyType();
}

std::vector<double> CyPy_RootEntity::sequence_asVector(const Py::Object& o)
{
    std::vector<double> vector;
    if (o.isList() || o.isTuple()) {
        Py::Sequence list(o);
        for (auto entry : list) {
            if (!entry.isNumeric()) {
                throw Py::TypeError("Vector must have numeric values.");
            }
            vector.push_back(Py::Float(entry).as_double());
        }
    } else {
        throw Py::TypeError("Object is not a sequence.");
    }
    return vector;
}

Py::Object CyPy_RootEntity::get_name()
{
    return Py::String("obj");
}


Py::Object CyPy_RootEntity::getattro(const Py::String& name)
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

int CyPy_RootEntity::setattro(const Py::String& name, const Py::Object& attr)
{
    auto nameStr = name.as_string();

    if (nameStr == "name") {
        m_value->setName(verifyString(attr));
    }

    m_value->setAttr(nameStr, CyPy_Element::asElement(attr));
    return 0;
}
