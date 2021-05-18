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

#include "../TestBase.h"

#include "common/Property.h"
#include "common/PropertyFactory.h"
#include "common/PropertyManager.h"

#include <Atlas/Objects/RootOperation.h>

#include <cassert>

class TestPropertyManager : public PropertyManager {
  public:
    TestPropertyManager() { }

    std::unique_ptr<PropertyBase> addProperty(const std::string &) const override {
        return {};
    }
};

class TestPropertyFactory : public PropertyKit
{
  public:
    std::unique_ptr<PropertyBase> newProperty() override
    {
        return {};
    }

    std::unique_ptr<PropertyKit> duplicateFactory() const override
    {
        return std::make_unique<TestPropertyFactory>();
    }
};

class PropertyManagertest : public Cyphesis::TestBase
{
  private:
    PropertyManager * m_pm;
  public:
    PropertyManagertest();

    void setup() override;
    void teardown() override;

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
    auto ret = m_pm->addProperty("one");

    ASSERT_FALSE(ret);
}

void PropertyManagertest::test_installFactory()
{
    m_pm->installFactory("test_property_factory",
                         {},
                         std::make_unique<TestPropertyFactory>());

    ASSERT_TRUE(m_pm->getPropertyFactories().find("test_property_factory") !=
                m_pm->getPropertyFactories().end());
    ASSERT_EQUAL(m_pm->getPropertyFactories().size(), 1u);
}

void PropertyManagertest::test_installFactory_duplicate()
{
    PropertyKit * first = new TestPropertyFactory;
    m_pm->installFactory("test_property_factory2", {}, std::unique_ptr<PropertyKit>(first));

    ASSERT_TRUE(m_pm->getPropertyFactories().find("test_property_factory2") !=
                m_pm->getPropertyFactories().end());
    ASSERT_EQUAL(
          m_pm->getPropertyFactories().find("test_property_factory2")->second.get(),
          first
    );
    ASSERT_EQUAL(m_pm->getPropertyFactories().size(), 1u);

    m_pm->installFactory("test_property_factory2",
                         {},
                         std::make_unique<TestPropertyFactory>());

    // Exactly the same tests as above. The second call should not have
    // installed something else, or affected what is there.
    ASSERT_TRUE(m_pm->getPropertyFactories().find("test_property_factory2") !=
                m_pm->getPropertyFactories().end());
    ASSERT_EQUAL(
          m_pm->getPropertyFactories().find("test_property_factory2")->second.get(),
          first
    );
    ASSERT_EQUAL(m_pm->getPropertyFactories().size(), 1u);
}

void PropertyManagertest::test_getPropertyFactory()
{
    PropertyKit * first = new TestPropertyFactory;
    m_pm->installFactory("test_property_factory3", {}, std::unique_ptr<PropertyKit>(first));

    ASSERT_TRUE(m_pm->getPropertyFactories().find("test_property_factory3") !=
                m_pm->getPropertyFactories().end());
    ASSERT_EQUAL(
          m_pm->getPropertyFactories().find("test_property_factory3")->second.get(),
          first
    );
    ASSERT_EQUAL(m_pm->getPropertyFactories().size(), 1u);

    m_pm->installFactory("test_property_factory3",
                         {},
                         std::make_unique<TestPropertyFactory>());

    PropertyKit * factory = m_pm->getPropertyFactory("test_property_factory3");

    ASSERT_NOT_NULL(factory);
    ASSERT_EQUAL(factory, first);
}

void PropertyManagertest::test_getPropertyFactory_nonexist()
{
    PropertyKit * first = new TestPropertyFactory;
    m_pm->installFactory("test_property_factory4", {}, std::unique_ptr<PropertyKit>(first));

    ASSERT_TRUE(m_pm->getPropertyFactories().find("test_property_factory4") !=
                m_pm->getPropertyFactories().end());
    ASSERT_EQUAL(
          m_pm->getPropertyFactories().find("test_property_factory4")->second.get(),
          first
    );
    ASSERT_EQUAL(m_pm->getPropertyFactories().size(), 1u);

    m_pm->installFactory("test_property_factory4",
                         {},
                         std::make_unique<TestPropertyFactory>());

    PropertyKit * factory = m_pm->getPropertyFactory("non_existent_factory");

    ASSERT_NULL(factory);
}

int main()
{
    PropertyManagertest t;

    return t.run();
}

// stubs

