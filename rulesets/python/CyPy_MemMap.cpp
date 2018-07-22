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

#include "CyPy_MemMap.h"
#include "CyPy_MemEntity.h"
#include "CyPy_Element.h"
#include "CyPy_RootEntity.h"
#include "CyPy_EntityFilter.h"

#include <Atlas/Objects/objectFactory.h>
#include <rulesets/Py_Message.h>
#include <rulesets/Py_Thing.h>


using Atlas::Objects::Root;
using Atlas::Objects::Factories;
using Atlas::Objects::Entity::RootEntity;

CyPy_MemMap::CyPy_MemMap(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    args.verify_length(1);
    //verifyObject<CyPy_LocatedEntity>(args.front());
}

CyPy_MemMap::CyPy_MemMap(Py::PythonClassInstance* self, MemMap* value)
    : WrapperBase(self, value)
{

}

void CyPy_MemMap::init_type()
{
    behaviors().name("MemMap");
    behaviors().doc("");

    PYCXX_ADD_VARARGS_METHOD(find_by_location, find_by_location, "");

    PYCXX_ADD_VARARGS_METHOD(find_by_type, find_by_type, "");

    PYCXX_ADD_VARARGS_METHOD(updateAdd, updateAdd, "");

    PYCXX_ADD_VARARGS_METHOD(delete, delete_, "");

    PYCXX_ADD_VARARGS_METHOD(get, get, "");

    PYCXX_ADD_VARARGS_METHOD(get_add, get_add, "");

    PYCXX_ADD_VARARGS_METHOD(add_hooks_append, add_hooks_append, "");

    PYCXX_ADD_VARARGS_METHOD(update_hooks_append, update_hooks_append, "");

    PYCXX_ADD_VARARGS_METHOD(delete_hooks_append, delete_hooks_append, "");

    PYCXX_ADD_VARARGS_METHOD(find_by_filter, find_by_filter, "");

    PYCXX_ADD_VARARGS_METHOD(find_by_location_query, find_by_location_query, "");

    PYCXX_ADD_VARARGS_METHOD(add_entity_memory, add_entity_memory, "");

    PYCXX_ADD_VARARGS_METHOD(recall_entity_memory, recall_entity_memory, "");


    behaviors().readyType();
}

Py::Object CyPy_MemMap::find_by_location(const Py::Tuple& args)
{
    Py::List list;
    args.verify_length(3);
    auto& location = verifyObject<CyPy_Location>(args[0]);
    if (!location.isValid()) {
        throw Py::TypeError("Location is incomplete");
    }
    auto radius = verifyNumeric(args[1]);
    auto type = verifyString(args[2]);
    auto res = m_value->findByLocation(location, radius, type);

    for (auto& entry : res) {
        list.append(CyPy_LocatedEntity::wrap(entry));
    }
    return list;
}

Py::Object CyPy_MemMap::find_by_type(const Py::Tuple& args)
{
    Py::List list;

    args.verify_length(1);
    auto what = verifyString(args.front());
    auto res = m_value->findByType(what);

    for (auto& entry : res) {
        list.append(CyPy_LocatedEntity::wrap(entry));
    }
    return list;
}

Py::Object CyPy_MemMap::updateAdd(const Py::Tuple& args)
{
    args.verify_length(2);

    double time = verifyNumeric(args[1]);
    if (CyPy_Element::check(args[0])) {
        auto& element = CyPy_Element::value(args[0]);
        if (!element.isMap()) {
            throw Py::TypeError("arg is a Message that is not a map");
        }
        try {
            Root obj = Factories::instance()->createObject(element.Map());

            RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(obj);
            if (!ent.isValid()) {
                throw Py::TypeError("arg is a Message that does not represent an entity");
            }
            auto* ret = m_value->updateAdd(ent, time);
            if (!ret) {
                throw Py::TypeError("arg is a Message that does not have an ID");
            }
            return CyPy_MemEntity::wrap(ret);
        }
        catch (Atlas::Message::WrongTypeException&) {
            throw Py::TypeError("arg is a Message that contains malformed attributes");
        }
    } else if (CyPy_RootEntity::check(args[0])) {
        auto& ent = CyPy_RootEntity::value(args[0]);
        auto* ret = m_value->updateAdd(ent, time);
        if (!ret) {
            throw Py::TypeError("arg is a Message that does not have an ID");
        }
        return CyPy_MemEntity::wrap(ret);
    } else {
        throw Py::TypeError("arg is not an Atlas Entity or Message");
    }
}

Py::Object CyPy_MemMap::delete_(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->del(verifyString(args[0]));
    return Py::None();
}

Py::Object CyPy_MemMap::get(const Py::Tuple& args)
{
    args.verify_length(1);

    auto ret = m_value->get(verifyString(args[0]));
    if (!ret) {
        return Py::None();
    }
    return CyPy_MemEntity::wrap(ret);
}

Py::Object CyPy_MemMap::get_add(const Py::Tuple& args)
{
    args.verify_length(1);

    auto ret = m_value->getAdd(verifyString(args[0]));
    if (!ret) {
        return Py::None();
    }
    return CyPy_MemEntity::wrap(ret);
}

Py::Object CyPy_MemMap::add_hooks_append(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->getAddHooks().push_back(verifyString(args[0]));
    return Py::None();
}

Py::Object CyPy_MemMap::update_hooks_append(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->getUpdateHooks().push_back(verifyString(args[0]));
    return Py::None();
}

Py::Object CyPy_MemMap::delete_hooks_append(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->getDeleteHooks().push_back(verifyString(args[0]));
    return Py::None();
}

///\brief Return Python list of entities that match a given Filter
Py::Object CyPy_MemMap::find_by_filter(const Py::Tuple& args)
{
    args.verify_length(1);
    auto& filter = verifyObject<CyPy_Filter>(args[0]);

    Py::List list;

    for (auto& entry : m_value->getEntities()) {
        if (filter.match(*entry.second)) {
            list.append(CyPy_MemEntity::wrap(entry.second));
        }
    }
    return list;
}

///\brief find entities using a query in a specified location
Py::Object CyPy_MemMap::find_by_location_query(const Py::Tuple& args)
{
    args.verify_length(3);

    auto& location = verifyObject<CyPy_Location>(args[0]);
    auto radius = verifyNumeric(args[1]);
    auto& filter = verifyObject<CyPy_Filter>(args[2]);

    if (!location.isValid()) {
        throw Py::RuntimeError("Location is incomplete");
    }

    //Create a vector and fill it with entities that match the given filter and are in range
    float square_range = radius * radius;

    Py::List list;
    if (location.m_loc && location.m_loc->m_contains) {
        for (auto entry : *location.m_loc->m_contains) {
            if (entry->isVisible() && filter.match(*entry)) {
                if (squareDistance(location.pos(), entry->m_location.pos()) < square_range) {
                    list.append(CyPy_LocatedEntity::wrap(entry));
                }
            }
        }
    }

    return list;
}

Py::Object CyPy_MemMap::add_entity_memory(const Py::Tuple& args)
{

    args.verify_length(3);
    auto id = verifyString(args[0]);
    auto memory_name = verifyString(args[1]);

    m_value->addEntityMemory(id, memory_name, CyPy_Element::asElement(args[2]));

    return Py::None();
}

Py::Object CyPy_MemMap::recall_entity_memory(const Py::Tuple& args)
{

    args.verify_length(2);
    auto id = verifyString(args[0]);
    auto memory_name = verifyString(args[1]);

    Atlas::Message::Element element_val;
    m_value->recallEntityMemory(id, memory_name, element_val);
    if (element_val.isNone()) {
        return Py::None();
    }
    return CyPy_Element::wrap(element_val);
}


