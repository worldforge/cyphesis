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

#include "rules/AtlasProperties.h"
#include "rules/PhysicalProperties.h"
#include "CyPy_MemMap.h"
#include "CyPy_MemEntity.h"
#include "rules/python/CyPy_RootEntity.h"
#include "rules/entityfilter/python/CyPy_EntityFilter.h"

#include "rules/entityfilter/Providers.h"


using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

CyPy_MemMap::CyPy_MemMap(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
        : WrapperBase(self, args, kwds)
{
    throw Py::TypeError("MemMap() can't be instanced from Python.");
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

    PYCXX_ADD_VARARGS_METHOD(add, updateAdd, "");

    PYCXX_ADD_VARARGS_METHOD(update, updateAdd, "");

    PYCXX_ADD_VARARGS_METHOD(delete, delete_, "");

    PYCXX_ADD_NOARGS_METHOD(get_all, get_all, "");

    PYCXX_ADD_VARARGS_METHOD(get, get, "");

    PYCXX_ADD_VARARGS_METHOD(get_add, get_add, "");

    PYCXX_ADD_VARARGS_METHOD(find_by_filter, find_by_filter, "");

    PYCXX_ADD_VARARGS_METHOD(find_by_location_query, find_by_location_query, "");

    PYCXX_ADD_VARARGS_METHOD(add_entity_memory, add_entity_memory, "add_entity_memory(entity_id, name, memory)\n--\n\nAdds memory for an entity. First parameter is entity id, second is memory name, and third is the memory contents.");

    PYCXX_ADD_VARARGS_METHOD(remove_entity_memory, remove_entity_memory,
                             "Removes memory for an entity. First parameter is entity id, second is memory name. If no memory name is supplied all memory for that entity will be removed");

    PYCXX_ADD_VARARGS_METHOD(recall_entity_memory, recall_entity_memory, "");


    behaviors().readyType();
}

Py::Object CyPy_MemMap::find_by_location(const Py::Tuple& args)
{
    Py::List list;
    args.verify_length(3);
    auto& location = verifyObject<CyPy_Location>(args[0]);
    if (!location.isValid()) {
        throw Py::RuntimeError("Location is incomplete");
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
    if (args[0].isDict()) {
        auto element = CyPy_Element::dictAsElement(Py::Dict(args[0]));
        try {

            Root obj = m_value->getTypeStore().getFactories().createObject(element);

            RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(obj);
            if (!ent.isValid()) {
                throw Py::TypeError("arg is a Message that does not represent an entity");
            }
            auto ret = m_value->updateAdd(ent, time);
            if (!ret) {
                throw Py::TypeError("arg is a Message that does not have an ID");
            }
            return CyPy_MemEntity::wrap(ret.get());
        }
        catch (Atlas::Message::WrongTypeException&) {
            throw Py::TypeError("arg is a Message that contains malformed attributes");
        }
    } else if (CyPy_RootEntity::check(args[0])) {
        auto& ent = CyPy_RootEntity::value(args[0]);
        auto ret = m_value->updateAdd(ent, time);
        if (!ret) {
            throw Py::TypeError("arg is a Message that does not have an ID");
        }
        return CyPy_MemEntity::wrap(ret.get());
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
    return CyPy_MemEntity::wrap(ret.get());
}

Py::Object CyPy_MemMap::get_add(const Py::Tuple& args)
{
    args.verify_length(1);

    auto ret = m_value->getAdd(verifyString(args[0]));
    if (!ret) {
        return Py::None();
    }
    return CyPy_MemEntity::wrap(ret.get());
}

Py::Object CyPy_MemMap::get_all()
{
    Py::List list;

    for (auto& entry : m_value->getEntities()) {
        list.append(CyPy_MemEntity::wrap(entry.second.get()));
    }
    return list;
}

///\brief Return Python list of entities that match a given Filter
Py::Object CyPy_MemMap::find_by_filter(const Py::Tuple& args)
{
    args.verify_length(1);
    auto& filter = verifyObject<CyPy_Filter>(args[0]);

    Py::List list;

    for (auto& entry : m_value->getEntities()) {
        EntityFilter::QueryContext queryContext = createFilterContext(entry.second.get(), m_value);

        if (filter->match(queryContext)) {
            list.append(CyPy_MemEntity::wrap(entry.second.get()));
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
    if (location.m_parent && location.m_parent->m_contains) {
        for (const auto& entry : *location.m_parent->m_contains) {
            EntityFilter::QueryContext queryContext = createFilterContext(entry.get(), m_value);

            if (entry->isVisible() && filter->match(queryContext)) {
                auto pos = PositionProperty::extractPosition(*entry);
                if (pos.isValid() && squareDistance(location.pos(), pos) < square_range) {
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

Py::Object CyPy_MemMap::remove_entity_memory(const Py::Tuple& args)
{

    args.verify_length(1, 2);
    auto id = verifyString(args[0]);
    if (args.length() == 2) {
        m_value->removeEntityMemory(id, verifyString(args[1]));
    } else {
        m_value->removeEntityMemory(id, "");
    }

    return Py::None();
}

Py::Object CyPy_MemMap::recall_entity_memory(const Py::Tuple& args)
{

    args.verify_length(2, 3);
    auto id = verifyString(args[0]);
    auto memory_name = verifyString(args[1]);

    Atlas::Message::Element element_val;
    m_value->recallEntityMemory(id, memory_name, element_val);
    if (element_val.isNone()) {
        //Check if a default value was specified as third parameter.
        if (args.length() == 3) {
            return args[2];
        }
        return Py::None();
    }
    return CyPy_Element::wrap(element_val);
}

EntityFilter::QueryContext CyPy_MemMap::createFilterContext(LocatedEntity* entity, MemMap* memMap)
{
    EntityFilter::QueryContext queryContext{*entity};
    queryContext.type_lookup_fn = [memMap](const std::string& id) -> const TypeNode* {
        return memMap->getTypeStore().getType(id);
    };
    queryContext.entity_lookup_fn = [memMap](const std::string& id) -> Ref<LocatedEntity> {
        return memMap->get(id);
    };
    queryContext.memory_lookup_fn = [memMap](const std::string& id) -> const Atlas::Message::MapType& {
        auto I = memMap->getEntityRelatedMemory().find(id);
        if (I != memMap->getEntityRelatedMemory().end()) {
            return I->second;
        }
        static Atlas::Message::MapType empty;
        return empty;
    };
    return queryContext;
}


