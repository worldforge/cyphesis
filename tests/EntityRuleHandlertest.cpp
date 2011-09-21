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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/EntityRuleHandler.h"

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
    EntityBuilder::init(*(BaseWorld*)0);

    {
        RuleHandler * rh = new EntityRuleHandler(EntityBuilder::instance());
        delete rh;
    }

    // check() stub says no
    {
        RuleHandler * rh = new EntityRuleHandler(EntityBuilder::instance());

        Anonymous description;
        int ret = rh->check(description);

        assert(ret == -1);

        delete rh;
    }

    // check() stub says yes
    {
        RuleHandler * rh = new EntityRuleHandler(EntityBuilder::instance());

        stub_isTask_result = true;

        Anonymous description;
        int ret = rh->check(description);

        assert(ret == 0);

        stub_isTask_result = false;

        delete rh;
    }

    {
        RuleHandler * rh = new EntityRuleHandler(EntityBuilder::instance());

        Anonymous description;
        std::string dependent, reason;
        int ret = rh->install("", "", description, dependent, reason);

        assert(ret == -1);

        delete rh;
    }

    // Install a rule with addChild rigged to give a correct result
    {
        RuleHandler * rh = new EntityRuleHandler(EntityBuilder::instance());

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
        RuleHandler * rh = new EntityRuleHandler(EntityBuilder::instance());

        Anonymous description;
        int ret = rh->update("", description);

        // FIXME Currently does nothing
        assert(ret == -1);

        delete rh;
    }

 
}

// stubs

#include "server/EntityFactory.h"

#include "common/Inheritance.h"
#include "common/log.h"

EntityBuilder * EntityBuilder::m_instance = NULL;

EntityBuilder::EntityBuilder(BaseWorld & w) : m_world(w)
{
}

bool EntityBuilder::isTask(const std::string & class_name)
{
    return stub_isTask_result;
}

EntityKit * EntityBuilder::getClassFactory(const std::string & class_name)
{
    return 0;
}

void EntityKit::updateChildren()
{
}

void EntityKit::updateChildrenProperties()
{
}

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance()
{
}

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
    }
    return *m_instance;
}

TypeNode * Inheritance::addChild(const Root & obj)
{
    return stub_addChild_result;
}

bool Inheritance::hasClass(const std::string & parent)
{
    return true;
}

Root atlasOpDefinition(const std::string & name, const std::string & parent)
{
    return Root();
}

void log(LogLevel lvl, const std::string & msg)
{
}
