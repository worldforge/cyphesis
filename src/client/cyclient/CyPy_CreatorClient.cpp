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

#include "CyPy_CreatorClient.h"

#include "rules/python/CyPy_Element.h"
#include "rules/python/CyPy_RootEntity.h"
#include "rules/python/CyPy_LocatedEntity.h"
#include "rules/python/CyPy_Operation.h"
#include "rules/ai/python/CyPy_MemMap.h"
#include "rules/python/CyPy_Location.h"

#include "common/TypeNode.h"

CyPy_CreatorClient::CyPy_CreatorClient(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds) :
    WrapperBase(self, args, kwds)
{
    throw Py::RuntimeError("Can't be created from Python.");
}

CyPy_CreatorClient::CyPy_CreatorClient(Py::PythonClassInstance* self, Ref<CreatorClient> value)
    : WrapperBase(self, value)
{

}

CyPy_CreatorClient::~CyPy_CreatorClient() = default;

void CyPy_CreatorClient::init_type()
{
    behaviors().name("CreatorClient");
    behaviors().doc("");

    PYCXX_ADD_VARARGS_METHOD(make, make, "");
    PYCXX_ADD_VARARGS_METHOD(set, set, "");
    PYCXX_ADD_VARARGS_METHOD(send, send, "");
    PYCXX_ADD_VARARGS_METHOD(look, look, "");
    PYCXX_ADD_VARARGS_METHOD(look_for, look_for, "");
    PYCXX_ADD_VARARGS_METHOD(delete, delete_, "");
    PYCXX_ADD_NOARGS_METHOD(as_entity, as_entity, "");


    behaviors().readyType();
}


Py::Object CyPy_CreatorClient::as_entity()
{
    Atlas::Message::MapType element;
    m_value->addToMessage(element);
    return CyPy_Element::wrap(std::move(element));
}

Py::Object CyPy_CreatorClient::make(const Py::Tuple& args)
{
    args.verify_length(1);
    auto entity = verifyObject<CyPy_RootEntity>(args.front());
    auto retval = m_value->make(std::move(entity));

    if (!retval) {
        throw Py::RuntimeError("Entity creation failed");
    }
    return CyPy_LocatedEntity::wrap(retval);
}

Py::Object CyPy_CreatorClient::set(const Py::Tuple& args)
{
    args.verify_length(2);
    m_value->sendSet(verifyString(args[0]), verifyObject<CyPy_RootEntity>(args[1]));
    return Py::None();
}

Py::Object CyPy_CreatorClient::look(const Py::Tuple& args)
{
    args.verify_length(1);
    auto retval = m_value->look(verifyString(args.front()));
    if (!retval) {
        throw Py::RuntimeError("Entity look failed");
    }
    return CyPy_LocatedEntity::wrap(std::move(retval));
}

Py::Object CyPy_CreatorClient::look_for(const Py::Tuple& args)
{
    args.verify_length(1);
    auto retval = m_value->lookFor(verifyObject<CyPy_RootEntity>(args.front()));
    if (!retval) {
        return Py::None();
    }
    return CyPy_LocatedEntity::wrap(std::move(retval));
}

Py::Object CyPy_CreatorClient::send(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->send(verifyObject<CyPy_Operation>(args.front()));
    return Py::None();
}

Py::Object CyPy_CreatorClient::delete_(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->del(verifyString(args.front()));
    return Py::None();
}

Py::Object CyPy_CreatorClient::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    //Don't know what this does, commented out for now
//    // If operation search gets to here, it goes no further
//    if (strstr(name, "_operation") != nullptr) {
//        PyErr_SetString(PyExc_AttributeError, name);
//        return nullptr;
//    }
//    if (nameStr == "type") {
//        if (m_value->getType()) {
//            return Py::String(m_value->getType()->name());
//        } else {
//            return Py::None();
//        }
//    }
    if (nameStr == "map") {
        return CyPy_MemMap::wrap(m_value->getMap());
    }
    if (nameStr == "entity") {
        if (m_value->getEntity()) {
            return CyPy_LocatedEntity::wrap(m_value->getEntity());
        } else {
            return Py::None();
        }
    }
//    if (nameStr == "location") {
//        return CyPy_Location::wrap(m_value->m_location);
//    }
//    if (nameStr == "time") {
//        return CyPy_WorldTime::wrap(m_value->getTime());
//    }
//    Atlas::Message::Element attr;
//    if (m_value->getAttr(nameStr, attr) == 0) {
//        return CyPy_Element::wrap(std::move(attr));
//    }

    auto I = m_dict.find(nameStr);
    if (I != m_dict.end()) {
        return I->second;
    }

    return PythonExtensionBase::getattro(name);
}

int CyPy_CreatorClient::setattro(const Py::String& name, const Py::Object& attr)
{
    auto nameStr = name.as_string();

    if (nameStr == "map") {
        throw Py::AttributeError("map attribute forbidden");
    }

    m_dict[nameStr] = attr;

    return 0;
}

