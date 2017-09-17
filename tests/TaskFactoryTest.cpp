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

#include "server/TaskFactory.h"

#include "rulesets/Entity.h"
#include "rulesets/Task.h"

#include "common/TypeNode.h"

#include <cassert>

static bool stub_TypeNode_isTypeOf = false;
static bool stub_LocatedEntity_hasAttr = false;

int main()
{
    {
        TaskKit * ek = new TaskFactory("test1");

        assert(ek->m_scriptFactory == 0);
        assert(ek->target() == 0);

        delete ek;
    }

    Entity * chr = new Entity("1", 1);
    Entity * target = new Entity("2", 2);

    // Check target will always succeed if no constraints are in place
    {
        TaskKit * ek = new TaskFactory("test1");

        assert(ek->m_scriptFactory == 0);
        assert(ek->target() == 0);

        int c = ek->checkTarget(target);
        assert(c == 0);
    }

    // Type match fails, no property set
    {
        TaskKit * ek = new TaskFactory("test1");

        stub_TypeNode_isTypeOf = false;

        ek->setTarget(new TypeNode("type1"));
        target->setType(new TypeNode("type2"));

        int c = ek->checkTarget(target);
        assert(c == -1);
    }

    // Type match succeeds, no property set
    {
        TaskKit * ek = new TaskFactory("test1");

        stub_TypeNode_isTypeOf = true;

        ek->setTarget(new TypeNode("type1"));
        target->setType(new TypeNode("type2"));

        int c = ek->checkTarget(target);
        assert(c == 0);
    }

    // property match fails, no type match
    {
        TaskKit * ek = new TaskFactory("test1");

        stub_LocatedEntity_hasAttr = false;

        ek->setRequireProperty("prop1");

        int c = ek->checkTarget(target);
        assert(c == -1);
    }

    // property match succeeds, no type match
    {
        TaskKit * ek = new TaskFactory("test1");

        stub_LocatedEntity_hasAttr = true;

        ek->setRequireProperty("prop1");

        int c = ek->checkTarget(target);
        assert(c == 0);
    }

    // property match fails, type match fails
    {
        TaskKit * ek = new TaskFactory("test1");

        stub_LocatedEntity_hasAttr = false;
        stub_TypeNode_isTypeOf = false;

        ek->setTarget(new TypeNode("type1"));
        target->setType(new TypeNode("type2"));

        ek->setRequireProperty("prop1");

        int c = ek->checkTarget(target);
        assert(c == -1);
    }

    // property match succeeds, type match fails
    {
        TaskKit * ek = new TaskFactory("test1");

        stub_LocatedEntity_hasAttr = true;
        stub_TypeNode_isTypeOf = false;

        ek->setTarget(new TypeNode("type1"));
        target->setType(new TypeNode("type2"));

        ek->setRequireProperty("prop1");

        int c = ek->checkTarget(target);
        assert(c == -1);
    }

    // property match fails, type match succeeds
    {
        TaskKit * ek = new TaskFactory("test1");

        stub_LocatedEntity_hasAttr = false;
        stub_TypeNode_isTypeOf = true;

        ek->setTarget(new TypeNode("type1"));
        target->setType(new TypeNode("type2"));

        ek->setRequireProperty("prop1");

        int c = ek->checkTarget(target);
        assert(c == -1);
    }

    // property match fails, type match fails
    {
        TaskKit * ek = new TaskFactory("test1");

        stub_LocatedEntity_hasAttr = true;
        stub_TypeNode_isTypeOf = true;

        ek->setTarget(new TypeNode("type1"));
        target->setType(new TypeNode("type2"));

        ek->setRequireProperty("prop1");

        int c = ek->checkTarget(target);
        assert(c == 0);
    }

    {
        TaskKit * ek = new TaskFactory("test1");

        Task * e = ek->newTask(*chr);
        assert(e);

        delete ek;
    }

    return 0;
}

// stubs

TaskKit::TaskKit() : m_target(0), m_scriptFactory(0)
{
}

TaskKit::~TaskKit()
{
}

void Task::initTask(const Operation & op, OpVector & res)
{
}

void Task::operation(const Operation & op, OpVector & res)
{
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
#include "stubs/rulesets/stubEntity.h"

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return stub_LocatedEntity_hasAttr;
}

int LocatedEntity::getAttr(const std::string & name,
                           Atlas::Message::Element & attr) const
{
    return -1;
}

int LocatedEntity::getAttrType(const std::string & name,
                               Atlas::Message::Element & attr,
                               int type) const
{
    return -1;
}

PropertyBase * LocatedEntity::setAttr(const std::string & name,
                                      const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * LocatedEntity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * LocatedEntity::setProperty(const std::string & name,
                                          PropertyBase * prop)
{
    return 0;
}

void LocatedEntity::installDelegate(int, const std::string &)
{
}

void LocatedEntity::removeDelegate(int class_no, const std::string & delegate)
{
}

void LocatedEntity::destroy()
{
}

Domain * LocatedEntity::getDomain()
{
    return 0;
}

const Domain * LocatedEntity::getDomain() const
{
    return 0;
}

void LocatedEntity::sendWorld(const Operation & op)
{
}

void LocatedEntity::onContainered(const LocatedEntity*)
{
}

void LocatedEntity::onUpdated()
{
}

void LocatedEntity::addChild(LocatedEntity& childEntity)
{
}

void LocatedEntity::removeChild(LocatedEntity& childEntity)
{
}

void LocatedEntity::setType(const TypeNode* t)
{

}

std::vector<Atlas::Objects::Root> LocatedEntity::getThoughts() const
{
    return std::vector<Atlas::Objects::Root>();
}

#include "stubs/common/stubRouter.h"

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

bool TypeNode::isTypeOf(const TypeNode * base_type) const
{
    return stub_TypeNode_isTypeOf;
}

#include "stubs/modules/stubLocation.h"

