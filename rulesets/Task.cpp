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

// $Id$

#include "Task.h"

#include "Character.h"

#include "common/Tick.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Message::MapType;

/// \brief Task constructor for classes which inherit from Task
Task::Task(Character & chr) : m_refCount(0), m_serialno(0), m_obsolete(false), m_progress(-1), m_rate(-1), m_character(chr)
{
}

/// \brief Task destructor
Task::~Task()
{
}

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

void Task::addToEntity(const Atlas::Objects::Entity::RootEntity & ent)
{
    Atlas::Message::MapType task;
    task["name"] = m_name;
    if (m_progress >= 0) {
        task["progress"] = m_progress;
    }
    if (m_rate >= 0) {
        task["progress_rate"] = m_rate;
    }
    MapType::const_iterator J = m_attr.begin();
    MapType::const_iterator Jend = m_attr.end();
    for (; J != Jend; ++J) {
        task[J->first] = J->second;
    }
    ent->setAttr("tasks", Atlas::Message::ListType(1, task));
}

Operation Task::nextTick(double interval)
{
    Anonymous tick_arg;
    tick_arg->setName("task");
    tick_arg->setAttr("serialno", newTick());
    Tick tick;
    tick->setArgs1(tick_arg);
    tick->setTo(m_character.getId());
    tick->setFutureSeconds(interval);

    return tick;
}
