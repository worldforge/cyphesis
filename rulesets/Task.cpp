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
#include "Script.h"

#include "common/log.h"
#include "common/Tick.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Message::MapType;

/// \brief Task constructor for classes which inherit from Task
Task::Task(Character & chr) : m_refCount(0), m_serialno(0), m_obsolete(false), m_progress(-1), m_rate(-1), m_character(chr), m_script(0)
{
}

/// \brief Task destructor
Task::~Task()
{
    delete m_script;
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

/// \brief Retrieve additional attribute values
int Task::getAttr(const std::string & attr,
                  Atlas::Message::Element & val) const
{
    MapType::const_iterator I = m_attr.find(attr);
    if (I == m_attr.end()) {
        return -1;
    }
    val = I->second;
    return 0;
}

/// \brief Set additional attributes
void Task::setAttr(const std::string & attr,
                   const Atlas::Message::Element & val)
{
    m_attr[attr] = val;
}


/// \brief Assign a script to this scripted task
///
/// @param scrpt the language script object handle this task
void Task::setScript(Script * scrpt)
{
    if (m_script != 0) {
        log(WARNING, "Installing a new task script over an existing script");
        delete m_script;
    }
    m_script = scrpt;
}

void Task::initTask(const Operation & op, OpVector & res)
{
    assert(!op->getParents().empty());
    if (m_script == 0) {
        log(WARNING, "Task script failed");
        irrelevant();
    } else if (!m_script->operation(op->getParents().front(), op, res)) {
        log(WARNING, "Task init failed");
        irrelevant();
    }

    if (obsolete()) {
        return;
    }

    Anonymous tick_arg;
    tick_arg->setName("task");
    tick_arg->setAttr("serialno", 0);
    Tick tick;
    tick->setArgs1(tick_arg);
    tick->setTo(m_character.getId());

    res.push_back(tick);
}

void Task::TickOperation(const Operation & op, OpVector & res)
{
    if (m_script != 0) {
        m_script->operation("tick", op, res);
    }
}
