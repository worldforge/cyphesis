// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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

// $Id: TaskScript.cpp,v 1.9 2006-10-26 00:48:12 alriddoch Exp $

#include "rulesets/TaskScript.h"

#include "rulesets/Script.h"
#include "rulesets/Character.h"

#include "common/log.h"
#include "common/Tick.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Entity::Anonymous;

TaskScript::TaskScript(Character & chr) : Task(chr), m_script(0)
{
}

TaskScript::~TaskScript()
{
}

void TaskScript::setScript(Script * scrpt)
{
    if (m_script != 0) {
        log(WARNING, "Installing a new task script over an existing script");
        delete m_script;
    }
    m_script = scrpt;
}

void TaskScript::irrelevant()
{
    Task::irrelevant();
}

void TaskScript::initTask(const Operation & op, OpVector & res)
{
    assert(m_script != 0);
    if (!m_script->operation(op->getParents().front(), op, res)) {
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

void TaskScript::TickOperation(const Operation & op, OpVector & res)
{
    assert(m_script != 0);
    m_script->operation("tick", op, res);
}
