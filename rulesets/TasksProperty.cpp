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

// $Id$

#include "TasksProperty.h"

#include "LocatedEntity.h"
#include "Task.h"

#include "common/compose.hpp"
#include "common/debug.h"
#include "common/log.h"
#include "common/TypeNode.h"
#include "common/Update.h"

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Update;

static const bool debug_flag = false;

static const std::string SERIALNO = "serialno";

TasksProperty::TasksProperty() : PropertyBase(per_ephem), m_task(0)
{
}

int TasksProperty::get(Atlas::Message::Element & val) const
{
    if (m_task == 0) {
        val = ListType();
        return 0;
    }
    MapType task;
    task["name"] = m_task->name();
    float progress = m_task->progress();
    if (progress > 0) {
        task["progress"] = progress;
    }
    float rate = m_task->rate();
    if (rate > 0) {
        task["rate"] = rate;
    }
    val = ListType(1, task);
    return 0;
}

void TasksProperty::set(const Atlas::Message::Element & val)
{
    if (!val.isList())
    {
        log(ERROR, "Task property must be a list.");
        return;
    }

    if (m_task == 0) {
        log(ERROR, "No task in ::set");
        return;
    }

    ListType tasks = val.asList();
    ListType::const_iterator I = tasks.begin();
    ListType::const_iterator Iend = tasks.end();
    for (; I != Iend; ++I) {
        if (!I->isMap()) {
            log(ERROR, "Task must be a map.");
            return;
        }
        const MapType & task = I->asMap();
        MapType::const_iterator J = task.begin();
        MapType::const_iterator Jend = task.end();
        for (J = task.begin(); J != Jend; ++J) {
            m_task->setAttr(J->first, J->second);
        }
    }
}

TasksProperty * TasksProperty::copy() const
{
    return new TasksProperty(*this);
}

int TasksProperty::updateTask(LocatedEntity * owner, OpVector & res)
{
    setFlags(flag_unsent);

    Update update;
    update->setTo(owner->getId());

    res.push_back(update);

    return 0;
}

int TasksProperty::startTask(Task * task,
                             LocatedEntity * owner,
                             const Operation & op,
                             OpVector & res)
{
    bool update_required = false;
    if (m_task != 0) {
        update_required = true;
        m_task->decRef();
        m_task = 0;
    }

    task->initTask(op, res);

    assert(task->count() == 0);

    if (task->obsolete()) {
        // Thus far a task can not legally have a reference, so we can't
        // decref it.
        delete task;
    } else {
        assert(!res.empty());
        m_task = task;
        m_task->incRef();
        update_required = true;
    }

    if (update_required) {
        updateTask(owner, res);
    }

    return (m_task == 0) ? -1 : 0;

}

int TasksProperty::clearTask(LocatedEntity * owner, OpVector & res)
{
    if (m_task == 0) {
        // This function should never be called when there is no task,
        // except during Entity destruction
        assert(owner->getFlags() & entity_destroyed);
        return -1;
    }
    // Thus far a task can only have one reference legally, so if we
    // have a task it's count must be 1
    assert(m_task->count() == 1);
    m_task->decRef();
    m_task = 0;

    return updateTask(owner, res);
}

void TasksProperty::stopTask(LocatedEntity * owner, OpVector & res)
{
    // This is just clearTask without an assert
    if (m_task == 0) {
        log(ERROR, "Tasks property stop when no task");
        return;
    }

    assert(m_task->count() == 1);
    m_task->decRef();
    m_task = 0;

    updateTask(owner, res);
}

void TasksProperty::TickOperation(LocatedEntity * owner,
                                  const Operation & op,
                                  OpVector & res)
{
    if (m_task == 0) {
        return;
    }

    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }

    const Root & arg = args.front();

    Element serialno;
    if (arg->copyAttr(SERIALNO, serialno) == 0 && (serialno.isInt())) {
        if (serialno.asInt() != m_task->serialno()) {
            debug(std::cout << "Old tick" << std::endl << std::flush;);
            return;
        }
    } else {
        log(ERROR, "Character::TickOperation: No serialno in tick arg");
        return;
    }
    operation(owner, op, res);
    if (m_task != 0 && res.empty()) {
        log(WARNING, String::compose("Character::%1: Task %2 has "
                                     "stalled", __func__,
                                     m_task->name()));
    }
}

void TasksProperty::UseOperation(LocatedEntity * owner,
                                 const Operation & op,
                                 OpVector & res)
{
}

HandlerResult TasksProperty::operation(LocatedEntity * owner,
                                       const Operation & op,
                                       OpVector & res)
{
    m_task->operation(op, res);
    if (m_task->obsolete()) {
        clearTask(owner, res);
    } else {
        updateTask(owner, res);
    }
    return OPERATION_HANDLED;
}
