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

#include "TestBase.h"

#include "server/EntityFactory.h"

#include "rulesets/Stackable.h"
#include "rulesets/World.h"

#include "common/ScriptKit.h"
#include "common/TypeNode.h"

#include <Atlas/Objects/Entity.h>
#include <cassert>

class TestScriptFactory : public ScriptKit<LocatedEntity> {
  protected:
    std::string m_package;
  public:
    virtual const std::string & package() const {
        return m_package;
    }

    virtual int addScript(LocatedEntity * entity) const {
        return 0;
    }

    virtual int refreshClass() {
        return 0;
    }
};

class EntityFactorytest : public Cyphesis::TestBase
{
  private:
    EntityFactoryBase * m_ek;
  public:
    EntityFactorytest();

    void setup();
    void teardown();

    void test_newEntity();
    void test_destructor();
    void test_updateProperties();
    void test_updateProperties_child();
};

EntityFactorytest::EntityFactorytest()
{
    ADD_TEST(EntityFactorytest::test_newEntity);
    ADD_TEST(EntityFactorytest::test_destructor);
    ADD_TEST(EntityFactorytest::test_updateProperties);
    ADD_TEST(EntityFactorytest::test_updateProperties_child);
}

void EntityFactorytest::setup()
{
    m_ek = new EntityFactory<Thing>;
    m_ek->m_type = new TypeNode("foo");
}

void EntityFactorytest::teardown()
{
    delete m_ek->m_type;
    delete m_ek;
}

void EntityFactorytest::test_newEntity()
{
    auto e = m_ek->newEntity("1", 1, Atlas::Objects::Entity::RootEntity(), nullptr);

    ASSERT_TRUE(e);
}

void EntityFactorytest::test_destructor()
{
}

void EntityFactorytest::test_updateProperties()
{
    std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
    m_ek->updateProperties(changes);
}

void EntityFactorytest::test_updateProperties_child()
{
    EntityFactory<Thing> * ekc = new EntityFactory<Thing>;
    ekc->m_type = m_ek->m_type;
    ekc->m_classAttributes.insert(std::make_pair("foo", "value"));

    m_ek->m_children.insert(ekc);

    std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
    m_ek->updateProperties(changes);

    assert(ekc->m_attributes.find("foo") != ekc->m_attributes.end());
}

int main()
{
    EntityFactorytest t;

    return t.run();
}

// stubs

Stackable::Stackable(const std::string & id, long intId) :
           Thing(id, intId), m_num(1)
{
    // m_properties["num"] = new Property<int>(m_num, 0);
}

Stackable::~Stackable()
{
}

void Stackable::CombineOperation(const Operation & op, OpVector & res)
{
}

void Stackable::DivideOperation(const Operation & op, OpVector & res)
{
}
#include "stubs/rulesets/stubPlant.h"


#include "stubs/rulesets/stubThing.h"
#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/common/stubRouter.h"
#include "stubs/common/stubTypeNode.h"
#include "stubs/common/stubProperty.h"
#include "common/Property_impl.h"
#include "stubs/rulesets/stubLocation.h"

void log(LogLevel lvl, const std::string & msg)
{
}
