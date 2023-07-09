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

#include "CyPy_Task.h"
#include "rules/python/CyPy_Operation.h"
#include "rules/python/CyPy_LocatedEntity.h"
#include "CyPy_UsageInstance.h"
#include "rules/python/CyPy_EntityLocation.h"
#include "rules/python/CyPy_Element.h"

#include "rules/python/PythonWrapper.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <rules/entityfilter/python/CyPy_EntityFilter.h>
#include <modules/Variant.h>
#include <rules/python/CyPy_Point3D.h>
#include <rules/python/CyPy_Vector3D.h>

template<>
Py::Object wrapPython(Task* value)
{
    return CyPy_Task::wrap(value);
}

CyPy_Task::CyPy_Task(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
        : WrapperBase(self, args, kwds)
{
    args.verify_length(1);
    auto arg = args.front();
    if (CyPy_Task::check(arg)) {
        m_value = CyPy_Task::value(arg);
    } else if (CyPy_UsageInstance::check(arg)) {
        m_value = new Task(CyPy_UsageInstance::value(arg), this->self());
    } else {
        throw Py::TypeError("Task requires a Task, or UsageInstance");
    }

    for (auto entry : kwds) {
        if (entry.first.isString()) {
            setattro(entry.first.as_string(), entry.second);
        }
    }

}

CyPy_Task::CyPy_Task(Py::PythonClassInstance* self, Ref<Task> value)
        : WrapperBase(self, std::move(value))
{

}


CyPy_Task::~CyPy_Task() = default;

void CyPy_Task::init_type()
{
    behaviors().name("Task");
    behaviors().doc("");

    behaviors().supportRichCompare();

    PYCXX_ADD_VARARGS_METHOD(irrelevant, irrelevant, "");
    PYCXX_ADD_VARARGS_METHOD(get_arg, getArg, "");
    PYCXX_ADD_VARARGS_METHOD(start_action, start_action, "Starts an action that will be tied to this task.");
    PYCXX_ADD_VARARGS_METHOD(stop_action, stop_action, "Stops an action that previously was started.");


    PYCXX_ADD_NOARGS_METHOD(obsolete, obsolete, "");

    behaviors().readyType();

    Task::argsCreator = [](const std::map<std::string, std::vector<UsageParameter::UsageArg>>& args) {
        Py::Dict dict;

        for (auto& entry : args) {
            Py::List list;

            auto visitor = compose(
                    [&](const EntityLocation& value) {
                        list.append(CyPy_EntityLocation::wrap(value));
                    },
                    [&](const WFMath::Point<3>& value) {
                        list.append(CyPy_Point3D::wrap(value));
                    },
                    [&](const WFMath::Vector<3>& value) {
                        list.append(CyPy_Vector3D::wrap(value));
                    }
            );

            for (const auto& vector_instance : entry.second) {
                boost::apply_visitor(visitor, vector_instance);
            }
            dict.setItem(entry.first, list);
        }
        return dict;
    };
}


Py::Object CyPy_Task::irrelevant(const Py::Tuple& args)
{
    if (!m_value->obsolete()) {
        m_value->irrelevant();
        if (args.size() > 0) {
            args.verify_length(1);
            Atlas::Objects::Operation::Error e;
            Atlas::Objects::Entity::Anonymous arg;
            arg->setAttr("message", verifyString(args.front()));
            e->modifyArgs().push_back(arg);
            e->setTo(m_value->m_usageInstance.actor->getId());
            return CyPy_Operation::wrap(e);
        }
    }
    return Py::None();
}

Py::Object CyPy_Task::start_action(const Py::Tuple& args) {
    args.verify_length(1);
    OpVector res;
    m_value->startAction(verifyString(args.front()), res);
    m_value->m_usageInstance.actor->sendWorld(res);

    return Py::None();
}

Py::Object CyPy_Task::stop_action(const Py::Tuple& args) {
    OpVector res;
    m_value->stopAction( res);
    m_value->m_usageInstance.actor->sendWorld(res);

    return Py::None();
}


Py::Object CyPy_Task::obsolete()
{
    return Py::Boolean(m_value->obsolete());
}

Py::Object CyPy_Task::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "usage") {
        return CyPy_UsageInstance::wrap(m_value->m_usageInstance);
    }
    if (nameStr == "actor") {
        return CyPy_LocatedEntity::wrap(m_value->m_usageInstance.actor);
    }
    if (nameStr == "tool") {
        return CyPy_LocatedEntity::wrap(m_value->m_usageInstance.tool);
    }
    if (nameStr == "definition") {
        return CyPy_Usage::wrap(m_value->m_usageInstance.definition);
    }
    if (nameStr == "op") {
        return CyPy_Operation::wrap(m_value->m_usageInstance.op);
    }


    if (nameStr == "progress") {
        return Py::Float(m_value->progress());
    }
    if (nameStr == "duration") {
        if (!m_value->m_duration) {
            return Py::None();
        }
        return Py::Float(*m_value->m_duration);
    }
    if (nameStr == "tick_interval") {
        if (!m_value->m_tick_interval) {
            return Py::None();
        }
        return Py::Float(*m_value->m_tick_interval);
    }
    if (nameStr == "name") {
        return Py::String(m_value->name());
    }
    if (nameStr == "usages") {
        Py::List list;
        for (auto& usage : m_value->usages()) {
            Py::Dict map;
            map.setItem("name", Py::String(usage.name));
            Py::Dict params;
            for (auto& param : usage.params) {
                Py::Dict paramMap;
                if (param.second.max != 1) {
                    paramMap.setItem("max", Py::Long(param.second.max));
                }
                if (param.second.min != 1) {
                    paramMap.setItem("min", Py::Long(param.second.min));
                }
                if (param.second.constraint) {
                    paramMap.setItem("constraint", CyPy_Filter::wrap(param.second.constraint));
                }
                switch (param.second.type) {
                    case UsageParameter::Type::DIRECTION:
                        paramMap.setItem("type", Py::String("direction"));
                        break;
                    case UsageParameter::Type::ENTITY:
                        paramMap.setItem("type", Py::String("entity"));
                        break;
                    case UsageParameter::Type::ENTITYLOCATION:
                        paramMap.setItem("type", Py::String("entity_location"));
                        break;
                    case UsageParameter::Type::POSITION:
                        paramMap.setItem("type", Py::String("position"));
                        break;
                }
                params.setItem(param.first, paramMap);
            }
            map.setItem("params", params);

            list.append(map);
        }
        return list;
    }
    if (nameStr == "task") {
        return PythonExtensionBase::getattro(name);
    }
    if (nameStr == "temporaries") {
        return mTemporaries;
    }
    Atlas::Message::Element val;
    if (m_value->getAttr(name, val) == 0) {
        if (val.isNone()) {
            return Py::None();
        } else {
            return CyPy_Element::wrap(val);
        }
    }

    return PythonExtensionBase::getattro(name);
}

int CyPy_Task::setattro(const Py::String& name, const Py::Object& attr)
{

    auto nameStr = name.as_string();
    if (nameStr == "progress") {
        m_value->progress() = verifyNumeric(attr);
        return 0;
    }
    if (nameStr == "duration") {
        m_value->m_duration = verifyNumeric(attr);
        return 0;
    }
    if (nameStr == "tick_interval") {
        m_value->m_tick_interval = verifyNumeric(attr);
        return 0;
    }
    if (nameStr == "name") {
        m_value->name() = verifyString(attr);
        return 0;
    }
    if (nameStr == "usages") {
        auto list = verifyList(attr);

        auto& usages = m_value->usages();
        usages.resize(0);
        usages.reserve(list.length());
        for (auto item : list) {
            auto usageMap = verifyDict(item);
            TaskUsage usage;
            usage.name = verifyString(usageMap.getItem("name"));
            if (usageMap.hasKey("params")) {
                auto params = verifyDict(usageMap.getItem("params"));
                for (auto paramEntry : params) {
                    UsageParameter param;
                    auto paramMap = verifyDict(paramEntry.second);
                    if (paramMap.hasKey("max")) {
                        param.max = verifyLong(paramMap.getItem("max"));
                    }
                    if (paramMap.hasKey("min")) {
                        param.min = verifyLong(paramMap.getItem("min"));
                    }
                    if (paramMap.hasKey("constraint")) {
                        param.constraint = CyPy_Filter::value(paramMap.getItem("constraint"));
                    }
                    if (paramMap.hasKey("type")) {
                        auto typeString = verifyString(paramMap.getItem("type"));
                        if (typeString == "direction") {
                            param.type = UsageParameter::Type::DIRECTION;
                        } else if (typeString == "entity") {
                            param.type = UsageParameter::Type::ENTITY;
                        } else if (typeString == "entity_location") {
                            param.type = UsageParameter::Type::ENTITYLOCATION;
                        } else if (typeString == "position") {
                            param.type = UsageParameter::Type::POSITION;
                        }
                    }
                    usage.params.emplace(verifyString(paramEntry.first), std::move(param));
                }
            }
            usages.emplace_back(std::move(usage));
        }
        return 0;
    }
    if (nameStr == "task") {
        PythonExtensionBase::setattro(name, attr);
        return 0;
    }

    if (nameStr == "intermediates") {
        return 0;
    }

    auto element = CyPy_Element::asElement(attr);
    m_value->setAttr(nameStr, element);
    return 0;
}

Py::Object CyPy_Task::rich_compare(const Py::Object& other, int type)
{
    if (type == Py_EQ) {
        if (CyPy_Task::check(other)) {
            return Py::Boolean(m_value == CyPy_Task::value(other));
        }
        return Py::False();
    }
    if (type == Py_NE) {
        if (CyPy_Task::check(other)) {
            return Py::Boolean(m_value != CyPy_Task::value(other));
        }
        return Py::True();
    }
    throw Py::NotImplementedError("Not implemented");
}

Py::Object CyPy_Task::getArg(const Py::Tuple& args)
{
    return CyPy_UsageInstance::getArg(m_value->m_usageInstance, args);
}



