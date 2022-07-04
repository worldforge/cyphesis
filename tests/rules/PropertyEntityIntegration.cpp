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

#include "../TestBaseWithContext.h"
#include "../TestWorld.h"

#include "rules/simulation/Entity.h"

#include "common/Property.h"
#include "common/PropertyManager.h"
#include "common/TypeNode.h"

#include <cstdlib>

#include <cassert>
#include <rules/simulation/World.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

std::ostream& operator<<(std::ostream& os,
                         const std::map<std::string, ModifiableProperty>::const_iterator&)
{
    os << "[iterator]";
    return os;
}

template<typename T>
class test_values
{
    public:
        static const char* name;
        static T initial_value;
        static T default_value;
};

template<>
const char* test_values<long>::name = "test_int";

template<>
long test_values<long>::initial_value = 42;

template<>
long test_values<long>::default_value = 23;

template<>
const char* test_values<double>::name = "test_float";

template<>
double test_values<double>::initial_value = 69.5;

template<>
double test_values<double>::default_value = 17.5;

template<>
const char* test_values<std::string>::name = "test_string";

template<>
std::string test_values<std::string>::initial_value =
        "1356ebe4-220d-46a7-8c69-73f787f6b1ff";

template<>
std::string test_values<std::string>::default_value =
        "1423a858-f7c6-4b3c-8b0b-726af6dcbeff";

template<>
const char* test_values<MapType>::name = "test_map";

template<>
MapType test_values<MapType>::initial_value = {
        std::make_pair("map_int", 23),
        std::make_pair("map_float", 17.5f),
};

template<>
MapType test_values<MapType>::default_value = {
        std::make_pair("map_int", 42l),
        std::make_pair("map_float", 69.5f),
};

class TestPropertyManager : public PropertyManager
{
    public:
        virtual ~TestPropertyManager();

        std::unique_ptr<PropertyBase> addProperty(const std::string& name) const override;
};

TestPropertyManager::~TestPropertyManager()
{
}

std::unique_ptr<PropertyBase> TestPropertyManager::addProperty(const std::string& name) const
{
    if (name == test_values<long>::name) {
        return std::make_unique<Property<long>>();
    } else if (name == test_values<double>::name) {
        return std::make_unique<Property<double>>();
    } else if (name == test_values<std::string>::name) {
        return std::make_unique<Property<std::string>>();
    } else if (name == test_values<MapType>::name) {
        return std::make_unique<Property<MapType>>();
    } else {
        return std::make_unique<SoftProperty>();
    }
}

struct TestEntity : Entity
{
    explicit TestEntity(RouterId id) : Entity(id)
    {}

    std::map<std::string, ModifiableProperty>& modProperties()
    { return m_properties; }

    void sendWorld(Operation op) override
    {
        //no-op
    }

};

struct TestContext
{
    TypeNode m_type{"test_type"};
    Ref<TestEntity> m_entity{new TestEntity(1L)};
    TestPropertyManager propertyManager{};

    TestContext()
    {
        m_type.addProperties(MapType{
                std::make_pair(test_values<long>::name,
                               test_values<long>::initial_value),
                std::make_pair(test_values<double>::name,
                               test_values<double>::initial_value),
                std::make_pair(test_values<std::string>::name,
                               test_values<std::string>::initial_value),
                std::make_pair(test_values<MapType>::name,
                               test_values<MapType>::initial_value)
        }, propertyManager);
        m_entity->setType(&m_type);
    }
};

struct PropertyEntityIntegration : public Cyphesis::TestBaseWithContext<TestContext>
{


    PropertyEntityIntegration()
    {
        ADD_TEST(test_addModifiersToTypeAttr);
        ADD_TEST(test_setAttrInt);
        ADD_TEST(PropertyEntityIntegration::test_setAttrIntWithNameModifiers);
        ADD_TEST(PropertyEntityIntegration::test_setAttrList);
        ADD_TEST(PropertyEntityIntegration::test_setAttrMap);
        ADD_TEST(PropertyEntityIntegration::test_setAttrWithModifiers);


        ADD_TEST(PropertyEntityIntegration::test_requirePropertyClass<long>);
        ADD_TEST(PropertyEntityIntegration::test_requirePropertyClass<double>);
        ADD_TEST(PropertyEntityIntegration::test_requirePropertyClass<std::string>);
        ADD_TEST(PropertyEntityIntegration::test_requirePropertyClass<MapType>);

        ADD_TEST(PropertyEntityIntegration::test_requirePropertyClass_default<long>);
        ADD_TEST(PropertyEntityIntegration::test_requirePropertyClass_default<double>);
        ADD_TEST(PropertyEntityIntegration::test_requirePropertyClass_default<std::string>);
        ADD_TEST(PropertyEntityIntegration::test_requirePropertyClass_default<MapType>);

        ADD_TEST(PropertyEntityIntegration::test_modProperty<long>);
        ADD_TEST(PropertyEntityIntegration::test_modProperty<double>);
        ADD_TEST(PropertyEntityIntegration::test_modProperty<std::string>);
        ADD_TEST(PropertyEntityIntegration::test_modProperty<MapType>);

        ADD_TEST(PropertyEntityIntegration::test_modPropertyClass<long>);
        ADD_TEST(PropertyEntityIntegration::test_modPropertyClass<double>);
        ADD_TEST(PropertyEntityIntegration::test_modPropertyClass<std::string>);
        ADD_TEST(PropertyEntityIntegration::test_modPropertyClass<MapType>);
    }


    void test_setAttrWithModifiers(const TestContext& context)
    {

        //Test different ways of setting an attribute which has a modifier
        AppendModifier appendModifier(1);

        {
            Ref<TestEntity> entity(new TestEntity(1L));
            //We'll add a modifier without any previous value
            entity->addModifier("foo", &appendModifier, entity.get());
            entity->setAttrValue("foo!append", 1);
            Atlas::Message::Element element;
            entity->getAttr("foo", element);
            ASSERT_EQUAL(element, 2)
        }

        {
            Ref<TestEntity> entity(new TestEntity(1L));
            entity->addModifier("foo", &appendModifier, entity.get());
            entity->setAttrValue("foo", 4);
            Atlas::Message::Element element;
            entity->getAttr("foo", element);
            ASSERT_EQUAL(element, 5)
        }
    }

    void test_setAttrInt(const TestContext& context)
    {
        Atlas::Message::Element element;

        context.m_entity->setAttrValue("foo", 1);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 1);

        context.m_entity->setAttrValue("foo", 2);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 2);

        context.m_entity->setAttrValue("foo!append", 2);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 4);

        context.m_entity->setAttrValue("foo!prepend", 2);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 6);

        context.m_entity->setAttrValue("foo!subtract", 3);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 3);

        context.m_entity->setAttrValue("foo!add-fraction", 2);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 9);

        context.m_entity->setAttrValue("bar!append", 2);
        context.m_entity->getAttr("bar", element);
        ASSERT_EQUAL(element, 2);

        context.m_entity->setAttrValue("baz!add-fraction", 2);
        context.m_entity->getAttr("baz", element);
        ASSERT_TRUE(element.isNone());
    }

    void test_setAttrIntWithNameModifiers(const TestContext& context)
    {
        Atlas::Message::Element element;

        AppendModifier appendModifier(1);

        context.m_entity->addModifier("foo", &appendModifier, context.m_entity.get());

        context.m_entity->setAttrValue("foo", 1);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 2);


        AddFractionModifier addFractionModifier(2.0);
        context.m_entity->addModifier("foo", &addFractionModifier, context.m_entity.get());

        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 4);


        context.m_entity->setAttrValue("foo", 6);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 19);


        context.m_entity->removeModifier("foo", &addFractionModifier);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 7);
    }

    void test_addModifiersToTypeAttr(TestContext& context)
    {
        Atlas::Message::Element element;
        context.m_type.addProperties(MapType{
                                             {"foo", 10}
                                     },
                                     context.propertyManager);


        ASSERT_EQUAL(*context.m_entity->getAttr("foo"), 10);

        AppendModifier appendModifier(1);

        context.m_entity->addModifier("foo", &appendModifier, context.m_entity.get());
        ASSERT_EQUAL(*context.m_entity->getAttr("foo"), 11);
        context.m_entity->removeModifier("foo", &appendModifier);
        ASSERT_EQUAL(*context.m_entity->getAttr("foo"), 10);
    }

    void test_setAttrList(const TestContext& context)
    {
        Atlas::Message::Element element;

        Atlas::Message::ListType list;
        list.emplace_back(1);
        context.m_entity->setAttrValue("foo", list);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, list);

        list.emplace_back(2);

        context.m_entity->setAttrValue("foo", list);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, list);

        list.clear();
        list.emplace_back(4);
        context.m_entity->setAttrValue("foo!append", list);
        context.m_entity->getAttr("foo", element);
        Atlas::Message::ListType correctList;
        correctList.emplace_back(1);
        correctList.emplace_back(2);
        correctList.emplace_back(4);
        ASSERT_EQUAL(element, correctList);

        list.clear();
        list.emplace_back(2);
        context.m_entity->setAttrValue("foo!prepend", list);
        context.m_entity->getAttr("foo", element);
        correctList.clear();
        correctList.emplace_back(2);
        correctList.emplace_back(1);
        correctList.emplace_back(2);
        correctList.emplace_back(4);
        ASSERT_EQUAL(element, correctList);


        list.clear();
        list.emplace_back(4);
        context.m_entity->setAttrValue("foo!subtract", list);
        context.m_entity->getAttr("foo", element);
        correctList.clear();
        correctList.emplace_back(2);
        correctList.emplace_back(1);
        correctList.emplace_back(2);
        ASSERT_EQUAL(element, correctList);
    }

    void test_setAttrMap(const TestContext& context)
    {
        Atlas::Message::Element element;

        Atlas::Message::MapType map;
        map.emplace("one", 1);

        context.m_entity->setAttrValue("foo", map);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, map);

        map.emplace("two", 2);

        context.m_entity->setAttrValue("foo", map);
        context.m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, map);

        map.clear();
        map.emplace("three", 3);
        context.m_entity->setAttrValue("foo!append", map);
        context.m_entity->getAttr("foo", element);
        Atlas::Message::MapType correctMap = {{"one",   1},
                                              {"two",   2},
                                              {"three", 3}};
        ASSERT_EQUAL(element, correctMap);

        map.clear();
        map.emplace("four", 4);
        context.m_entity->setAttrValue("foo!prepend", map);
        context.m_entity->getAttr("foo", element);
        correctMap = {{"one",   1},
                      {"two",   2},
                      {"three", 3},
                      {"four",  4}};
        ASSERT_EQUAL(element, correctMap);

        map.clear();
        map.emplace("four", 400);
        context.m_entity->setAttrValue("foo!prepend", map);
        context.m_entity->getAttr("foo", element);
        correctMap = {{"one",   1},
                      {"two",   2},
                      {"three", 3},
                      {"four",  400}};
        ASSERT_EQUAL(element, correctMap);

        map.clear();
        map.emplace("four", "any_value_it_doesnt_matter");
        context.m_entity->setAttrValue("foo!subtract", map);
        context.m_entity->getAttr("foo", element);
        correctMap = {{"one",   1},
                      {"two",   2},
                      {"three", 3}};
        ASSERT_EQUAL(element, correctMap);
    }

    template<class T>
    void test_requirePropertyClass(const TestContext& context)
    {
        context.m_entity->requirePropertyClass<Property<T>>("bill");
    }

    template<class T>
    void test_requirePropertyClass_default(const TestContext& context)
    {
        auto& p = context.m_entity->requirePropertyClass<Property<T>>("bill",
                                                                     Element(test_values<T>::default_value));
        ASSERT_EQUAL(p.data(), test_values<T>::default_value);
    }

    template<class T>
    void test_modProperty(TestContext& context)
    {
        // Get a pointer to the types default property
        auto& dflt = context.m_type.defaults().find(test_values<T>::name)->second;
        ASSERT_TRUE(dflt);
        ASSERT_TRUE(dflt->flags().m_flags & prop_flag_class);

        // The entity instance should not have a property by this name
        ASSERT_EQUAL(context.m_entity->getProperties().find(test_values<T>::name),
                     context.m_entity->getProperties().end());

        PropertyBase* p = context.m_entity->modProperty(test_values<T>::name);
        ASSERT_NOT_NULL(p);
        ASSERT_TRUE((p->flags().m_flags & prop_flag_class) == 0);
        // modProperty should have forced a new object
        ASSERT_NOT_EQUAL(p, dflt.get());

        auto subp = dynamic_cast<Property<T>*>(p);
        ASSERT_NOT_NULL(subp);
    }

    template<class T>
    void test_modPropertyClass(const TestContext& context)
    {
        // Get a pointer to the types default property
        auto& dflt = context.m_type.defaults().find(test_values<T>::name)->second;
        ASSERT_TRUE(dflt);
        ASSERT_TRUE(dflt->flags().m_flags & prop_flag_class);

        // The entity instance should not have a property by this name
        ASSERT_EQUAL(context.m_entity->getProperties().find(test_values<T>::name),
                     context.m_entity->getProperties().end());

        auto p = context.m_entity->modPropertyClass<Property<T>>(
                test_values<T>::name
        );
        ASSERT_NOT_NULL(p);
        ASSERT_TRUE((p->flags().m_flags & prop_flag_class) == 0);
        // modProperty should have forced a new object
        ASSERT_NOT_EQUAL(p, dflt.get());

        ASSERT_EQUAL(p->data(), test_values<T>::initial_value);
    }


};


int main()
{
    PropertyEntityIntegration t;

    return t.run();
}

// stubs

#include "rules/AtlasProperties.h"
#include "rules/Domain.h"
#include "rules/simulation/DomainProperty.h"
#include "rules/Script.h"

#include "common/id.h"


#include "../stubs/common/stubVariable.h"
#include "../stubs/common/stubMonitors.h"
#include "../stubs/common/stubcustom.h"
#include "../stubs/rules/stubDomain.h"
#include "../stubs/rules/simulation/stubDomainProperty.h"

void addToEntity(const Point3D& p, std::vector<double>& vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}

#ifndef STUB_BaseWorld_getEntity
#define STUB_BaseWorld_getEntity

Ref<LocatedEntity> BaseWorld::getEntity(const std::string& id) const
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

#include "../stubs/rules/simulation/stubBaseWorld.h"

#include "../stubs/rules/stubScript.h"

#include "../stubs/rules/stubLocation.h"
#include "../stubs/rules/stubAtlasProperties.h"
#include "../stubs/common/stubPropertyManager.h"
#include "../stubs/common/stubLink.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stubid.h"
#include "../stubs/common/stublog.h"
#include "../stubs/rules/stubPhysicalProperties.h"

