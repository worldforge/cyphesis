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

#include <common/compose.hpp>
#include <common/debug.h>
#include "CyPy_Element.h"
#include "CyPy_Operation.h"
#include "CyPy_Oplist.h"
#include "CyPy_Location.h"
#include "CyPy_RootEntity.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

/**
 * Used when iterating over a List element.
 */
struct CyPy_ListElementIterator : Py::PythonClass<CyPy_ListElementIterator>
{
    //The owning element. Reference count is incremented at construction and decremented at destruction.
    CyPy_Element* m_element;
    Atlas::Message::ListType::const_iterator iter;

    CyPy_ListElementIterator(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
        : PythonClass(self, args, kwds), m_element(nullptr)
    {
        throw Py::RuntimeError("Can not instantiate directly.");
    }


    CyPy_ListElementIterator(Py::PythonClassInstance* self, CyPy_Element* value)
        : PythonClass(self),
          m_element(value),
          iter(m_element->m_value.List().begin())
    {
        m_element->self().increment_reference_count();
    }

    ~CyPy_ListElementIterator() override
    {
        m_element->self().decrement_reference_count();
    }

    PyObject* iternext()
    {
        if (iter != m_element->m_value.List().end()) {
            auto wrapper = CyPy_Element::wrap(*iter);
            wrapper.increment_reference_count();
            *(iter)++;
            return wrapper.ptr();
        } else {
            return nullptr;
        }
    }

    static void init_type()
    {
        behaviors().name("Element list iterator");
        behaviors().doc("");
        behaviors().supportIter(Py::PythonType::support_iter_iternext);

        behaviors().readyType();


    }

    static Py::PythonClassObject<CyPy_ListElementIterator> wrap(CyPy_Element* value)
    {
        auto obj = extension_object_new(type_object(), nullptr, nullptr);
        reinterpret_cast<Py::PythonClassInstance*>(obj)->m_pycxx_object = new CyPy_ListElementIterator(reinterpret_cast<Py::PythonClassInstance*>(obj), value);
        return Py::PythonClassObject<CyPy_ListElementIterator>(obj);
    }


};

CyPy_Element::CyPy_Element(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds) :
    WrapperBase(self, args, kwds)
{
    if (args.size() > 1) {
        throw Py::TypeError("Must take max one argument.");
    }
    if (args.size()) {
        m_value = asElement(args.front());
    }
}

CyPy_Element::CyPy_Element(Py::PythonClassInstance* self, Atlas::Message::Element element)
    : WrapperBase(self, std::move(element))
{
}

void CyPy_Element::init_type()
{
    behaviors().name("Element");
    behaviors().doc("");

    behaviors().supportRepr();
    behaviors().supportRichCompare();

    behaviors().supportMappingType(Py::PythonType::support_mapping_ass_subscript
                                   | Py::PythonType::support_mapping_subscript);
    behaviors().supportIter(Py::PythonType::support_iter_iter);

    PYCXX_ADD_NOARGS_METHOD(get_name, get_name, "");
    PYCXX_ADD_NOARGS_METHOD(pythonize, pythonize, "");

    behaviors().readyType();

    CyPy_ListElementIterator::init_type();
}

Py::Object CyPy_Element::get_name()
{
    return Py::String("obj");
}

Py::Object CyPy_Element::pythonize()
{
    return asPyObject(m_value);
}

Py::Object CyPy_Element::mapAsPyObject(const MapType& map)
{
    Py::Dict dict;
    for (auto& entry : map) {
        dict.setAttr(entry.first, CyPy_Element::wrap(entry.second));
    }
    return dict;
}

Py::Object CyPy_Element::listAsPyObject(const ListType& list)
{
    Py::List pyList;
    for (auto& entry : list) {
        pyList.append(CyPy_Element::wrap(entry));
    }
    return pyList;
}

Py::Object CyPy_Element::asPyObject(const Atlas::Message::Element& obj)
{
    switch (obj.getType()) {
        case Element::TYPE_INT:
            return Py::Long(obj.Int());
        case Element::TYPE_FLOAT:
            return Py::Float(obj.Float());
        case Element::TYPE_STRING:
            return Py::String(obj.String());
        case Element::TYPE_MAP:
            return mapAsPyObject(obj.Map());
        case Element::TYPE_LIST:
            return listAsPyObject(obj.List());
        default:
            break;
    }
    return Py::None();
}

Py::Object CyPy_Element::repr()
{
    if (m_value.isString()) {
        return Py::String(m_value.String());
    }
    return Py::String(String::compose("<%1 object at %2>(%3)", type_object()->tp_name, this, debug_tostring(m_value)));
}

Py::Object CyPy_Element::getattro(const Py::String& name)
{
    if (m_value.isMap()) {
        auto I = m_value.Map().find(name);
        if (I != m_value.Map().end()) {
            return asPyObject(I->second);
        }
    }
    return PythonExtensionBase::getattro(name);
}

int CyPy_Element::setattro(const Py::String& name, const Py::Object& attr)
{
    if (m_value.isMap()) {
        m_value.Map().emplace(name.as_string(), asElement(attr));
    } else {
        throw Py::AttributeError("Cannot set attribute on non-map in MessageElement.setattr");
    }
    return 0;
}

Py::Object CyPy_Element::rich_compare(const Py::Object& other, int op)
{
    if (m_value.isFloat()) {
        if (op == Py_GT && other.isNumeric()) {
            return Py::Boolean(m_value.Float() > Py::Float(other));
        } else if (op == Py_GE && other.isNumeric()) {
            return Py::Boolean(m_value.Float() >= Py::Float(other));
        } else if (op == Py_LT && other.isNumeric()) {
            return Py::Boolean(m_value.Float() < Py::Float(other));
        } else if (op == Py_LE && other.isNumeric()) {
            return Py::Boolean(m_value.Float() <= Py::Float(other));
        }
    } else if (m_value.isInt()) {
        if (op == Py_GT && other.isNumeric()) {
            return Py::Boolean(m_value.Int() > Py::Long(other));
        } else if (op == Py_GE && other.isNumeric()) {
            return Py::Boolean(m_value.Int() >= Py::Long(other));
        } else if (op == Py_LT && other.isNumeric()) {
            return Py::Boolean(m_value.Int() < Py::Long(other));
        } else if (op == Py_LE && other.isNumeric()) {
            return Py::Boolean(m_value.Int() <= Py::Long(other));
        }
    }


    bool equal = false;
    if ((op != Py_EQ) && (op != Py_NE)) {
        throw Py::NotImplementedError("MessageElement object can only be check for == or !=, or <, <=, > and >= for ints and floats.");
    }
    if (m_value.isString()) {
        if (other.isString() &&
            m_value.asString() == Py::String(other).as_string()) {
            equal = true;
        }
    } else if (m_value.isInt()) {
        if (other.isLong() &&
            m_value.asInt() == Py::Long(other).as_long()) {
            equal = true;
        }
    } else if (m_value.isFloat()) {
        if (other.isFloat()
            && m_value.asFloat() == Py::Float(other).as_double()) {
            equal = true;
        }
    }

    if ((equal && op == Py_EQ) || (!equal && op == Py_NE)) {
        return Py::True();
    }
    return Py::False();
}

ListType CyPy_Element::listAsElement(const Py::List& list)
{
    ListType res;
    for (auto& entry : list) {
        res.push_back(asElement(entry));
    }
    return res;
}

MapType CyPy_Element::dictAsElement(const Py::Dict& dict)
{
    MapType map;
    for (auto key : dict.keys()) {
        map.emplace(key.str(), asElement(dict.getItem(key)));
    }
    return map;
}

Element CyPy_Element::asElement(const Py::Object& o)
{
    if (o.isLong()) {
        return Py::Long(o).as_long();
    }
    if (o.isFloat()) {
        return Py::Float(o).as_double();
    }
    if (o.isString()) {
        return o.as_string();
    }
    if (o.isList()) {
        return listAsElement(Py::List(o));
    }
    if (o.isDict()) {
        return dictAsElement(Py::Dict(o));
    }
    if (o.isSequence()) {
        Py::Sequence seq(o);
        ListType list;
        for (const auto& entry : seq) {
            list.push_back(asElement(entry));
        }
        return list;
    }
    if (CyPy_Element::check(o)) {
        return CyPy_Element::value(o);
    }
    if (CyPy_Operation::check(o)) {
        return CyPy_Operation::value(o)->asMessage();
    }
    if (CyPy_RootEntity::check(o)) {
        return CyPy_RootEntity::value(o)->asMessage();
    }
    if (CyPy_Oplist::check(o)) {
        Py::PythonClassObject<CyPy_Oplist> listObj(o);
        ListType list;

        for (auto& entry : listObj.getCxxObject()->m_value) {
            list.push_back(entry->asMessage());
        }
        return list;
    }
    if (CyPy_Location::check(o)) {
        MapType map;
        CyPy_Location::value(o).addToMessage(map);
        return map;
    }
    throw Py::TypeError(String::compose("Contained object (of type %1) could not be converted to an Element.", o.type().as_string()));
}

Py::Object CyPy_Element::wrap(Atlas::Message::Element value)
{
    if (value.isNone()) {
        return Py::None();
    } else if (value.isString()) {
        return Py::String(value.String());
    } else if (value.isInt()) {
        return Py::Long(value.Int());
    } else if (value.isFloat()) {
        return Py::Float(value.Float());
    } else {
        return WrapperBase::wrap(std::move(value));
    }
}

Py::Object CyPy_Element::mapping_subscript(const Py::Object& key)
{
    if (!m_value.isMap()) {
        throw Py::RuntimeError("Element is not of Map type.");
    }
    auto I = m_value.Map().find(verifyString(key));
    if (I != m_value.Map().end()) {
        return CyPy_Element::asPyObject(I->second);
    }
    return Py::None();
}

int CyPy_Element::mapping_ass_subscript(const Py::Object& key, const Py::Object& value)
{
    if (!m_value.isMap()) {
        throw Py::RuntimeError("Element is not of Map type.");
    }

    m_value.Map()[verifyString(key)] = CyPy_Element::asElement(value);
    return 0;

}

Py::Object CyPy_Element::iter()
{
    if (!m_value.isList()) {
        return Py::None();
    }
    return CyPy_ListElementIterator::wrap(this);
}

