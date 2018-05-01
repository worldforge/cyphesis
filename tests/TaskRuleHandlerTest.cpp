// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#include <Python.h>

#include "server/TaskRuleHandler.h"

#include "server/EntityBuilder.h"

#include "common/TypeNode.h"

#include <Atlas/Objects/Anonymous.h>

#include <cstdlib>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;

static TypeNode * stub_addChild_result = 0;
static bool stub_isTask_result = false;

int main()
{
    EntityBuilder::init();

    {
        RuleHandler * rh = new TaskRuleHandler(EntityBuilder::instance());
        delete rh;
    }

    // check() stub says no
    {
        RuleHandler * rh = new TaskRuleHandler(EntityBuilder::instance());

        Anonymous description;
        description->setParent("foo");
        int ret = rh->check(description);

        assert(ret == -1);

        delete rh;
    }

    // check() stub says yes
    {
        RuleHandler * rh = new TaskRuleHandler(EntityBuilder::instance());

        stub_isTask_result = true;

        Anonymous description;
        description->setParent("foo");
        int ret = rh->check(description);

        assert(ret == 0);

        stub_isTask_result = false;

        delete rh;
    }

    {
        RuleHandler * rh = new TaskRuleHandler(EntityBuilder::instance());

        Anonymous description;
        std::string dependent, reason;
        int ret = rh->install("", "", description, dependent, reason);

        assert(ret == -1);

        delete rh;
    }

    // Install a rule with addChild rigged to give a correct result
    {
        RuleHandler * rh = new TaskRuleHandler(EntityBuilder::instance());

        Anonymous description;
        std::string dependent, reason;

        stub_addChild_result = (TypeNode *) malloc(sizeof(TypeNode));
        int ret = rh->install("", "", description, dependent, reason);

        assert(ret == -1);

        free(stub_addChild_result);
        stub_addChild_result = 0;

        delete rh;
    }
    {
        RuleHandler * rh = new TaskRuleHandler(EntityBuilder::instance());

        Anonymous description;
        int ret = rh->update("", description);

        // FIXME Currently does nothing
        assert(ret == -1);

        delete rh;
    }

 
}

// stubs

#include "server/EntityFactory.h"
#include "server/TaskFactory.h"
#include "server/CorePropertyManager.h"

#include "rulesets/PythonScriptFactory.h"

#include "common/Inheritance.h"
#include "common/log.h"

TaskKit::TaskKit() : m_scriptFactory(0)
{
}

TaskKit::~TaskKit()
{
}

#include "stubs/server/stubTaskFactory.h"

template <class T>
PythonScriptFactory<T>::PythonScriptFactory(const std::string & package,
                                         const std::string & type) :
                                         PythonClass(package,
                                                     type,
                                                     &PyBaseObject_Type)
{
}

template <class T>
int PythonScriptFactory<T>::setup()
{
    return 0;
}

template <class T>
const std::string & PythonScriptFactory<T>::package() const
{
    return m_package;
}

template <class T>
int PythonScriptFactory<T>::addScript(T * entity) const
{
    return 0;
}

template class PythonScriptFactory<Task>;

template <class T>
int PythonScriptFactory<T>::refreshClass()
{
    return 0;
}

PythonClass::PythonClass(const std::string & package,
                         const std::string & type,
                         PyTypeObject * base) : m_package(package),
                                                m_type(type),
                                                m_base(base),
                                                m_module(0),
                                                m_class(0)
{
}

PythonClass::~PythonClass()
{
}

int PythonClass::load()
{
    return 0;
}

int PythonClass::getClass(struct _object *)
{
    return 0;
}

int PythonClass::refresh()
{
    return 0;
}

EntityBuilder * EntityBuilder::m_instance = nullptr;

EntityBuilder::EntityBuilder()
{
}

bool EntityBuilder::isTask(const std::string & class_name)
{
    return stub_isTask_result;
}

bool EntityBuilder::hasTask(const std::string & class_name)
{
    return true;
}

void EntityBuilder::installTaskFactory(const std::string & class_name,
                                       TaskKit *)
{
}

EntityKit * EntityBuilder::getClassFactory(const std::string & class_name)
{
    return 0;
}

void EntityBuilder::addTaskActivation(const std::string & tool,
                                      const std::string & op,
                                      TaskKit *)
{
}

TaskKit * EntityBuilder::getTaskFactory(const std::string & class_name)
{
    return 0;
}

#include "stubs/common/stubEntityKit.h"
#include "stubs/server/stubEntityFactory.h"

#ifndef STUB_Inheritance_addChild
#define STUB_Inheritance_addChild
TypeNode* Inheritance::addChild(const Atlas::Objects::Root & obj)
{
    return stub_addChild_result;
}
#endif //STUB_Inheritance_addChild


#ifndef STUB_Inheritance_hasClass
#define STUB_Inheritance_hasClass
bool Inheritance::hasClass(const std::string & parent)
{
    return true;
}
#endif //STUB_Inheritance_hasClass

#include "stubs/common/stubInheritance.h"

Root atlasOpDefinition(const std::string & name, const std::string & parent)
{
    return Root();
}

void log(LogLevel lvl, const std::string & msg)
{
}
