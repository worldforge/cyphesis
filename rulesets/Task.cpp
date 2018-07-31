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

#include "common/Tick.h"
#include "UsagesProperty.h"

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

Operation Task::nextTick(double interval)
{
    Anonymous tick_arg;
    tick_arg->setName("task");
    tick_arg->setAttr("serialno", newTick());
    Tick tick;
    tick->setArgs1(tick_arg);
    tick->setTo(m_usageInstance.actor->getId());
    tick->setFutureSeconds(interval);

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

void Task::initTask(OpVector& res)
{
    if (m_script.isNull()) {
        log(WARNING, "Task script failed");
        irrelevant();
    } else {
        if (m_script.hasAttr("setup")) {
            try {
                auto ret = m_script.callMemberFunction("setup");
                //Ignore any return codes
                UsagesProperty::processScriptResult(m_script.str(), ret, res, m_usageInstance.actor);
            } catch (const Py::BaseException& e) {
                log(ERROR, String::compose("Error when setting up task '%1' on entity '%2'.", m_script.str(), m_usageInstance.actor->describeEntity()));
                if (PyErr_Occurred() != nullptr) {
                    PyErr_Print();
                }
                irrelevant();
            }
        }
    }

    if (obsolete()) {
        return;
    }

    res.push_back(nextTick(m_tick_interval));
}

void Task::tick(OpVector& res)
{
    if (m_script.hasAttr("tick")) {
        try {
            auto ret = m_script.callMemberFunction("tick");
            //Ignore any return codes
            UsagesProperty::processScriptResult(m_script.str(), ret, res, m_usageInstance.actor);
        } catch (const Py::BaseException& e) {
            log(ERROR, String::compose("Error when calling 'tick' on task '%1' on entity '%2'.", m_script.str(), m_usageInstance.actor->describeEntity()));
            if (PyErr_Occurred() != nullptr) {
                PyErr_Print();
            }
            irrelevant();
        }
    }

    if (!obsolete()) {
        res.push_back(nextTick(m_tick_interval));
    }
}
