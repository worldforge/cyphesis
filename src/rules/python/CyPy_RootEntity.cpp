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
        auto front = args.front();
        if (front.isString()) {
            m_value->setId(verifyString(front));
        } else {
            setFromDict(verifyDict(front));
        }
    } else if (args.size() == 2) {
        m_value->setId(verifyString(args.front()));
        setFromDict(verifyDict(args[1]));
    } else if (args.size() > 2) {
        throw Py::TypeError("Can only supply two arguments to constructor.");
    }
    if (kwds.size() > 0) {
        setFromDict(kwds);
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

    behaviors().supportMappingType(Py::PythonType::support_mapping_ass_subscript
                                   | Py::PythonType::support_mapping_subscript);
    behaviors().supportSequenceType(Py::PythonType::support_sequence_contains);

    PYCXX_ADD_NOARGS_METHOD(get_name, get_name, "");

    behaviors().readyType();
}

void CyPy_RootEntity::setFromDict(const Py::Dict& dict)
{
    for (auto key : dict.keys()) {
        auto keyStr = key.str().as_string();
        auto value = dict.getItem(key);
        if (keyStr == "location") {
            if (!CyPy_Location::check(value)) {
                throw Py::TypeError("location must be a Location object");
            }
            CyPy_Location::value(value).addToEntity(m_value);
        } else if (keyStr == "pos") {
            m_value->setPos(sequence_asVector(value));
        } else if (keyStr == "parent") {
            m_value->setParent(verifyString(value));
        } else if (keyStr == "objtype") {
            m_value->setObjtype(verifyString(value));
        } else {
            m_value->setAttr(key.str(), CyPy_Element::asElement(value));
        }
    }
}


std::vector<double> CyPy_RootEntity::sequence_asVector(const Py::Object& o)
{
    std::vector<double> vector;
    if (o.isList() || o.isTuple() || o.isSequence()) {
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
    return Py::String(m_value->getName());
}

Py::Object CyPy_RootEntity::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "name") {
        return Py::String(m_value->getName());
    } else if (nameStr == "id") {
        return Py::String(m_value->getId());
    } else if (nameStr == "from_") {
        // "from" is protected in Python, so we'll use the "from_" version.
        Element attr;
        if (m_value->copyAttr("from", attr) == 0) {
            if (attr.isPtr()) {
                return Py::Object((PyObject*) attr.Ptr());
            }
            return CyPy_Element::asPyObject(attr, false);
        }
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

Py::Object CyPy_RootEntity::mapping_subscript(const Py::Object& key)
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

int CyPy_RootEntity::mapping_ass_subscript(const Py::Object& key, const Py::Object& value)
{
    m_value->setAttr(verifyString(key), CyPy_Element::asElement(value));
    return 0;
}

int CyPy_RootEntity::sequence_contains(const Py::Object& key)
{
    auto keyStr = verifyString(key);
    if (m_value->hasAttr(keyStr)) {
        return 1;
    }
    return 0;
}
