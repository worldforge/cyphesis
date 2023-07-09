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
#include "CyPy_Root.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;


/**
 * Used when iterating over a List element.
 */
struct CyPy_ListElementIterator : Py::PythonClass<CyPy_ListElementIterator>
{
    //The owning element. Reference count is incremented at construction and decremented at destruction.
    CyPy_ElementList* m_element;
    Atlas::Message::ListType::const_iterator iterator;

    CyPy_ListElementIterator(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
        : PythonClass(self, args, kwds), m_element(nullptr)
    {
        throw Py::RuntimeError("Can not instantiate directly.");
    }


    CyPy_ListElementIterator(Py::PythonClassInstance* self, CyPy_ElementList* value)
        : PythonClass(self),
          m_element(value),
          iterator(m_element->m_value.begin())
    {
        m_element->self().increment_reference_count();
    }

    ~CyPy_ListElementIterator() override
    {
        m_element->self().decrement_reference_count();
    }

    Py::Object iter() override
    {
        return self();
    }

    PyObject* iternext() override
    {
        if (iterator != m_element->m_value.end()) {
            auto wrapper = CyPy_Element::wrap(*iterator);
            wrapper.increment_reference_count();
            iterator++;
            return wrapper.ptr();
        } else {
            return nullptr;
        }
    }

    static void init_type()
    {
        behaviors().name("Element list iterator");
        behaviors().doc("");
        behaviors().supportIter(Py::PythonType::support_iter_iter|Py::PythonType::support_iter_iternext);

        behaviors().readyType();


    }

    static Py::PythonClassObject<CyPy_ListElementIterator> wrap(CyPy_ElementList* value)
    {
        auto obj = extension_object_new(type_object(), nullptr, nullptr);
        reinterpret_cast<Py::PythonClassInstance*>(obj)->m_pycxx_object = new CyPy_ListElementIterator(reinterpret_cast<Py::PythonClassInstance*>(obj), value);
        return Py::PythonClassObject<CyPy_ListElementIterator>(obj, true);
    }


};

struct CyPy_MapElementIterator : Py::PythonClass<CyPy_MapElementIterator>
{
    //The owning element. Reference count is incremented at construction and decremented at destruction.
    CyPy_ElementMap* m_element;
    Atlas::Message::MapType::const_iterator iterator;

    CyPy_MapElementIterator(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
        : PythonClass(self, args, kwds), m_element(nullptr)
    {
        throw Py::RuntimeError("Can not instantiate directly.");
    }


    CyPy_MapElementIterator(Py::PythonClassInstance* self, CyPy_ElementMap* value)
        : PythonClass(self),
          m_element(value),
          iterator(m_element->m_value.begin())
    {
        m_element->self().increment_reference_count();
    }

    ~CyPy_MapElementIterator() override
    {
        m_element->self().decrement_reference_count();
    }

    Py::Object iter() override
    {
        return self();
    }


    PyObject* iternext() override
    {
        if (iterator != m_element->m_value.end()) {
            auto wrapper = CyPy_Element::wrap(iterator->second);
            auto key = Py::String(iterator->first);

            Py::TupleN tuple(key, wrapper);
            tuple.increment_reference_count();

            *(iterator)++;
            return tuple.ptr();
        } else {
            return nullptr;
        }
    }

    static void init_type()
    {
        behaviors().name("Element map iterator");
        behaviors().doc("");
        behaviors().supportIter(Py::PythonType::support_iter_iter | Py::PythonType::support_iter_iternext);

        behaviors().readyType();
    }

    static Py::PythonClassObject<CyPy_MapElementIterator> wrap(CyPy_ElementMap* value)
    {
        auto obj = extension_object_new(type_object(), nullptr, nullptr);
        reinterpret_cast<Py::PythonClassInstance*>(obj)->m_pycxx_object = new CyPy_MapElementIterator(reinterpret_cast<Py::PythonClassInstance*>(obj), value);
        return Py::PythonClassObject<CyPy_MapElementIterator>(obj, true);
    }


};

CyPy_ElementList::CyPy_ElementList(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds) :
    WrapperBase(self, args, kwds)
{
    for (auto entry : args) {
        m_value.push_back(CyPy_Element::asElement(entry));
    }
}

CyPy_ElementList::CyPy_ElementList(Py::PythonClassInstance* self, Atlas::Message::ListType element)
    : WrapperBase(self, std::move(element))
{

}

void CyPy_ElementList::init_type()
{
    behaviors().name("Element List");
    behaviors().doc("");

    behaviors().supportRepr();
    behaviors().supportRichCompare();

    behaviors().supportIter(Py::PythonType::support_iter_iter);
    behaviors().supportSequenceType(Py::PythonType::support_sequence_length
                                    | Py::PythonType::support_sequence_contains
                                    | Py::PythonType::support_sequence_item
                                    | Py::PythonType::support_sequence_ass_item
                                    | Py::PythonType::support_sequence_repeat
                                    | Py::PythonType::support_sequence_slice
                                    | Py::PythonType::support_sequence_inplace_concat
                                    | Py::PythonType::support_sequence_inplace_repeat);

    behaviors().readyType();

    CyPy_ListElementIterator::init_type();
}

Py::Object CyPy_ElementList::repr()
{
    return Py::String(String::compose("<%1 object at %2>(%3)", type_object()->tp_name, this, debug_tostring(m_value)));
}

Py::Object CyPy_ElementList::rich_compare(const Py::Object& other, int op)
{

    if ((op != Py_EQ) && (op != Py_NE)) {
        throw Py::NotImplementedError("Element List object can only be check for == or !=.");
    }

    bool equal = false;

    if (CyPy_ElementList::check(other)) {
        equal = m_value == CyPy_ElementList::value(other);
    }

    if ((equal && op == Py_EQ) || (!equal && op == Py_NE)) {
        return Py::True();
    }
    return Py::False();
}


Py::Object CyPy_ElementList::iter()
{
    return CyPy_ListElementIterator::wrap(this);
}

PyCxx_ssize_t CyPy_ElementList::sequence_length()
{
    return m_value.size();
}

Py::Object CyPy_ElementList::sequence_repeat(Py_ssize_t count)
{
    Py::List list;
    for (Py_ssize_t i = 0; i < count; ++count) {
        for (auto& entry : m_value) {
            list.append(CyPy_Element::asPyObject(entry, false));
        }
    }
    return list;
}


int CyPy_ElementList::sequence_contains(const Py::Object& object)
{
    auto element = CyPy_Element::asElement(object);

    for (auto& entity : m_value) {
        if (entity == element) {
            return 1;
        }
    }
    return 0;
}

Py::Object CyPy_ElementList::sequence_inplace_repeat(Py_ssize_t)
{
//TODO: implement
    return self();
}

Py::Object CyPy_ElementList::sequence_inplace_concat(const Py::Object&)
{
//TODO: implement
    return self();
}

int CyPy_ElementList::sequence_ass_item(Py_ssize_t index, const Py::Object& object)
{
    if (index < static_cast<Py_ssize_t>(m_value.size())) {
        m_value[index] = CyPy_Element::asElement(object);
        return 1;
    }
    return -1;

}

Py::Object CyPy_ElementList::sequence_item(Py_ssize_t index)
{
    if (index < static_cast<Py_ssize_t>(m_value.size())) {
        return CyPy_Element::asPyObject(m_value.at(static_cast<unsigned long>(index)), false);
    }
    return Py::None();
}

Py::Object CyPy_ElementList::sequence_concat(const Py::Object& otherValue)
{
    auto element = CyPy_Element::asElement(otherValue);
    auto list = m_value;
    list.push_back(element);
    return wrap(list);
}


CyPy_ElementMap::CyPy_ElementMap(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    for (auto entry : kwds) {
        m_value.emplace(entry.first.as_string(), CyPy_Element::asElement(entry.second));
    }
}

CyPy_ElementMap::CyPy_ElementMap(Py::PythonClassInstance* self, Atlas::Message::MapType element)
    : WrapperBase(self, std::move(element))
{
}

void CyPy_ElementMap::init_type()
{
    behaviors().name("Element Map");
    behaviors().doc("");

    behaviors().supportRepr();
    behaviors().supportRichCompare();

    behaviors().supportMappingType(Py::PythonType::support_mapping_ass_subscript
                                   | Py::PythonType::support_mapping_subscript);
    behaviors().supportSequenceType(Py::PythonType::support_sequence_contains);

    PYCXX_ADD_NOARGS_METHOD(items, items, "");

    behaviors().readyType();

    CyPy_MapElementIterator::init_type();

}


Py::Object CyPy_ElementMap::repr()
{
    return Py::String(String::compose("<%1 object at %2>(%3)", type_object()->tp_name, this, debug_tostring(m_value)));
}


Py::Object CyPy_ElementMap::rich_compare(const Py::Object& other, int op)
{

    if ((op != Py_EQ) && (op != Py_NE)) {
        throw Py::NotImplementedError("Element Map object can only be check for == or !=.");
    }

    bool equal = false;

    if (CyPy_ElementMap::check(other)) {
        equal = m_value == CyPy_ElementMap::value(other);
    }

    if ((equal && op == Py_EQ) || (!equal && op == Py_NE)) {
        return Py::True();
    }
    return Py::False();
}


Py::Object CyPy_ElementMap::getattro(const Py::String& name)
{
    auto I = m_value.find(name);
    if (I != m_value.end()) {
        return CyPy_Element::asPyObject(I->second, false);
    }

    return PythonExtensionBase::getattro(name);
}

int CyPy_ElementMap::setattro(const Py::String& name, const Py::Object& attr)
{
    m_value.emplace(name.as_string(), CyPy_Element::asElement(attr));

    return 0;
}


Py::Object CyPy_ElementMap::mapping_subscript(const Py::Object& key)
{
    auto I = m_value.find(verifyString(key));
    if (I != m_value.end()) {
        return CyPy_Element::asPyObject(I->second, false);
    }
    return Py::None();
}

int CyPy_ElementMap::mapping_ass_subscript(const Py::Object& key, const Py::Object& value)
{
    m_value[verifyString(key)] = CyPy_Element::asElement(value);
    return 0;
}

Py::Object CyPy_ElementMap::items()
{
    return CyPy_MapElementIterator::wrap(this);
}

int CyPy_ElementMap::sequence_contains(const Py::Object& key)
{
    auto keyStr = verifyString(key);
    if (m_value.find(keyStr) != m_value.end()) {
        return 1;
    }
    return 0;
}

Py::Object CyPy_Element::mapAsPyObject(const MapType& map, bool useNativePythonType)
{
    Py::Dict dict;
    for (auto& entry : map) {
        if (useNativePythonType) {
            dict.setItem(entry.first, CyPy_Element::asPyObject(entry.second, useNativePythonType));
        } else {
            dict.setItem(entry.first, CyPy_Element::wrap(entry.second));
        }
    }
    return dict;
}

Py::Object CyPy_Element::listAsPyObject(const ListType& list, bool useNativePythonType)
{
    Py::List pyList;
    for (auto& entry : list) {
        if (useNativePythonType) {
            pyList.append(CyPy_Element::asPyObject(entry, useNativePythonType));
        } else {
            pyList.append(CyPy_Element::wrap(entry));
        }
    }
    return pyList;
}

Py::Object CyPy_Element::asPyObject(const Atlas::Message::Element& obj, bool useNativePythonType)
{
    switch (obj.getType()) {
        case Element::TYPE_INT:
            return Py::Long(obj.Int());
        case Element::TYPE_FLOAT:
            return Py::Float(obj.Float());
        case Element::TYPE_STRING:
            return Py::String(obj.String());
        case Element::TYPE_MAP:
            return mapAsPyObject(obj.Map(), useNativePythonType);
        case Element::TYPE_LIST:
            return listAsPyObject(obj.List(), useNativePythonType);
        default:
            break;
    }
    return Py::None();
}


ListType CyPy_Element::listAsElement(const Py::List& list)
{
    ListType res;
    for (auto& entry : list) {
        res.push_back(asElement(entry));
    }
    return res;
}

ListType CyPy_Element::sequenceAsElement(const Py::Sequence& sequence)
{
    ListType res;
    for (auto& entry : sequence) {
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
    if (CyPy_ElementList::check(o)) {
        return CyPy_ElementList::value(o);
    }
    if (CyPy_ElementMap::check(o)) {
        return CyPy_ElementMap::value(o);
    }
    if (CyPy_Operation::check(o)) {
        return CyPy_Operation::value(o)->asMessage();
    }
    if (CyPy_RootEntity::check(o)) {
        return CyPy_RootEntity::value(o)->asMessage();
    }
    if (CyPy_Root::check(o)) {
        return CyPy_Root::value(o)->asMessage();
    }
    if (CyPy_Oplist::check(o)) {
        auto& oplist = CyPy_Oplist::value(o);
        ListType list;
        for (auto& entry : oplist) {
            list.push_back(entry->asMessage());
        }
        return list;
    }
    if (CyPy_Location::check(o)) {
        MapType map;
        CyPy_Location::value(o).addToMessage(map);
        return map;
    }
    if (o.isList()) {
        return listAsElement(Py::List(o));
    }
    if (o.isDict()) {
        return dictAsElement(Py::Dict(o));
    }
    if (o.isSequence()) {
        return sequenceAsElement(Py::Sequence(o));
    }
    if (o.isNone()) {
        return Element();
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
    } else if (value.isList()) {
        return CyPy_ElementList::wrap(value.List());
    } else {
        return CyPy_ElementMap::wrap(value.Map());
    }
}
