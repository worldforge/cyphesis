// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#include <common/Tick.h>
#include "TasksProperty.h"

#include "LocatedEntity.h"
#include "Task.h"

#include "common/debug.h"
#include "common/TypeNode.h"
#include "common/Update.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Update;

static const bool debug_flag = false;

static const std::string SERIALNO = "serialno";

TasksProperty::TasksProperty()
    : PropertyBase(per_ephem)
{
}

int TasksProperty::get(Atlas::Message::Element& val) const
{
    MapType tasks;
    for (auto entry : m_tasks) {
        auto& task = entry.second;
        MapType taskMap;
        taskMap["name"] = task->name();
        auto progress = task->progress();
        if (progress > 0) {
            taskMap["progress"] = progress;
        }
        if (task->m_duration) {
            taskMap["rate"] = 1.0f / *task->m_duration;
        }
        tasks.emplace(entry.first, std::move(taskMap));
    }

    val = std::move(tasks);
    return 0;
}

void TasksProperty::set(const Atlas::Message::Element& val)
{
    log(ERROR, "Cannot set 'tasks' property.");
}

TasksProperty* TasksProperty::copy() const
{
    return new TasksProperty(*this);
}

int TasksProperty::updateTask(LocatedEntity* owner, OpVector& res)
{
    m_flags.addFlags(flag_unsent);

    Update update;
    update->setTo(owner->getId());

    res.push_back(update);

    return 0;
}

int TasksProperty::startTask(std::string id, Ref<Task> task,
                             LocatedEntity* owner,
                             OpVector& res)
{
    bool update_required = false;

    auto tasksI = m_tasks.find(id);

    if (tasksI != m_tasks.end()) {
        update_required = true;
        m_tasks.erase(id);
    }

    task->initTask(id, res);

    if (!task->obsolete()) {
        assert(!res.empty());
        m_tasks.emplace(id, task);
        update_required = true;
    } else {
        task = nullptr;
    }

    if (update_required) {
        updateTask(owner, res);
    }

    return task ? 0 : -1;

}

int TasksProperty::clearTask(const std::string& id, LocatedEntity* owner, OpVector& res)
{
    if (m_tasks.empty()) {
        // This function should never be called when there is no task,
        // except during Entity destruction
        assert(owner->hasFlags(entity_destroyed));
        return -1;
    }
    m_tasks.erase(id);

    return updateTask(owner, res);
}

void TasksProperty::stopTask(const std::string& id, LocatedEntity* owner, OpVector& res)
{
    // This is just clearTask without an assert
    if (m_tasks.find(id) == m_tasks.end()) {
        log(ERROR, "Tasks property stop when no task");
        return;
    }

    m_tasks.erase(id);

    updateTask(owner, res);
}

void TasksProperty::TickOperation(LocatedEntity* owner,
                                  const Operation& op,
                                  OpVector& res)
{

    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        return;
    }

    const Root& arg = args.front();

    if (arg->isDefaultId()) {
        return;
    }

    auto taskI = m_tasks.find(arg->getId());
    if (taskI == m_tasks.end()) {
        return;
    }

    auto& id = taskI->first;
    auto& task = taskI->second;

    Element serialno;
    if (arg->copyAttr(SERIALNO, serialno) == 0 && (serialno.isInt())) {
        if (serialno.asInt() != task->serialno()) {
            debug_print("Old tick");
            return;
        }
    } else {
        log(ERROR, "Character::TickOperation: No serialno in tick arg");
        return;
    }
    task->tick(id, op, res);
    if (task->obsolete()) {
        clearTask(id, owner, res);
    } else {
        updateTask(owner, res);
    }

    if (task != nullptr && res.empty()) {
        log(WARNING, String::compose("Character::%1: Task %2 has "
                                     "stalled", __func__,
                                     task->name()));
    }
}

void TasksProperty::UseOperation(LocatedEntity* owner,
                                 const Operation& op,
                                 OpVector& res)
{
}

HandlerResult TasksProperty::operation(LocatedEntity* owner,
                                       const Operation& op,
                                       OpVector& res)
{
    auto& args = op->getArgs();
    if (!args.empty()) {
        auto& arg = args.front();
        if (arg->getName() == "task") {
            TickOperation(owner, op, res);
            return OPERATION_BLOCKED;
        }
    }

    return OPERATION_IGNORED;
}

void TasksProperty::install(LocatedEntity* owner, const std::string& name)
{
    owner->installDelegate(Atlas::Objects::Operation::TICK_NO, name);
}

void TasksProperty::remove(LocatedEntity* owner, const std::string& name)
{
    owner->removeDelegate(Atlas::Objects::Operation::TICK_NO, name);
}
