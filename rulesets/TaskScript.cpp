// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "rulesets/TaskScript.h"

#include "rulesets/Script.h"

#include "common/log.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootOperation.h>

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
}

void TaskScript::TickOperation(const Operation & op, OpVector & res)
{
    assert(m_script != 0);
    m_script->operation("tick", op, res);
}
