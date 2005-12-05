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

TaskScriptFactory::TaskScriptFactory() : m_scriptFactory(0)
{
}

TaskScriptFactory::~TaskScriptFactory()
{
    if (m_scriptFactory != 0) {
        delete m_scriptFactory;
    }
}

Task * TaskScriptFactory::newTask(Character & chr)
{
    // Create the task, and use its script to add a script
    return 0;
}
