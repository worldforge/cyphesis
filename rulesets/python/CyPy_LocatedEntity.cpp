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

#include <rulesets/Py_WorldTime.h>
#include <rulesets/Py_Location.h>
#include <rulesets/Py_Thing.h>
#include <common/id.h>
#include "CyPy_LocatedEntity.h"
#include "CyPy_Props.h"
#include "common/TypeNode.h"
#include "common/Inheritance.h"

Py::PythonClassObject<CyPy_LocatedEntity> wrapLocatedEntity(LocatedEntity* le)
{
    return CyPy_LocatedEntity::wrap(le);
}

CyPy_LocatedEntity::CyPy_LocatedEntity(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : PythonClass(self, args, kwds)
{

    args.verify_length(1);
    auto arg = args.front();
    auto id = arg.str().as_string();

    long intId = integerId(id);
    if (intId == -1L) {
        throw Py::TypeError("LocatedEntity() requires string/int ID");
    }
//    m_value = new Entity(id, intId);
}

CyPy_LocatedEntity::CyPy_LocatedEntity(Py::PythonClassInstance* self, LocatedEntity* value)
    : PythonClass(self), m_value(value)
{
    m_value->incRef();
}


CyPy_LocatedEntity::~CyPy_LocatedEntity()
{
    m_value->decRef();
}


void CyPy_LocatedEntity::init_type()
{
    behaviors().name("LocatedEntity");
    behaviors().doc("");

    behaviors().supportRichCompare();

    behaviors().readyType();
}

LocatedEntity& CyPy_LocatedEntity::value(const Py::Object& object)
{
    return *Py::PythonClassObject<CyPy_LocatedEntity>(object).getCxxObject()->m_value;
}

Py::Object CyPy_LocatedEntity::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "id") {
        return Py::String(m_value->getId());
    }

    if (nameStr == "props") {
        Py::Callable class_type(CyPy_Props::type());
        Py::PythonClassObject<CyPy_Props> propsObj(class_type.apply());
        propsObj.getCxxObject()->m_value = m_value;
        return propsObj;
    }

    if (nameStr == "type") {
        if (!m_value->getType()) {
            throw Py::AttributeError(name);
        }
        return Py::String(m_value->getType()->name());
    }

    //TODO: move to CyPy_BaseMind
//    if (nameStr == "time") {
//        PyWorldTime * worldtime = newPyWorldTime();
//        if (worldtime != nullptr) {
//            worldtime->time = m_value->getTime();
//        }
//        return Py::Object((PyObject *)worldtime);
//    }

    if (nameStr == "location") {
        PyLocation* loc = newPyLocation();
        if (loc != nullptr) {
            loc->location = &m_value->m_location;
            loc->owner = m_value;
        }
        return Py::Object((PyObject*) loc);
    }

    if (nameStr == "contains") {
        if (!m_value->m_contains) {
            return Py::None();
        }

        Py::List list;
        for (auto& child : *m_value->m_contains) {
            list.append(wrapLocatedEntity(child));
        }
        return list;
    }

    if (nameStr == "visible") {
        return Py::Boolean(m_value->isVisible());
    }

    return PythonExtensionBase::getattro(name);
}

int CyPy_LocatedEntity::setattro(const Py::String& name, const Py::Object& attr)
{
    auto nameStr = name.as_string();

    //FIXME: perhaps in BaseMind instead?
    if (nameStr == "map") {
        throw Py::AttributeError("map attribute forbidden");
    }
    if (nameStr == "type") {
        if (m_value->getType()) {
            throw Py::RuntimeError("Cannot mutate entity type");
        }

        const TypeNode* type = Inheritance::instance().getType(attr.str().as_string());
        if (!type) {
            throw Py::ValueError("Entity type unknown");
        }
        m_value->setType(type);
        return 0;
    }
    return PythonExtensionBase::setattro(name, attr);
}

Py::Object CyPy_LocatedEntity::rich_compare(const Py::Object& other, int op)
{

    if (other.isType(CyPy_LocatedEntity::type())) {
        Py::PythonClassObject<CyPy_LocatedEntity> otherObj(other);
        if (m_value == nullptr || otherObj.getCxxObject()->m_value == nullptr) {
            return Py::False();
        }
        if (op == Py_EQ) {
            return (m_value == otherObj.getCxxObject()->m_value) ? Py::True() : Py::False();
        } else if (op == Py_NE) {
            return (m_value != otherObj.getCxxObject()->m_value) ? Py::True() : Py::False();
        }

    } else {
        if (op == Py_EQ) {
            return Py::False();
        } else if (op == Py_NE) {
            return Py::True();
        }
    }

    throw Py::NotImplementedError();

}

Py::PythonClassObject<CyPy_LocatedEntity> CyPy_LocatedEntity::wrap(LocatedEntity* value)
{

    if (!value->m_scriptEntity.empty()) {
        return boost::any_cast<Py::PythonClassObject<CyPy_LocatedEntity>>(value->m_scriptEntity);
    } else {
        auto obj = CyPy_LocatedEntity::extension_object_new(CyPy_LocatedEntity::type_object(), nullptr, nullptr);
        new CyPy_LocatedEntity(reinterpret_cast<Py::PythonClassInstance*>(obj), value);
        Py::PythonClassObject<CyPy_LocatedEntity> wrapper(obj);
        value->m_scriptEntity = boost::any(wrapper);
        return wrapper;
    }



//    auto obj = (CyPy_LocatedEntity*)extension_object_new(type_object(), nullptr, nullptr);
//    obj->m_value = entity;
//    return Py::PythonClassObject<CyPy_LocatedEntity>(obj);

//    auto obj = (Py::PythonClassInstance*)extension_object_new(type_object(), nullptr, nullptr);
//    obj->m_pycxx_object = new CyPy_LocatedEntity(obj, entity);
//    return Py::PythonClassObject<CyPy_LocatedEntity>(obj);
}

