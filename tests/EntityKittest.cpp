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

#include "TestBase.h"

#include "common/EntityKit.h"

#include "common/ScriptKit.h"
#include "common/TypeNode.h"

#include <Atlas/Message/Element.h>

#include <cassert>

using Atlas::Message::MapType;

class TestEntityKit : public EntityKit
{
  public:
    virtual ~TestEntityKit() { }

    virtual LocatedEntity * newEntity(const std::string & id, long intId) { return 0; }

    virtual EntityKit * duplicateFactory() { return 0; }
};

class TestScriptKit : public ScriptKit<LocatedEntity>
{
  public:
    std::string m_package;
    virtual const std::string & package() const { return m_package; }
    virtual int addScript(LocatedEntity * entity) const { return 0; }
    virtual int refreshClass() { return 0; }
};

class EntityKittest : public Cyphesis::TestBase
{
  private:
    EntityKit * m_ek;
  public:
    EntityKittest();

    void setup();
    void teardown();

    void test_destructor();
    void test_addProperties();
    void test_updateProperties();
    void test_updateProperties_child();
};

EntityKittest::EntityKittest()
{
    ADD_TEST(EntityKittest::test_destructor);
    ADD_TEST(EntityKittest::test_addProperties);
    ADD_TEST(EntityKittest::test_updateProperties);
    ADD_TEST(EntityKittest::test_updateProperties_child);
}

void EntityKittest::setup()
{
    m_ek = new TestEntityKit;
    m_ek->m_type = new TypeNode("foo");
}

void EntityKittest::teardown()
{
    delete m_ek->m_type;
    delete m_ek;
}

void EntityKittest::test_destructor()
{
    m_ek->m_scriptFactory = new TestScriptKit;
}

void EntityKittest::test_addProperties()
{
    m_ek->addProperties();
}

void EntityKittest::test_updateProperties()
{
    m_ek->updateProperties();
}

void EntityKittest::test_updateProperties_child()
{
    EntityKit * ekc = new TestEntityKit;
    ekc->m_type = m_ek->m_type;
    ekc->m_classAttributes.insert(std::make_pair("foo", "value"));

    m_ek->m_children.insert(ekc);

    m_ek->updateProperties();

    assert(ekc->m_attributes.find("foo") != ekc->m_attributes.end());
}

int main()
{
    EntityKittest t;

    return t.run();
}

// stubs

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

TypeNode::~TypeNode()
{
}

void TypeNode::addProperties(const MapType & attributes)
{
}

void TypeNode::updateProperties(const MapType & attributes)
{
}
