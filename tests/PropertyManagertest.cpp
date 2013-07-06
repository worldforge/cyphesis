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

#include "common/PropertyFactory.h"
#include "common/PropertyManager.h"

#include <cassert>

class TestPropertyManager : public PropertyManager {
  public:
    TestPropertyManager() { }

    virtual PropertyBase * addProperty(const std::string &, int) {
        return 0;
    }
};

class TestPropertyFactory : public PropertyKit
{
  public:
    virtual PropertyBase * newProperty()
    {
        return 0;
    }

    virtual TestPropertyFactory * duplicateFactory() const
    {
        return new TestPropertyFactory;
    }
};

class PropertyManagertest : public Cyphesis::TestBase
{
  private:
    PropertyManager * m_pm;
  public:
    PropertyManagertest();

    void setup();
    void teardown();

    void test_interface();
    void test_installFactory();
    void test_installFactory_duplicate();
    void test_getPropertyFactory();
    void test_getPropertyFactory_nonexist();
};

PropertyManagertest::PropertyManagertest()
{
    ADD_TEST(PropertyManagertest::test_interface);
    ADD_TEST(PropertyManagertest::test_installFactory);
    ADD_TEST(PropertyManagertest::test_installFactory_duplicate);
    ADD_TEST(PropertyManagertest::test_getPropertyFactory);
    ADD_TEST(PropertyManagertest::test_getPropertyFactory_nonexist);
}

void PropertyManagertest::setup()
{
    m_pm = new TestPropertyManager;
}

void PropertyManagertest::teardown()
{
    delete m_pm;
}

void PropertyManagertest::test_interface()
{
    auto * ret = m_pm->addProperty("one", 1);

    ASSERT_NULL(ret);
}

void PropertyManagertest::test_installFactory()
{
    m_pm->installFactory("test_property_factory",
                         new TestPropertyFactory);

    ASSERT_TRUE(m_pm->m_propertyFactories.find("test_property_factory") !=
                m_pm->m_propertyFactories.end());
    ASSERT_EQUAL(m_pm->m_propertyFactories.size(), 1u);
}

void PropertyManagertest::test_installFactory_duplicate()
{
    PropertyKit * first = new TestPropertyFactory;
    m_pm->installFactory("test_property_factory2", first);

    ASSERT_TRUE(m_pm->m_propertyFactories.find("test_property_factory2") !=
                m_pm->m_propertyFactories.end());
    ASSERT_EQUAL(
          m_pm->m_propertyFactories.find("test_property_factory2")->second,
          first
    );
    ASSERT_EQUAL(m_pm->m_propertyFactories.size(), 1u);

    m_pm->installFactory("test_property_factory2",
                         new TestPropertyFactory);

    // Exactly the same tests as above. The second call should not have
    // installed something else, or affected what is there.
    ASSERT_TRUE(m_pm->m_propertyFactories.find("test_property_factory2") !=
                m_pm->m_propertyFactories.end());
    ASSERT_EQUAL(
          m_pm->m_propertyFactories.find("test_property_factory2")->second,
          first
    );
    ASSERT_EQUAL(m_pm->m_propertyFactories.size(), 1u);
}

void PropertyManagertest::test_getPropertyFactory()
{
    PropertyKit * first = new TestPropertyFactory;
    m_pm->installFactory("test_property_factory3", first);

    ASSERT_TRUE(m_pm->m_propertyFactories.find("test_property_factory3") !=
                m_pm->m_propertyFactories.end());
    ASSERT_EQUAL(
          m_pm->m_propertyFactories.find("test_property_factory3")->second,
          first
    );
    ASSERT_EQUAL(m_pm->m_propertyFactories.size(), 1u);

    m_pm->installFactory("test_property_factory3",
                         new TestPropertyFactory);

    PropertyKit * factory = m_pm->getPropertyFactory("test_property_factory3");

    ASSERT_NOT_NULL(factory);
    ASSERT_EQUAL(factory, first);
}

void PropertyManagertest::test_getPropertyFactory_nonexist()
{
    PropertyKit * first = new TestPropertyFactory;
    m_pm->installFactory("test_property_factory4", first);

    ASSERT_TRUE(m_pm->m_propertyFactories.find("test_property_factory4") !=
                m_pm->m_propertyFactories.end());
    ASSERT_EQUAL(
          m_pm->m_propertyFactories.find("test_property_factory4")->second,
          first
    );
    ASSERT_EQUAL(m_pm->m_propertyFactories.size(), 1u);

    m_pm->installFactory("test_property_factory4",
                         new TestPropertyFactory);

    PropertyKit * factory = m_pm->getPropertyFactory("non_existent_factory");

    ASSERT_NULL(factory);
}

int main()
{
    PropertyManagertest t;

    return t.run();
}

// stubs

PropertyKit::~PropertyKit()
{
}
