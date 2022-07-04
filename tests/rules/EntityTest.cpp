// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

#include "../TestBase.h"
#include "../IGEntityExerciser.h"
#include "../allOperations.h"

#include "rules/simulation/Entity.h"

#include "rules/AtlasProperties.h"
#include "rules/Domain.h"
#include "rules/Script.h"
#include "rules/simulation/DomainProperty.h"

#include "common/id.h"
#include "common/Property_impl.h"
#include "common/PropertyFactory_impl.h"
#include "common/TypeNode.h"

#include <cstdlib>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Message::ListType;

class Entitytest : public Cyphesis::TestBase
{
  private:
    TestPropertyManager * m_pm;
    TypeNode * m_type;
    Ref<Entity> m_entity;

    static bool m_TestProperty_install_called;
    static bool m_TestProperty_apply_called;
  public:
    Entitytest();

    void setup();
    void teardown();

    void test_setAttr_new();
    void test_setAttr_existing();
    void test_setAttr_type();
    void test_sequence();

    class TestProperty : public Property<int>
    {
      public:
        virtual void install(LocatedEntity&, const std::string &);
        void apply(LocatedEntity&) override;
        virtual TestProperty * copy() const;
    };

    static void TestProperty_install_called()
    {
        m_TestProperty_install_called = true;
    }

    static void TestProperty_apply_called()
    {
        m_TestProperty_apply_called = true;
    }
};

bool Entitytest::m_TestProperty_install_called;
bool Entitytest::m_TestProperty_apply_called;

void Entitytest::TestProperty::install(LocatedEntity&, const std::string & name)
{
    Entitytest::TestProperty_install_called();
}

void Entitytest::TestProperty::apply(LocatedEntity&)
{
    Entitytest::TestProperty_apply_called();
}

Entitytest::TestProperty * Entitytest::TestProperty::copy() const
{
    return new Entitytest::TestProperty(*this);
}

Entitytest::Entitytest()
{
    ADD_TEST(Entitytest::test_setAttr_new);
    ADD_TEST(Entitytest::test_setAttr_existing);
    ADD_TEST(Entitytest::test_setAttr_type);
    ADD_TEST(Entitytest::test_sequence);
}

void Entitytest::setup()
{
    m_pm = new TestPropertyManager;
    m_pm->installPropertyFactory("test_int_property", std::make_unique<PropertyFactory<TestProperty>>());
    m_type = new TypeNode("test_type");
    m_entity = new Entity(1);
    m_entity->setType(m_type);

    m_TestProperty_install_called = false;
}

void Entitytest::teardown()
{
    m_entity = nullptr;
    delete m_type;
    delete m_pm;
}

void Entitytest::test_setAttr_new()
{
    ASSERT_TRUE(!m_TestProperty_install_called);

    PropertyBase * pb = m_entity->setAttrValue("test_int_property", 24);
    ASSERT_NOT_NULL(pb);

    ASSERT_TRUE((pb->flags().m_flags & prop_flag_class) == 0);

    auto * int_property = dynamic_cast<TestProperty *>(pb);
    ASSERT_NOT_NULL(int_property);
    ASSERT_EQUAL(int_property->data(), 24);
    ASSERT_TRUE(m_TestProperty_install_called);
    ASSERT_TRUE(m_TestProperty_apply_called);
}

void Entitytest::test_setAttr_existing()
{
    PropertyBase * initial_property = m_entity->setProperty("test_int_property",
                                                            std::make_unique<TestProperty>());
    m_TestProperty_install_called = false;

    PropertyBase * pb = m_entity->setAttrValue("test_int_property", 24);
    ASSERT_NOT_NULL(pb);
    ASSERT_EQUAL(initial_property, pb);

    ASSERT_TRUE((pb->flags().m_flags & prop_flag_class) == 0);

    auto * int_property = dynamic_cast<TestProperty *>(pb);
    ASSERT_NOT_NULL(int_property);
    ASSERT_EQUAL(int_property->data(), 24);
    ASSERT_TRUE(!m_TestProperty_install_called);
    ASSERT_TRUE(m_TestProperty_apply_called);
}

void Entitytest::test_setAttr_type()
{
    auto type_property = new TestProperty;
    type_property->data() = 17;
    type_property->flags().m_flags &= prop_flag_class;
    m_type->injectProperty("test_int_property", std::unique_ptr<PropertyBase>(type_property));

    PropertyBase * pb = m_entity->setAttrValue("test_int_property", 24);
    ASSERT_NOT_NULL(pb);
    ASSERT_NOT_EQUAL(type_property, pb);

    ASSERT_TRUE((pb->flags().m_flags & prop_flag_class) == 0);

    auto * int_property = dynamic_cast<TestProperty *>(pb);
    ASSERT_NOT_NULL(int_property);
    ASSERT_EQUAL(int_property->data(), 24);
    ASSERT_TRUE(!m_TestProperty_install_called);
    ASSERT_TRUE(m_TestProperty_apply_called);
}

void Entitytest::test_sequence()
{
    // The entity exerciser creates one of these, and its singleton, so
    // we must remove ours.
    delete m_pm;
    m_pm = 0;

    IGEntityExerciser ee(m_entity);

    // Throw an op of every type at the entity
    ee.runOperations();

    // Subscribe the entity to every class of op
    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    // Add the test attributes
    m_entity->setAttrValue("test_int", 1);
    m_entity->setAttrValue("test_float", 1.f);
    m_entity->setAttrValue("test_list_string", "test_value");
    m_entity->setAttrValue("test_list_int", ListType(1, 1));
    m_entity->setAttrValue("test_list_float", ListType(1, 1.f));
    m_entity->setAttrValue("test_map_string", ListType(1, "test_value"));
    MapType test_map;
    test_map["test_key"] = 1;
    m_entity->setAttrValue("test_map_int", test_map);
    test_map["test_key"] = 1.f;
    m_entity->setAttrValue("test_map_float", test_map);
    test_map["test_key"] = "test_value";
    m_entity->setAttrValue("test_map_string", test_map);
    
    // Make sure we have the test attributes now
    MapType entityAsAtlas;

    // Dump a representation of the entity into an Atlas Message
    m_entity->addToMessage(entityAsAtlas);

    Atlas::Objects::Entity::RootEntity entityAsAtlasEntity;

    m_entity->addToEntity(entityAsAtlasEntity);

    // Make sure we got at least some of it
    assert(entityAsAtlas.size() >= 9);

    // Read the contents of the Atlas Message back in
    m_entity->merge(entityAsAtlas);

    // Throw an op of every type at the entity again now it is subscribed,
    // and full of data.
    ee.runOperations();

    {
        m_entity->getProperty("test_int");
    }

    {
        m_entity->getProperty("non_existant");
    }

    {
        m_entity->modProperty("test_int");
    }

    {
        m_entity->modProperty("non_existant");
    }

    {
        m_entity->modProperty("test_default");
    }

    {
        m_entity->setProperty("new_test_prop", std::unique_ptr<PropertyBase>(new SoftProperty));
    }
}

int main()
{
    Entitytest t;

    return t.run();
}

// stubs




void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}

#ifndef STUB_BaseWorld_getEntity
#define STUB_BaseWorld_getEntity
Ref<LocatedEntity> BaseWorld::getEntity(const std::string & id) const
{
    return getEntity(integerId(id));
}

Ref<LocatedEntity> BaseWorld::getEntity(long id) const
{
    auto I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second);
        return I->second;
    } else {
        return nullptr;
    }
}
#endif //STUB_BaseWorld_getEntity

#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stubcustom.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/rules/stubLocation.h"

#include "../stubs/rules/stubDomain.h"
#include "../stubs/common/stubProperty.h"
#include "../stubs/common/stubMonitors.h"
#include "../stubs/common/stubVariable.h"
#include "../stubs/rules/simulation/stubDomainProperty.h"
#include "../stubs/rules/stubAtlasProperties.h"
#include "../stubs/rules/simulation/stubBaseWorld.h"
#include "../stubs/rules/stubScript.h"
#include "../stubs/common/stubLink.h"
#include "../stubs/common/stubPropertyManager.h"
#include "../stubs/common/stubid.h"
#include "../stubs/common/stublog.h"
#include "../stubs/rules/stubModifier.h"
#include "../stubs/rules/stubPhysicalProperties.h"
