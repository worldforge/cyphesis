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

#include "TestBase.h"

#include "server/PropertyRuleHandler.h"

#include "common/Property.h"
#include "common/PropertyFactory.h"
#include "common/PropertyManager.h"

#include <Atlas/Objects/Anonymous.h>

#include <cstdlib>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;

class TestPropertyManager : public PropertyManager
{
  public:
    virtual PropertyBase * addProperty(const std::string & name,
                                       int type);
};

PropertyBase * TestPropertyManager::addProperty(const std::string & name,
                                                int type)
{
    return 0;
}

class PropertyRuleHandlertest : public Cyphesis::TestBase
{
  private:
    PropertyRuleHandler * rh;
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
    new TestPropertyManager;
    rh = new PropertyRuleHandler(0);
}

void PropertyRuleHandlertest::teardown()
{
    delete rh;
    delete PropertyManager::instance();
}

void PropertyRuleHandlertest::test_sequence()
{
}

// check() empty description
void PropertyRuleHandlertest::test_check_fail()
{
    Anonymous description;
    description->setParents(std::list<std::string>(1, "foo"));
    int ret = rh->check(description);

    assert(ret == -1);
}

// check() description with op_definition objtype
void PropertyRuleHandlertest::test_check_pass()
{
    Anonymous description;
    description->setObjtype("type");
    description->setParents(std::list<std::string>(1, "foo"));
    int ret = rh->check(description);

    assert(ret == 0);
}

void PropertyRuleHandlertest::test_install()
{
    Anonymous description;
    description->setObjtype("type");
    std::string dependent, reason;

    int ret = rh->install("new_int_type", "int", description, dependent, reason);

    assert(ret == 0);
}

void PropertyRuleHandlertest::test_install_noparent()
{
    Anonymous description;
    description->setObjtype("type");
    std::string dependent, reason;

    int ret = rh->install("new_int_type", "int", description, dependent, reason);

    assert(ret == 0);
}

void PropertyRuleHandlertest::test_install_exists()
{
    PropertyManager::instance()->installFactory("existing_int_type",
          Root(),
          new PropertyFactory<Property<int>>);

    Anonymous description;
    description->setObjtype("type");
    std::string dependent, reason;

    int ret = rh->install("existing_int_type", "int", description, dependent, reason);

    assert(ret == 0);
}

void PropertyRuleHandlertest::test_update()
{
    Anonymous description;
    int ret = rh->update("", description);

    // FIXME Currently does nothing
    assert(ret == 0);
}

int main()
{
    PropertyRuleHandlertest t;

    return t.run();
}

// stubs

#include "Property_stub_impl.h"

#include "common/Inheritance.h"
#include "common/log.h"
#include "common/PropertyFactory_impl.h"

RuleHandler::~RuleHandler()
{
}

int RuleHandler::getScriptDetails(const Atlas::Message::MapType & script,
                                  const std::string & class_name,
                                  const std::string & context,
                                  std::string & script_package,
                                  std::string & script_class)
{
    return 0;
}

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance() : noClass(0)
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
    return 0;
}

bool Inheritance::hasClass(const std::string & parent)
{
    return true;
}

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(LocatedEntity *, const std::string & name)
{
}

void PropertyBase::remove(LocatedEntity *)
{
}

void PropertyBase::apply(LocatedEntity *)
{
}

void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}

HandlerResult PropertyBase::operation(LocatedEntity *,
                                      const Operation &,
                                      OpVector & res)
{
    return OPERATION_IGNORED;
}

PropertyKit::~PropertyKit()
{
}

PropertyManager * PropertyManager::m_instance = 0;

PropertyManager::PropertyManager()
{
    assert(m_instance == 0);
    m_instance = this;
    m_propertyFactories["int"] = new PropertyFactory<Property<int>>;
}

PropertyManager::~PropertyManager()
{
    m_instance = 0;
}

int PropertyManager::installFactory(const std::string & type_name,
                                    const Root & type_desc,
                                    PropertyKit * factory)
{
    return 0;
}

PropertyKit * PropertyManager::getPropertyFactory(const std::string & name) const
{
    auto I = m_propertyFactories.find(name);
    if (I != m_propertyFactories.end()) {
        assert(I->second != 0);
        return I->second;
    }
    return 0;
}

void PropertyManager::installFactory(const std::string & name,
                                     PropertyKit * factory)
{
}

Root atlasOpDefinition(const std::string & name, const std::string & parent)
{
    return Root();
}

void log(LogLevel lvl, const std::string & msg)
{
}
