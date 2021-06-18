// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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

#include "server/PropertyRuleHandler.h"

#include "common/Property.h"
#include "common/PropertyFactory.h"
#include "common/PropertyManager.h"

#include <Atlas/Objects/Anonymous.h>

#include <cstdlib>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;

struct TestPropertyManager : public PropertyManager
{

    TestPropertyManager()
    {
        m_propertyFactories["int"] = std::make_unique<PropertyFactory<Property<int>>>();
    }

    std::unique_ptr<PropertyBase> addProperty(const std::string& name) const override
    {
        return {};
    }
};


class PropertyRuleHandlertest : public Cyphesis::TestBase
{
    private:
        PropertyRuleHandler* rh;
        PropertyManager* propertyManager;
    public:
        PropertyRuleHandlertest();

        void setup();

        void teardown();

        void test_sequence();

        void test_check_fail();

        void test_check_pass();

        void test_install();

        void test_install_noparent();

        void test_install_exists();

        void test_update();
};

PropertyRuleHandlertest::PropertyRuleHandlertest()
{
    ADD_TEST(PropertyRuleHandlertest::test_sequence);
    ADD_TEST(PropertyRuleHandlertest::test_check_fail);
    ADD_TEST(PropertyRuleHandlertest::test_check_pass);
    ADD_TEST(PropertyRuleHandlertest::test_install);
    ADD_TEST(PropertyRuleHandlertest::test_install_noparent);
    ADD_TEST(PropertyRuleHandlertest::test_install_exists);
    ADD_TEST(PropertyRuleHandlertest::test_update);
}

void PropertyRuleHandlertest::setup()
{
    propertyManager = new TestPropertyManager;
    rh = new PropertyRuleHandler(*propertyManager);
}

void PropertyRuleHandlertest::teardown()
{
    delete rh;
    delete propertyManager;
}

void PropertyRuleHandlertest::test_sequence()
{
}

// check() empty description
void PropertyRuleHandlertest::test_check_fail()
{
    Anonymous description;
    description->setParent("foo");
    int ret = rh->check(description);

    assert(ret == -1);
}

// check() description with op_definition objtype
void PropertyRuleHandlertest::test_check_pass()
{
    Anonymous description;
    description->setObjtype("type");
    description->setParent("foo");
    int ret = rh->check(description);

    assert(ret == 0);
}

void PropertyRuleHandlertest::test_install()
{
    std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;

    Anonymous description;
    description->setObjtype("type");
    std::string dependent, reason;

    int ret = rh->install("new_int_type", "int", description, dependent, reason, changes);

    assert(ret == 0);
}

void PropertyRuleHandlertest::test_install_noparent()
{
    std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;

    Anonymous description;
    description->setObjtype("type");
    std::string dependent, reason;

    int ret = rh->install("new_int_type", "int", description, dependent, reason, changes);

    assert(ret == 0);
}

void PropertyRuleHandlertest::test_install_exists()
{
    std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;

    propertyManager->installFactory("existing_int_type",
                                    Root(),
                                    std::make_unique<PropertyFactory<Property<int>>>());

    Anonymous description;
    description->setObjtype("type");
    std::string dependent, reason;

    int ret = rh->install("existing_int_type", "int", description, dependent, reason, changes);

    assert(ret == 0);
}

void PropertyRuleHandlertest::test_update()
{
    std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;

    Anonymous description;
    int ret = rh->update("", description, changes);

    // FIXME Currently does nothing
    assert(ret == 0);
}

int main()
{
    PropertyRuleHandlertest t;

    return t.run();
}

// stubs

#include "../stubs/common/stubProperty.h"
#include "common/Inheritance.h"
#include "common/log.h"
#include "common/PropertyFactory_impl.h"

#ifndef STUB_PropertyManager_getPropertyFactory
#define STUB_PropertyManager_getPropertyFactory

PropertyKit* PropertyManager::getPropertyFactory(const std::string& name) const
{
    auto I = m_propertyFactories.find(name);
    if (I != m_propertyFactories.end()) {
        assert(I->second != 0);
        return I->second.get();
    }
    return 0;
}

#endif //STUB_PropertyManager_getPropertyFactory

#include "../stubs/common/stubPropertyManager.h"

Root atlasOpDefinition(const std::string& name, const std::string& parent)
{
    return Root();
}

#include "../stubs/common/stublog.h"

