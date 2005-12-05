// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "rulesets/TaskScript.h"

TaskScript::TaskScript(Character & chr) : Task(chr)
{
}

TaskScript::~TaskScript()
{
}

void TaskScript::setup(OpVector & res)
{
}

void TaskScript::irrelevant()
{
}

void TaskScript::TickOperation(const Operation & op, OpVector & res)
{
}
