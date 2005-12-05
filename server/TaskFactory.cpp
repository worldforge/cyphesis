// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "server/TaskFactory.h"

#include "server/ScriptFactory.h"

TaskFactory::TaskFactory()
{
}

TaskFactory::~TaskFactory()
{
}

PythonTaskScriptFactory::PythonTaskScriptFactory() : m_module(0), m_class(0)
{
}

PythonTaskScriptFactory::~PythonTaskScriptFactory()
{
    if (m_module != 0) {
    }
    if (m_class != 0) {
    }
}

Task * PythonTaskScriptFactory::newTask(Character & chr)
{
    // Create the task, and use its script to add a script
    return 0;
}
