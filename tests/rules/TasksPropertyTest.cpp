// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../PropertyCoverage.h"
#include "../TestWorld.h"

#include "rules/simulation/Entity.h"
#include "rules/simulation/TasksProperty.h"
#include "rules/simulation/Task.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include "pythonbase/PythonMalloc.h"
#include "pythonbase/Python_API.h"

using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Message::Element;

int main()
{
    setupPythonMalloc();
    init_python_api({});

    TasksProperty ap;

    PropertyChecker<TasksProperty> pc(ap);

    Py::Dict fake;
    OpVector res;
    Ref<Entity> actor = pc.createCharacterEntity();
    UsageInstance usageInstance;
    usageInstance.actor = actor;
    usageInstance.op = Atlas::Objects::Operation::Action();
    Ref<Task> task = new Task(usageInstance, fake);
    task->progress() = .1;
    task->rate() = .1;
    actor->requirePropertyClassFixed<TasksProperty>().startTask("", task, *actor, res);

    MapType map;
    map["one"] = 23;
    map["two"] = 23.;
    map["three"] = "twenty_three";
    map["four"] = ListType(1, 23);
    map["five"] = ListType(1, 23.);
    map["six"] = ListType(1, "twenty_three");
    pc.testDataAppend(ListType(1, map));

    pc.basicCoverage();

    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}

// stubs

#define STUB_Task_initTask
void Task::initTask(const std::string& id, OpVector & res)
{
    this->m_obsolete = true;
}

#include "../stubs/rules/simulation/stubTask.h"
#include "../stubs/rules/entityfilter/stubFilter.h"
#include "../stubs/rules/simulation/stubUsageInstance.h"
#include "../stubs/common/stubInheritance.h"

#include "../stubs/common/stubcustom.h"
#include "../stubs/common/stubTypeNode.h"

