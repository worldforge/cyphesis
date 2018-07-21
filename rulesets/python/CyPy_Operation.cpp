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
#include "CyPy_EntityFilter.h"
#include "CyPy_Oplist.h"
#include <Atlas/Objects/Generic.h>
#include <Atlas/Objects/Entity.h>
#include <common/log.h>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Factories;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Generic;
using Atlas::Objects::Entity::RootEntity;

CyPy_Operation::CyPy_Operation(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    if (args.size() == 0) {
        throw Py::TypeError("Must submit argument for operation type.");
    }

    auto typeArg = args.front();
    if (!typeArg.isString()) {
        throw Py::TypeError("First argument must be string.");
    }

    Root r = Atlas::Objects::Factories::instance()->createObject(Py::String(typeArg).as_string());
    m_value = Atlas::Objects::smart_dynamic_cast<RootOperation>(r);
    if (!m_value) {
        // PyErr_SetString(PyExc_TypeError, "Operation() unknown operation type requested");
        // return -1;
        m_value = Generic();
        m_value->setParent(Py::String(typeArg).as_string());
    }
    if (kwds.size()) {
        if (kwds.hasKey("from_")) {
            auto fromObj = kwds.getItem("from_");
            if (fromObj.isString()) {
                m_value->setFrom(fromObj.as_string());
            } else if (fromObj.hasAttr("id") && fromObj.getAttr("id").isString()) {
                m_value->setFrom(fromObj.getAttr("id").as_string());
            } else {
                throw Py::TypeError("from is not a string and has no id");
            }
        }

        if (kwds.hasKey("to")) {
            auto toObj = kwds.getItem("to");
            if (toObj.isString()) {
                m_value->setFrom(toObj.as_string());
            } else if (toObj.hasAttr("id") && toObj.getAttr("id").isString()) {
                m_value->setTo(toObj.getAttr("id").as_string());
            } else {
                throw Py::TypeError("to is not a string and has no id");
            }
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
    if (CyPy_Element::check(arg)) {
        Py::PythonClassObject<CyPy_Element> obj(arg);
        auto& o = obj.getCxxObject()->m_value;
        if (o.isMap()) {
            args.push_back(Atlas::Objects::Factories::instance()->createObject(o.asMap()));
        } else {
            throw Py::TypeError("Operation arg is not a map");
        }
    } else if (CyPy_Operation::check(arg)) {
        Py::PythonClassObject<CyPy_Operation> obj(arg);
        args.push_back(obj.getCxxObject()->m_value);
    } else if (CyPy_RootEntity::check(arg)) {
        Py::PythonClassObject<CyPy_RootEntity> obj(arg);
        args.push_back(obj.getCxxObject()->m_value);
    } else {
        throw Py::TypeError("Operation arg is of unknown type");
    }
}

void CyPy_Operation::init_type()
{
    behaviors().name("Operation");
    behaviors().doc("");

    behaviors().supportNumberType(Py::PythonType::support_number_add);
    behaviors().supportSequenceType(Py::PythonType::support_sequence_length
                                    | Py::PythonType::support_sequence_item);


    PYCXX_ADD_VARARGS_METHOD(setSerialno, setSerialno, "");
    PYCXX_ADD_VARARGS_METHOD(setRefno, setRefno, "");
    PYCXX_ADD_VARARGS_METHOD(setFrom, setFrom, "");
    PYCXX_ADD_VARARGS_METHOD(setTo, setTo, "");
    PYCXX_ADD_VARARGS_METHOD(setSeconds, setSeconds, "");
    PYCXX_ADD_VARARGS_METHOD(setFutureSeconds, setFutureSeconds, "");
    PYCXX_ADD_VARARGS_METHOD(setName, setName, "");
    PYCXX_ADD_VARARGS_METHOD(setArgs, setArgs, "");
    PYCXX_ADD_NOARGS_METHOD(getSerialno, getSerialno, "");
    PYCXX_ADD_NOARGS_METHOD(isDefaultSerialno, isDefaultSerialno, "");
    PYCXX_ADD_NOARGS_METHOD(getRefno, getRefno, "");
    PYCXX_ADD_NOARGS_METHOD(getFrom, getFrom, "");
    PYCXX_ADD_NOARGS_METHOD(getTo, getTo, "");
    PYCXX_ADD_NOARGS_METHOD(getSeconds, getSeconds, "");
    PYCXX_ADD_NOARGS_METHOD(getFutureSeconds, getFutureSeconds, "");
    PYCXX_ADD_NOARGS_METHOD(getName, getName, "");
    PYCXX_ADD_NOARGS_METHOD(getArgs, getArgs, "");
    PYCXX_ADD_NOARGS_METHOD(get_name, get_name, "");

    behaviors().readyType();
}


Py::Object CyPy_Operation::setSerialno(const Py::Tuple& args)
{
    args.verify_length(1);
    auto arg = args.front();
    if (!arg.isLong()) {
        throw Py::TypeError("Must submit long.");
    }
    m_value->setSerialno(Py::Long(arg));
    return Py::None();
}

Py::Object CyPy_Operation::setRefno(const Py::Tuple& args)
{
    args.verify_length(1);
    auto arg = args.front();
    if (!arg.isLong()) {
        throw Py::TypeError("Must submit long.");
    }
    m_value->setRefno(Py::Long(args.front()));
    return Py::None();
}

Py::Object CyPy_Operation::setFrom(const Py::Tuple& args)
{
    args.verify_length(1);
    auto arg = args.front();
    if (!arg.isString()) {
        throw Py::TypeError("Must submit string.");
    }
    m_value->setFrom(Py::String(args.front()).as_string());
    return Py::None();
}

Py::Object CyPy_Operation::setTo(const Py::Tuple& args)
{
    args.verify_length(1);
    auto arg = args.front();
    if (!arg.isString()) {
        throw Py::TypeError("Must submit string.");
    }
    m_value->setTo(Py::String(args.front()).as_string());
    return Py::None();
}

Py::Object CyPy_Operation::setSeconds(const Py::Tuple& args)
{
    args.verify_length(1);
    auto arg = args.front();
    if (!arg.isNumeric()) {
        throw Py::TypeError("Must submit numeric.");
    }
    m_value->setSeconds(Py::Float(args.front()));
    return Py::None();
}

Py::Object CyPy_Operation::setFutureSeconds(const Py::Tuple& args)
{
    args.verify_length(1);
    auto arg = args.front();
    if (!arg.isNumeric()) {
        throw Py::TypeError("Must submit numeric.");
    }
    m_value->setFutureSeconds(Py::Float(args.front()));
    return Py::None();
}

Py::Object CyPy_Operation::setName(const Py::Tuple& args)
{
    args.verify_length(1);
    auto arg = args.front();
    if (!arg.isString()) {
        throw Py::TypeError("Must submit string.");
    }
    m_value->setName(Py::String(args.front()).as_string());
    return Py::None();
}

Py::Object CyPy_Operation::setArgs(const Py::Tuple& args)
{
    args.verify_length(1);
    auto arg = args.front();
    if (!arg.isList()) {
        throw Py::TypeError("Must submit list.");
    }
    Py::List argsAsList(args.front());
    std::vector<Root> argslist;

    for (auto item : argsAsList) {
        if (CyPy_Operation::check(item)) {
            argslist.push_back(Py::PythonClassObject<CyPy_Operation>(item).getCxxObject()->m_value);
        } else if (CyPy_Element::check(item)) {
            auto& e = CyPy_Element::value(item);
            if (!e.isMap()) {
                throw Py::TypeError("args contains non map");
            }
            argslist.push_back(Factories::instance()->createObject(e.Map()));
        } else if (CyPy_RootEntity::check(item)) {
            argslist.push_back(Py::PythonClassObject<CyPy_RootEntity>(item).getCxxObject()->m_value);
        } else {
            //std::cout << "o: " << i << item->ob_type->tp_name << std::endl << std::flush;
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
    return Py::Long(m_value->getArgs().size());
}

Py::Object CyPy_Operation::sequence_item(Py_ssize_t item)
{

    auto& args = m_value->getArgs();
    if (item < 0 || item >= (Py_ssize_t) args.size()) {
        throw Py::IndexError("Operation.[]: Not enough op arguments");
    }
    const Root& arg = args[item];
    RootOperation op = Atlas::Objects::smart_dynamic_cast<RootOperation>(arg);
    if (op.isValid()) {
        return CyPy_Operation::wrap(std::move(op));
    }
    RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(arg);
    if (ent.isValid()) {
        return CyPy_RootEntity::wrap(std::move(ent));
    }
    log(WARNING, "Non operation or entity being returned as arg of operation");
    return CyPy_Element::wrap(arg->asMessage());
}

Py::Object CyPy_Operation::number_add(const Py::Object& other)
{

    //It's not clear what use this is...
    if (other.isNone()) {
        return self();
    }

    if (CyPy_Oplist::check(other)) {
        Py::PythonClassObject<CyPy_Oplist> otherList(other);
        Py::PythonClassObject<CyPy_Oplist> res(Py::Callable(CyPy_Oplist::type()).apply());

        res.getCxxObject()->m_value = otherList.getCxxObject()->m_value;
        res.getCxxObject()->m_value.push_back(m_value);
        return res;
    }
    if (CyPy_Operation::check(other)) {
        Py::PythonClassObject<CyPy_Operation> otherOp(other);
        Py::PythonClassObject<CyPy_Oplist> res(Py::Callable(CyPy_Oplist::type()).apply());
        res.getCxxObject()->m_value.push_back(otherOp.getCxxObject()->m_value);
        res.getCxxObject()->m_value.push_back(m_value);
        return res;
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
    //Why should "id" return the parent?
    if (nameStr == "id") {
        return Py::String(m_value->getParent());
    }

    return PythonExtensionBase::getattro(name);
}

int CyPy_Operation::setattro(const Py::String& name, const Py::Object& attr)
{
    auto nameStr = name.as_string();

    //TODO: Original code looked for an attribute "id" in the submitted "attr" object.
        //We've kept this, but it's not clear exactly why it's this way. Perhaps look into changing it?
    if (nameStr == "from_") {
        if (attr.hasAttr("id")) {
            auto inner = attr.getAttr("id");
            if (inner.isString()) {
                m_value->setFrom(Py::String(inner));
                return 0;
            }
        }
        throw Py::TypeError("invalid from_");
    }
    if (nameStr == "to") {
        if (attr.hasAttr("id")) {
            auto inner = attr.getAttr("id");
            if (inner.isString()) {
                m_value->setTo(Py::String(inner));
                return 0;
            }
        }
        throw Py::TypeError("invalid to_");
    }
    throw Py::AttributeError("unknown attribute");
}

