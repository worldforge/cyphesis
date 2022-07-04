// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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

#include "../common/EntityExerciser.h"
#include "../TestBase.h"

#include "rules/LocatedEntity.h"

#include "rules/AtlasProperties.h"
#include "rules/Script.h"
#include "../TestPropertyManager.h"

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

class LocatedEntitytest : public Cyphesis::TestBase
{
  private:
    Ref<LocatedEntity> m_entity;
    static bool m_TestProperty_remove_called;
  public:
    LocatedEntitytest();

    void setup();
    void teardown();

    void test_setProperty();
    void test_removeAttr();
    void test_coverage();

    class TestProperty : public PropertyBase
    {
      public:
        virtual void remove(LocatedEntity&, const std::string & name);
        virtual TestProperty * copy() const;
        virtual int get(Atlas::Message::Element&) const;
        virtual void set(const Atlas::Message::Element&);
    };

    static void TestProperty_remove_called()
    {
        m_TestProperty_remove_called = true;
    }

};

bool LocatedEntitytest::m_TestProperty_remove_called;

void LocatedEntitytest::TestProperty::remove(LocatedEntity&, const std::string & name)
{
    LocatedEntitytest::TestProperty_remove_called();
}

int LocatedEntitytest::TestProperty::get(Atlas::Message::Element&) const
{
    return 0;
}

void LocatedEntitytest::TestProperty::set(const Atlas::Message::Element&)
{
}


LocatedEntitytest::TestProperty * LocatedEntitytest::TestProperty::copy() const
{
    return new LocatedEntitytest::TestProperty(*this);
}

LocatedEntitytest::LocatedEntitytest()
{
    ADD_TEST(LocatedEntitytest::test_setProperty);
    ADD_TEST(LocatedEntitytest::test_removeAttr);
    ADD_TEST(LocatedEntitytest::test_coverage);
}

void LocatedEntitytest::setup()
{
    m_entity = new LocatedEntityTest(1);
}

void LocatedEntitytest::teardown()
{
    m_entity = nullptr;
}

void LocatedEntitytest::test_setProperty()
{
    std::string test_property("test_property");
    ASSERT_TRUE(m_entity->getProperties().find(test_property) ==
                m_entity->getProperties().end());

    PropertyBase * tp = new SoftProperty;

    m_entity->setProperty(test_property, std::unique_ptr<PropertyBase>(tp));

    ASSERT_TRUE(m_entity->getProperties().find(test_property) !=
                m_entity->getProperties().end());
}

void LocatedEntitytest::test_removeAttr()
{
    std::string test_property("test_property");
    LocatedEntity* entity = new LocatedEntityTest(1);

    ASSERT_TRUE(!m_TestProperty_remove_called);

    PropertyBase * tp = new TestProperty();

    entity->setProperty(test_property, std::unique_ptr<PropertyBase>(tp));

    ASSERT_TRUE(!m_TestProperty_remove_called);
    delete entity;
    ASSERT_TRUE(m_TestProperty_remove_called);
}

void LocatedEntitytest::test_coverage()
{
    m_entity->setScript(std::make_unique<Script>());
    // Installing a second one should delete the first.
    m_entity->setScript(std::make_unique<Script>());

    m_entity->onContainered(nullptr);
    m_entity->onUpdated();

    EntityExerciser ee(m_entity);
    // Throw an op of every type at the entity
    ee.runOperations();

    // Subscribe the entity to every class of op
    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    std::set<std::string> attrNames;
    attrNames.insert("id");

    // Make sure we have all the default attributes
    assert(ee.checkAttributes(attrNames));

    attrNames.insert("test_int");
    attrNames.insert("test_float");
    attrNames.insert("test_list_string");
    attrNames.insert("test_list_int");
    attrNames.insert("test_list_float");
    attrNames.insert("test_map_string");
    attrNames.insert("test_map_int");
    attrNames.insert("test_map_float");

    // Make sure we don't have the test attributes yet
    assert(!ee.checkAttributes(attrNames));

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
    assert(ee.checkAttributes(attrNames));

    MapType entityAsAtlas;

    m_entity->merge(entityAsAtlas);

    // Throw an op of every type at the entity again now it is subscribed,
    // and full of data.
    ee.runOperations();

}

int main()
{
    TestPropertyManager propertyManager;
    LocatedEntityTest::propertyManager = &propertyManager;
    // Test constructor
    {
        LocatedEntityTest e(1);
    }

    // Test destructor
    {
        LocatedEntityTest * e = new LocatedEntityTest(1);
        delete e;
    }

    // Test checkRef()
    {
        LocatedEntityTest * e = new LocatedEntityTest(1);

        assert(e->checkRef() == 0);
        delete e;
    }

    // Test incRef()
    {
        LocatedEntityTest * e = new LocatedEntityTest(1);

        e->incRef();
        assert(e->checkRef() == 1);
        delete e;
    }

    // Test decRef()
    {
        LocatedEntityTest * e = new LocatedEntityTest(1);

        e->incRef();
        e->incRef();
        e->decRef();
        assert(e->checkRef() == 1);
        delete e;
    }

    // Test decRef()
    {
        LocatedEntityTest * e = new LocatedEntityTest(1);
        e->incRef();
        e->decRef();
        // Enitity deleted - verified as not leaked
    }

    // Test setAttrValue()
    {
        LocatedEntityTest e(1);

        e.setAttrValue("foo", "bar");
    }

    // Test setAttrValue()
    {
        LocatedEntityTest e(1);

        e.setAttrValue("foo", "bar");
        e.setAttrValue("foo", 23);
    }

    // Test getAttr()
    {
        LocatedEntityTest e(1);
        Atlas::Message::Element val;

        int ret = e.getAttr("foo", val);
        assert(ret == -1);
        assert(val.isNone());
    }

    // Test getAttr()
    {
        LocatedEntityTest e(1);
        Atlas::Message::Element val;

        e.setAttrValue("foo", "bar");
        int ret = e.getAttr("foo", val);
        assert(ret == 0);
        assert(val.isString());
    }

    // Test getAttrType()
    {
        LocatedEntityTest e(1);
        Atlas::Message::Element val;

        int ret = e.getAttrType("foo", val, Element::TYPE_STRING);
        assert(ret == -1);
        assert(val.isNone());
    }

    // Test getAttrType()
    {
        LocatedEntityTest e(1);
        Atlas::Message::Element val;

        int ret = e.getAttrType("id", val, Element::TYPE_STRING);
        assert(ret == 0);
        assert(val.isString());
    }

    // Test getAttrType()
    {
        LocatedEntityTest e(1);
        Atlas::Message::Element val;

        int ret = e.getAttrType("id", val, Element::TYPE_FLOAT);
        assert(ret != 0);
        assert(val.isString());
    }

    // Test hasAttr()
    {
        LocatedEntityTest e(1);
        Atlas::Message::Element val;

        bool ret = e.hasAttr("foo");
        assert(ret == false);
    }

    // Test hasAttr()
    {
        LocatedEntityTest e(1);
        Atlas::Message::Element val;

        e.setAttrValue("foo", "bar");
        bool ret = e.hasAttr("foo");
        assert(ret == true);
    }

    LocatedEntitytest t;

    return t.run();
}

// stubs


#include "common/TypeNode.h"

#define STUB_SoftProperty_get
int SoftProperty::get(Atlas::Message::Element & val) const
{
    val = m_data;
    return 0;
}
#define STUB_SoftProperty_set
void SoftProperty::set(const Atlas::Message::Element & val)
{
    m_data = val;
}


#include "../stubs/rules/simulation/stubDomainProperty.h"
#include "../stubs/common/stubProperty.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stubPropertyManager.h"
#include "../stubs/common/stubcustom.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stublog.h"
#include "../stubs/rules/stubScript.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/rules/stubModifier.h"
