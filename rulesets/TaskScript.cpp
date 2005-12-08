// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "rulesets/TaskScript.h"

#include "rulesets/Script.h"

#include "common/log.h"

#include "common/Setup.h"

using Atlas::Objects::Operation::Setup;

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

void TaskScript::setup(OpVector & res)
{
    assert(m_script != 0);
    Setup op;
    m_script->operation("setup", op, res);
}

void TaskScript::irrelevant()
{
    Task::irrelevant();
}

void TaskScript::TickOperation(const Operation & op, OpVector & res)
{
    assert(m_script != 0);
    m_script->operation("tick", op, res);
}
