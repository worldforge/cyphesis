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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "PropertyCoverage.h"
#include "TestWorld.h"

#include "rulesets/Entity.h"
#include "rulesets/Character.h"
#include "rulesets/TasksProperty.h"
#include "rulesets/Task.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>

using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Message::Element;

int main()
{
    TasksProperty * ap = new TasksProperty;

    PropertyChecker<TasksProperty> pc(ap);

    OpVector res;
    Character * chr = pc.createCharacterEntity();
    Task * task = new Task(*chr);
    task->progress() = .1;
    task->rate() = .1;
    chr->startTask(task, Atlas::Objects::Operation::Action(), res);

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

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}

namespace Atlas { namespace Objects { namespace Operation {
int UPDATE_NO = -1;
} } }

int Character::startTask(Task * task, const Operation & op, OpVector & res)
{
    return 0;
}

Task::Task(LocatedEntity & chr) : m_refCount(0), m_serialno(0), m_obsolete(false), m_progress(-1), m_rate(-1), m_owner(chr)
{
}

Task::~Task()
{
}

void Task::irrelevant()
{
}

void Task::setAttr(const std::string & attr,
                   const Atlas::Message::Element & val)
{
}

void Task::initTask(const Operation & op, OpVector & res)
{
}

void Task::operation(const Operation & op, OpVector & res)
{
}
