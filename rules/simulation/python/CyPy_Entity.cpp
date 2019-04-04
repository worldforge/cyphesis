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

#include "CyPy_Entity.h"
#include "CyPy_Task.h"
#include "CyPy_EntityProps.h"
#include "rules/simulation/TasksProperty.h"
#include "rules/python/CyPy_Operation.h"
#include "rules/python/CyPy_Oplist.h"
#include "common/id.h"

CyPy_Entity::CyPy_Entity(Py::PythonClassInstanceWeak* self, Py::Tuple& args, Py::Dict& kwds)
    : CyPy_LocatedEntityBase(self, args, kwds)
{
    args.verify_length(1);

    auto arg = args.front();
    if (arg.isString()) {
        auto id = verifyString(args.front());

        long intId = integerId(id);
        if (intId == -1L) {
            throw Py::TypeError("Entity() requires string/int ID");
        }
        m_value = new Entity(id, intId);
    } else if (CyPy_Entity::check(arg)) {
        m_value = CyPy_Entity::value(arg);
    } else {
        throw Py::TypeError("Entity() requires string ID or Entity");
    }
}


CyPy_Entity::CyPy_Entity(Py::PythonClassInstanceWeak* self, Ref<Entity> value)
    : CyPy_LocatedEntityBase(self, std::move(value))

{
}

CyPy_Entity::~CyPy_Entity() = default;

void CyPy_Entity::init_type()
{
    behaviors().name("Entity");
    behaviors().doc("");

    behaviors().supportRichCompare();
    behaviors().supportStr();

    PYCXX_ADD_VARARGS_METHOD(get_child, get_child, "");
    PYCXX_ADD_NOARGS_METHOD(as_entity, as_entity, "");
    PYCXX_ADD_VARARGS_METHOD(can_reach, can_reach, "");
    PYCXX_ADD_NOARGS_METHOD(describe_entity, describe_entity, "");
    PYCXX_ADD_VARARGS_METHOD(client_error, client_error, "");

    PYCXX_ADD_VARARGS_METHOD(get_prop_num, get_prop_num, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_num, has_prop_num, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_float, get_prop_float, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_float, has_prop_float, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_int, get_prop_int, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_int, has_prop_int, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_string, get_prop_string, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_string, has_prop_string, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_bool, get_prop_bool, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_bool, has_prop_bool, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_list, get_prop_list, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_list, has_prop_list, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_map, get_prop_map, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_map, has_prop_map, "");


    PYCXX_ADD_VARARGS_METHOD(send_world, send_world, "");
    PYCXX_ADD_VARARGS_METHOD(mod_property, mod_property, "");
    PYCXX_ADD_VARARGS_METHOD(start_task, start_task, "");


    behaviors().readyType();

    LocatedEntityScriptProvider provider{
        [](const Ref<LocatedEntity>& locatedEntity) -> Py::Object {
            auto memEntity = dynamic_cast<Entity*>(locatedEntity.get());
            if (memEntity) {
                return WrapperBase<Ref<Entity>, CyPy_Entity, Py::PythonClassInstanceWeak>::wrap(memEntity);
            }
            return Py::None();
        },
        [](PyObject* obj) -> bool {
            return CyPy_Entity::check(obj);
        }, [](const Py::Object& object) -> Ref<LocatedEntity>* {
            if (check(object)) {
                //This cast should work.
                return reinterpret_cast<Ref<LocatedEntity>*>(&WrapperBase<Ref<Entity>, CyPy_Entity, Py::PythonClassInstanceWeak>::value(object));
            }
            return nullptr;
        }
    };
    CyPy_LocatedEntity::entityPythonProviders.emplace_back(std::move(provider));
}


Py::Object CyPy_Entity::start_task(const Py::Tuple& args)
{
    return CyPy_Entity::start_task(m_value, args);
}

Py::Object CyPy_Entity::mod_property(const Py::Tuple& args)
{
    return CyPy_Entity::mod_property(m_value, args);
}


Py::Object CyPy_Entity::send_world(const Py::Tuple& args)
{
    return CyPy_Entity::send_world(m_value, args);
}


Py::Object CyPy_Entity::send_world(const Ref<Entity>& entity, const Py::Tuple& args)
{
    args.verify_length(1);
    entity->sendWorld(verifyObject<CyPy_Operation>(args.front()));
    return Py::None();
}

Py::Object CyPy_Entity::start_task(const Ref<Entity>& entity, const Py::Tuple& args)
{
    OpVector res;
    args.verify_length(2);

    auto tp = entity->requirePropertyClassFixed<TasksProperty>();
    tp->startTask(verifyString(args[0]), verifyObject<CyPy_Task>(args[1]), entity.get(), res);

    return CyPy_Oplist::wrap(std::move(res));
}

Py::Object CyPy_Entity::mod_property(const Ref<Entity>& entity, const Py::Tuple& args)
{
    OpVector res;
    args.verify_length(1, 2);
    auto name = verifyString(args.front());

    PropertyBase* prop;
    if (args.length() == 2) {
        auto defaultElement = CyPy_Element::asElement(args[1]);
        prop = entity->modProperty(name, defaultElement);
    } else {
        prop = entity->modProperty(name);
    }
    if (!prop) {
        return Py::None();
    }
    Atlas::Message::Element value;
    if (prop->get(value) != 0) {
        throw Py::RuntimeError(String::compose("Could not create property '%1'.", name));
    }
    return CyPy_Element::asPyObject(value, false);
}

Py::Object CyPy_Entity::getattro(const Py::String& name)
{
    if (name.as_string() == "props") {
        return CyPy_EntityProps::wrap(this->m_value);
    }
    return CyPy_LocatedEntityBase::getattro(name);
}

int CyPy_Entity::setattro(const Py::String& name, const Py::Object& attr)
{
    if (name.as_string() == "props") {
        throw Py::AttributeError("Can not set 'props'.");
    }
    return CyPy_LocatedEntityBase::setattro(name, attr);
}
