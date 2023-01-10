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

#include "CyPy_Operation.h"
#include "CyPy_Element.h"
#include "CyPy_RootEntity.h"
#include "CyPy_Root.h"
#include "CyPy_Oplist.h"
#include <Atlas/Objects/Generic.h>
#include <Atlas/Objects/Entity.h>
#include <common/log.h>
#include <common/Inheritance.h>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Generic;
using Atlas::Objects::Entity::RootEntity;

CyPy_Operation::CyPy_Operation(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    if (args.size() == 0) {
        throw Py::TypeError("Must submit argument for operation type.");
    }

    auto parent = verifyString(args.front());
    Root r = Inheritance::instance().getFactories().createObject(parent);
    m_value = std::move(Atlas::Objects::smart_dynamic_cast<RootOperation>(r));
    if (!m_value) {
        m_value = Generic();
        m_value->setParent(parent);
    }

    for (auto entry : kwds) {
        if (entry.first.isString()) {
            setattro(entry.first.as_string(), entry.second);
        }
    }

    if (args.size() > 1) {
        auto& args_list = m_value->modifyArgs();
        for (PyCxx_ssize_t i = 1; i < args.size(); ++i) {
            addToArgs(args_list, args[i]);
        }
    }

}

CyPy_Operation::CyPy_Operation(Py::PythonClassInstance* self, Atlas::Objects::Operation::RootOperation value)
    : WrapperBase(self, std::move(value))
{

}

void CyPy_Operation::addToArgs(std::vector<Root>& args, const Py::Object& arg)
{
    if (arg.isDict()) {
        args.push_back(Inheritance::instance().getFactories().createObject(CyPy_Element::dictAsElement(Py::Dict(arg))));
    } else if (CyPy_ElementMap::check(arg)) {
        args.push_back(Inheritance::instance().getFactories().createObject(CyPy_ElementMap::value(arg)));
    } else if (CyPy_Operation::check(arg)) {
        args.push_back(CyPy_Operation::value(arg));
    } else if (CyPy_RootEntity::check(arg)) {
        args.push_back(CyPy_RootEntity::value(arg));
    } else if (CyPy_Root::check(arg)) {
        args.push_back(CyPy_Root::value(arg));
    } else {
        throw Py::TypeError("Operation arg is of unknown type");
    }
}

void CyPy_Operation::init_type()
{
    behaviors().name("Operation");
    behaviors().doc("An Atlas Operation.");

    behaviors().supportNumberType(Py::PythonType::support_number_add);
    behaviors().supportSequenceType(Py::PythonType::support_sequence_length
                                    | Py::PythonType::support_sequence_item
                                    | Py::PythonType::support_sequence_contains);

    behaviors().supportMappingType(Py::PythonType::support_mapping_ass_subscript
                                   | Py::PythonType::support_mapping_subscript);



    PYCXX_ADD_VARARGS_METHOD(set_serialno, setSerialno, PYCXX_SIG_DOC("set_serialno(long)", "Sets the serial number."));
    PYCXX_ADD_VARARGS_METHOD(set_refno, setRefno, PYCXX_SIG_DOC("set_refno(long)", "Sets the reference number."));
    PYCXX_ADD_VARARGS_METHOD(set_from, setFrom, PYCXX_SIG_DOC("set_from(string)", "Sets from which entity the operation is from."));
    PYCXX_ADD_VARARGS_METHOD(set_to, setTo, PYCXX_SIG_DOC("set_to(string)", "Sets to which entity the operation is directed."));
    PYCXX_ADD_VARARGS_METHOD(set_seconds, setSeconds, "");
    PYCXX_ADD_VARARGS_METHOD(set_future_seconds, setFutureSeconds, "");
    PYCXX_ADD_VARARGS_METHOD(set_name, setName, "");
    PYCXX_ADD_VARARGS_METHOD(set_args, setArgs, PYCXX_SIG_DOC("set_args(sequence)", "Sets the argument list. The supplied sequence could be any of Operation|ElementMap|RootEntity|Python Dict."));
    PYCXX_ADD_NOARGS_METHOD(get_serialno, getSerialno, "");
    PYCXX_ADD_NOARGS_METHOD(is_default_serialno, isDefaultSerialno, "");
    PYCXX_ADD_NOARGS_METHOD(get_refno, getRefno, "");
    PYCXX_ADD_NOARGS_METHOD(get_from, getFrom, "");
    PYCXX_ADD_NOARGS_METHOD(get_to, getTo, "");
    PYCXX_ADD_NOARGS_METHOD(get_seconds, getSeconds, "");
    PYCXX_ADD_NOARGS_METHOD(get_future_seconds, getFutureSeconds, "");
    PYCXX_ADD_NOARGS_METHOD(get_args, getArgs, "");
    PYCXX_ADD_NOARGS_METHOD(get_name, get_name, "");
    PYCXX_ADD_NOARGS_METHOD(copy, copy, "Copies the operation into a new instance.");

    behaviors().readyType();
}


Py::Object CyPy_Operation::mapping_subscript(const Py::Object& key)
{
    if (key.isLong()) {
        return sequence_item(Py::Long(key));
    }
    Element attr;
    if (m_value->copyAttr(verifyString(key), attr) == 0) {
        if (attr.isPtr()) {
            return Py::Object((PyObject*) attr.Ptr());
        }
        return CyPy_Element::asPyObject(attr, false);
    }
    return Py::None();
}

int CyPy_Operation::mapping_ass_subscript(const Py::Object& key, const Py::Object& value)
{
    m_value->setAttr(verifyString(key), CyPy_Element::asElement(value));
    return 0;
}

int CyPy_Operation::sequence_contains(const Py::Object& key)
{
    auto keyStr = verifyString(key);
    if (m_value->hasAttr(keyStr)) {
        return 1;
    }
    return 0;
}

Py::Object CyPy_Operation::setSerialno(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->setSerialno(verifyLong(args.front()));
    return Py::None();
}

Py::Object CyPy_Operation::setRefno(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->setRefno(verifyLong(args.front()));
    return Py::None();
}

Py::Object CyPy_Operation::setFrom(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->setFrom(verifyString(args.front()));
    return Py::None();
}

Py::Object CyPy_Operation::setTo(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->setTo(verifyString(args.front()));
    return Py::None();
}

Py::Object CyPy_Operation::setSeconds(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->setSeconds(verifyFloat(args.front()));
    return Py::None();
}

Py::Object CyPy_Operation::setFutureSeconds(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->setFutureSeconds(verifyFloat(args.front()));
    return Py::None();
}

Py::Object CyPy_Operation::setName(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->setName(verifyString(args.front()));
    return Py::None();
}

Py::Object CyPy_Operation::setArgs(const Py::Tuple& args)
{
    args.verify_length(1);
    auto arg = args.front();
    if (!arg.isSequence()) {
        throw Py::TypeError("Must submit sequence.");
    }
    Py::Sequence argsAsSequence(args.front());
    std::vector<Root> argslist;

    for (auto item : argsAsSequence) {
        if (CyPy_Operation::check(item)) {
            argslist.push_back(CyPy_Operation::value(item));
        } else if (item.isDict()) {
            argslist.push_back(Inheritance::instance().getFactories().createObject(CyPy_Element::dictAsElement(Py::Dict(item))));
        } else if (CyPy_ElementMap::check(item)) {
            argslist.push_back(Inheritance::instance().getFactories().createObject(CyPy_ElementMap::value(item)));
        } else if (CyPy_RootEntity::check(item)) {
            argslist.push_back(CyPy_RootEntity::value(item));
        } else {
            throw Py::TypeError("args contains non Atlas Object");
        }
    }
    m_value->setArgs(argslist);

    return Py::None();
}

Py::Object CyPy_Operation::getSerialno()
{
    return Py::Long(m_value->getSerialno());
}

Py::Object CyPy_Operation::isDefaultSerialno()
{
    return Py::Boolean(m_value->isDefaultSerialno());
}

Py::Object CyPy_Operation::getRefno()
{
    return Py::Long(m_value->getRefno());
}

Py::Object CyPy_Operation::getFrom()
{
    return Py::String(m_value->getFrom());
}

Py::Object CyPy_Operation::getTo()
{
    return Py::String(m_value->getTo());
}

Py::Object CyPy_Operation::getSeconds()
{
    return Py::Float(m_value->getSeconds());
}

Py::Object CyPy_Operation::getFutureSeconds()
{
    return Py::Float(m_value->getFutureSeconds());
}

Py::Object CyPy_Operation::getName()
{
    return Py::String(m_value->getName());
}

Py::Object CyPy_Operation::getArgs()
{
    auto args_list = m_value->getArgs();
    Py::List list;
    for (auto& arg : args_list) {
        auto elementObj = CyPy_Element::wrap(arg->asMessage());
        list.append(elementObj);
    }
    return list;
}


Py::Object CyPy_Operation::get_name()
{
    return Py::String("op");
}

PyCxx_ssize_t CyPy_Operation::sequence_length()
{
    return Py::Long(static_cast<unsigned long>(m_value->getArgs().size()));
}

Py::Object CyPy_Operation::sequence_item(Py_ssize_t item)
{

    auto& args = m_value->getArgs();
    if (item < 0 || item >= (Py_ssize_t) args.size()) {
        throw Py::IndexError("Operation.[]: Not enough op arguments");
    }
    const Root& arg = args[item];
    RootOperation op = Atlas::Objects::smart_dynamic_cast<RootOperation>(arg);
    if (op) {
        return CyPy_Operation::wrap(std::move(op));
    }
    RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(arg);
    if (ent) {
        return CyPy_RootEntity::wrap(std::move(ent));
    }
    Root root = Atlas::Objects::smart_dynamic_cast<Root>(arg);
    if (root) {
        return CyPy_Root::wrap(std::move(root));
    }
    //log(WARNING, "Non operation or entity being returned as arg of operation");
    return CyPy_Element::wrap(arg->asMessage());
}

Py::Object CyPy_Operation::number_add(const Py::Object& other)
{

    //It's not clear what use this is...
    if (other.isNone()) {
        return self();
    }

    if (CyPy_Oplist::check(other)) {
        auto list = CyPy_Oplist::value(other);
        list.push_back(m_value);
        return CyPy_Oplist::wrap(list);
    }
    if (CyPy_Operation::check(other)) {
        std::vector<Atlas::Objects::Operation::RootOperation> oplist;
        oplist.push_back(m_value);
        oplist.push_back(CyPy_Operation::value(other));
        return CyPy_Oplist::wrap(oplist);
    }
    throw Py::TypeError("Unknown other in Operation.num_add");
}

Py::Object CyPy_Operation::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();

    if (nameStr == "from_") {
        return Py::String(m_value->getFrom());
    }
    if (nameStr == "to") {
        return Py::String(m_value->getTo());
    }
    if (nameStr == "id") {
        return Py::String(m_value->getId());
    }
    if (nameStr == "parent") {
        return Py::String(m_value->getParent());
    }

    if (nameStr == "future_seconds") {
        return getFutureSeconds();
    }
    if (nameStr == "name") {
        return getName();
    }
    if (nameStr == "serialno") {
        return getSerialno();
    }
    if (nameStr == "refno") {
        return getRefno();
    }
    if (nameStr == "args") {
        return getArgs();
    }

    return PythonExtensionBase::getattro(name);
}

int CyPy_Operation::setattro(const Py::String& name, const Py::Object& attr)
{
    auto nameStr = name.as_string();

    if (nameStr == "from_") {
        if (attr.isString()) {
            m_value->setFrom(attr.as_string());
            return 0;
        } else if (attr.hasAttr("id") && attr.getAttr("id").isString()) {
            m_value->setFrom(attr.getAttr("id").as_string());
            return 0;
        } else if (attr.isDict()) {
            Py::Dict dict(attr);
            if (dict.hasKey("id")) {
                auto value = dict.getItem("id");
                if (value.isString()) {
                    m_value->setFrom(value.as_string());
                    return 0;
                }
            }
        }
        throw Py::TypeError("from_ is neither a string nor an object with an 'id' attribute");
    }
    if (nameStr == "to") {
        if (attr.isString()) {
            m_value->setTo(attr.as_string());
            return 0;
        } else if (attr.hasAttr("id") && attr.getAttr("id").isString()) {
            m_value->setTo(attr.getAttr("id").as_string());
            return 0;
        } else if (attr.isDict()) {
            Py::Dict dict(attr);
            if (dict.hasKey("id")) {
                auto value = dict.getItem("id");
                if (value.isString()) {
                    m_value->setTo(value.as_string());
                    return 0;
                }
            }
        }
        throw Py::TypeError("to is neither a string nor an object with an 'id' attribute");

    }
    if (nameStr == "future_seconds") {
        m_value->setFutureSeconds(verifyFloat(attr));
        return 0;
    }
    if (nameStr == "name") {
        m_value->setName(verifyString(attr));
        return 0;
    }
    if (nameStr == "serialno") {
        m_value->setSerialno(verifyLong(attr));
        return 0;
    }
    if (nameStr == "refno") {
        m_value->setRefno(verifyLong(attr));
        return 0;
    }
    if (nameStr == "args") {
        setArgs(Py::Tuple(attr));
        return 0;
    }
    if (nameStr == "id") {
        m_value->setId(verifyString(attr));
        return 0;
    }

    throw Py::AttributeError("unknown attribute");
}

Py::Object CyPy_Operation::copy()
{
    Atlas::Objects::Operation::RootOperation copy(m_value->copy());
    return CyPy_Operation::wrap(copy);
}

