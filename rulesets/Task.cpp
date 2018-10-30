#include <utility>

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004-2005 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "Task.h"

#include "LocatedEntity.h"
#include "Script.h"

#include "common/operations/Tick.h"
#include "UsagesProperty.h"
#include "ScriptUtils.h"

#include <Atlas/Objects/Anonymous.h>
#include <external/pycxx/CXX/Objects.hxx>

using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Message::MapType;

/// \brief Task constructor for classes which inherit from Task
Task::Task(UsageInstance usageInstance, const Py::Object& script) :
    m_refCount(0),
    m_serialno(0),
    m_obsolete(false),
    m_progress(-1),
    m_rate(-1),
    m_script(script),
    m_tick_interval(1.0),
    m_usageInstance(std::move(usageInstance))
{
}

/// \brief Task destructor
Task::~Task() = default;

/// \brief Set the obsolete flag indicating that this task is obsolete
///
/// The task should be checked to see if it is irrelevant before any
/// processing is passed to it. A typical example is the Task may
/// contain references to entities which are deleted. irrelevant() could
/// be connected to the destroyed signal of the entities, so that it
/// never de-references the pointers to deleted entities.
void Task::irrelevant()
{
    m_obsolete = true;
}

Operation Task::nextTick(const std::string& id, const Operation& op)
{
    Anonymous tick_arg;
    tick_arg->setId(id);
    tick_arg->setName("task");
    tick_arg->setAttr("serialno", newTick());
    Tick tick;
    tick->setArgs1(tick_arg);
    tick->setTo(m_usageInstance.actor->getId());
    //Default to once per second.
    double futureSeconds = 1.0;
    if (m_tick_interval) {
        futureSeconds = *m_tick_interval;
    } else if(m_duration) {
        futureSeconds = *m_duration;
    }

    //If there's a duration, adjust the tick interval so it matches the duration end
    if (m_duration) {
        futureSeconds = std::min(futureSeconds, *m_duration - (op->getSeconds() - m_start_time));
    }
    tick->setFutureSeconds(futureSeconds);

    return tick;
}

/// \brief Retrieve additional attribute values
int Task::getAttr(const std::string& attr,
                  Atlas::Message::Element& val) const
{
    auto I = m_attr.find(attr);
    if (I == m_attr.end()) {
        return -1;
    }
    val = I->second;
    return 0;
}

/// \brief Set additional attributes
void Task::setAttr(const std::string& attr,
                   const Atlas::Message::Element& val)
{
    m_attr[attr] = val;
}

void Task::initTask(const std::string& id, OpVector& res)
{
    m_start_time = m_usageInstance.op->getSeconds();
    if (m_script.isNull()) {
        log(WARNING, "Task script failed");
        irrelevant();
    } else {
        callScriptFunction("setup", res);
    }

    if (obsolete()) {
        return;
    }

    res.push_back(nextTick(id, m_usageInstance.op));
}

void Task::tick(const std::string& id, const Operation& op, OpVector& res)
{
    if (m_duration) {
        auto elapsed = (op->getSeconds() - m_start_time);
        m_progress = std::min(1.0,  elapsed / *m_duration);
    }
    callScriptFunction("tick", res);
    if (!obsolete()) {
        if (m_progress >= 1.0) {
            irrelevant();
            callScriptFunction("completed", res);
        } else {
            res.push_back(nextTick(id, op));
        }
    }
}

void Task::callScriptFunction(const std::string& function, OpVector& res)
{
    if (m_script.hasAttr(function)) {
        try {
            auto ret = m_script.callMemberFunction(function);
            //Ignore any return codes
            ScriptUtils::processScriptResult(m_script.str(), ret, res, m_usageInstance.actor.get());
        } catch (const Py::BaseException& e) {
            log(ERROR, String::compose("Error when calling '%1' on task '%2' on entity '%3'.", function, m_script.str(), m_usageInstance.actor->describeEntity()));
            if (PyErr_Occurred() != nullptr) {
                PyErr_Print();
            }
            irrelevant();
        }
    }
}

