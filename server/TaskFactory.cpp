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


#include "server/TaskFactory.h"

#include "rulesets/Task.h"
#include "rulesets/LocatedEntity.h"

#include "common/debug.h"
#include "common/ScriptKit.h"
#include "common/TypeNode.h"

static const bool debug_flag = false;

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

int TaskFactory::checkTarget(LocatedEntity * target)
{
    if (m_target != 0 && !target->getType()->isTypeOf(m_target)) {
        debug( std::cout << target->getType()->name() << " is not a "
                         << m_target->name()
                         << std::endl << std::flush; );
        return -1;
    }
    if (!m_property.empty() && !target->hasAttr(m_property)) {
        debug( std::cout << target->getType()->name() << " has not a "
                         << m_property
                         << std::endl << std::flush; );
        return -1;
    }
    return 0;
}

Task * TaskFactory::newTask(LocatedEntity & chr)
{
    // Create the task, and use its script to add a script

    Task * task = new Task(chr);
    task->name() = m_name;
    assert(task != 0);

    return task;
}
