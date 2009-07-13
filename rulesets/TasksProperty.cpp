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

#include "Character.h"
#include "Task.h"

#include "common/compose.hpp"
#include "common/debug.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <iostream>

using Atlas::Message::ListType;
using Atlas::Message::MapType;

TasksProperty::TasksProperty() : PropertyBase(per_ephem), m_task(0)
{
}

bool TasksProperty::get(Atlas::Message::Element & val) const
{
    if (m_task == 0 || *m_task == 0) {
        log(ERROR, "No task");
        return false;
    }
    log(NOTICE, "Task property setting.");
    MapType task;
    task["name"] = (*m_task)->name();
    float progress = (*m_task)->progress();
    if (progress > 0) {
        task["progress"] = progress;
    }
    float rate = (*m_task)->rate();
    if (rate > 0) {
        task["rate"] = rate;
    }
    val = ListType(1, task);
    return true;
}

void TasksProperty::set(const Atlas::Message::Element & val)
{
    log(NOTICE, "Task property got set");

    if (!val.isList())
    {
        log(ERROR, "Task property must be a list.");
        return;
    }

    if (m_task == 0 || *m_task == 0) {
        log(ERROR, "No task");
        return;
    }

    debug_dump(val.asList());

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
            (*m_task)->setAttr(J->first, J->second);
        }
    }
}

void TasksProperty::install(Entity * owner)
{
    if (flags() & flag_class) {
        return;
    }
    Character * c = dynamic_cast<Character *>(owner);
    if (c != 0) {
        m_task = c->monitorTask();
        log(NOTICE, String::compose("Bound task property to %1", c->getId()));
    }
}

void TasksProperty::apply(Entity * owner)
{
}
