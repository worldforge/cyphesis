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

// $Id$

#include "server/TaskFactory.h"

#include "rulesets/TaskScript.h"

TaskKit::TaskKit() : m_scriptFactory(0)
{
}

TaskKit::~TaskKit()
{
}

/// \brief TaskFactory constructor
///
/// @param package name of the package containing the script
/// @param name name of the type within the package for the script
TaskFactory::TaskFactory(const std::string & name) : m_name(name)
{
}

TaskFactory::~TaskFactory()
{
}

Task * TaskFactory::newTask(Character & chr)
{
    // Create the task, and use its script to add a script

    TaskScript * task = new TaskScript(chr);
    task->name() = m_name;
    assert(task != 0);

    return task;
}
