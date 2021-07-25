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

#include "rules/LocatedEntity.h"
#include "rules/Script.h"
#include "rules/simulation/BaseWorld.h"

#include "common/operations/Tick.h"
#include "rules/simulation/UsagesProperty.h"
#include "ScriptUtils.h"

#include <Atlas/Objects/Anonymous.h>
#include "pythonbase/Python_API.h"


#include "pycxx/CXX/Objects.hxx"
#include "ActionsProperty.h"

using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Message::MapType;

std::function<Py::Object(const std::map<std::string, std::vector<UsageParameter::UsageArg>>& args)> Task::argsCreator;

/// \brief Task constructor for classes which inherit from Task
Task::Task(UsageInstance usageInstance, Py::Object script) :
        m_serialno(0),
        m_obsolete(false),
        m_progress(-1),
        m_rate(-1),
        m_start_time(-1),
        m_script(std::move(script)),
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
    m_script = Py::None();
    if (!m_action.empty()) {
        OpVector res;
        stopAction(res);
        m_usageInstance.actor->sendWorld(res);
    }
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
    } else if (m_duration) {
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
        callScriptFunction("setup", Py::TupleN{Py::String(id)}, res);
    }

    if (obsolete()) {
        return;
    }

    res.push_back(nextTick(id, m_usageInstance.op));
}

bool Task::tick(const std::string& id, const Operation& op, OpVector& res)
{
    bool hadChange = false;
    if (m_duration) {
        auto elapsed = (op->getSeconds() - m_start_time);
        auto newProgress = std::min(1.0, elapsed / *m_duration);
        if (newProgress != m_progress) {
            m_progress = newProgress;
            hadChange = true;
        }
    }
    callScriptFunction("tick", Py::Tuple(), res);
    if (!obsolete()) {
        if (m_progress >= 1.0) {
            callScriptFunction("completed", Py::Tuple(), res);
            irrelevant();
        } else {
            res.push_back(nextTick(id, op));
        }
    }
    return hadChange;
}

void Task::callScriptFunction(const std::string& function, const Py::Tuple& args, OpVector& res)
{
    if (m_script.hasAttr(function)) {
        try {
            PythonLogGuard logGuard([this, function]() {
                return String::compose("Task '%1', entity %2, function %3: ", m_script.type().str(), m_usageInstance.actor->describeEntity(), function);
            });
            auto ret = m_script.callMemberFunction(function, args);
            //Ignore any return codes
            ScriptUtils::processScriptResult(m_script.type().str(), ret, res, *m_usageInstance.actor);
        } catch (const Py::BaseException& e) {
            log(ERROR, String::compose("Error when calling '%1' on task '%2' on entity '%3'.", function, m_script.str(), m_usageInstance.actor->describeEntity()));
            if (PyErr_Occurred() != nullptr) {
                PyErr_Print();
            }
            irrelevant();
        }
    }
}

void Task::callUsageScriptFunction(const std::string& function, const std::map<std::string, std::vector<UsageParameter::UsageArg>>& args, OpVector& res)
{
    if (m_script.hasAttr(function)) {
        Py::Object py_args = Py::None();
        if (argsCreator) {
            py_args = argsCreator(args);
        }
        try {
            PythonLogGuard logGuard([this, function]() {
                return String::compose("Task '%1', entity %2, function %3: ", m_script.type().str(), m_usageInstance.actor->describeEntity(), function);
            });
            //Make a copy if the script should be removed as part of a call to "irrelevant".
            auto script = m_script;
            auto ret = script.callMemberFunction(function, Py::TupleN(py_args));
            //Ignore any return codes
            ScriptUtils::processScriptResult(script.type().str(), ret, res, *m_usageInstance.actor);
        } catch (const Py::BaseException& e) {
            log(ERROR, String::compose("Error when calling '%1' on task '%2' on entity '%3'.", function, m_script.str(), m_usageInstance.actor->describeEntity()));
            if (PyErr_Occurred() != nullptr) {
                PyErr_Print();
            }
            irrelevant();
        }
    }
}

void Task::startAction(std::string actionName, OpVector& res)
{
    if (!m_action.empty()) {
        stopAction(res);
    }
    m_action = actionName;
    auto& actionsProp = m_usageInstance.actor->requirePropertyClassFixed<ActionsProperty>();
    actionsProp.addAction(*m_usageInstance.actor, res, actionName, {BaseWorld::instance().getTimeAsSeconds()});
}

void Task::stopAction(OpVector& res)
{
    auto& actionsProp = m_usageInstance.actor->requirePropertyClassFixed<ActionsProperty>();
    actionsProp.removeAction(*m_usageInstance.actor, res, m_action);
    m_action = "";
}


