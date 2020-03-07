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
#include "../TestWorld.h"

#include "rules/simulation/Entity.h"

#include "common/Property.h"
#include "common/PropertyManager.h"
#include "common/TypeNode.h"

#include <cstdlib>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

// If tests fail, and print out the message below, you'll have to actually
// implement this function to find out the details.
std::ostream& operator<<(std::ostream& os, const MapType& v)
{
    os << "[ATLAS_MAP]";
    return os;
}

std::ostream& operator<<(std::ostream& os,
                         const std::map<std::string, ModifiableProperty>::const_iterator&)
{
    os << "[iterator]";
    return os;
}

namespace std {

    std::ostream& operator<<(std::ostream& os, const Atlas::Message::Element& v);

    std::ostream& operator<<(std::ostream& os, const Atlas::Message::Element& v)
    {
        switch (v.getType()) {
            case Element::TYPE_NONE:
                os << "None";
                break;
            case Element::TYPE_INT:
                os << v.Int();
                break;
            case Element::TYPE_FLOAT:
                os << v.Float();
                break;
            case Element::TYPE_PTR:
                os << v.Ptr();
                break;
            case Element::TYPE_STRING:
                os << v.String();
                break;
            case Element::TYPE_MAP:
                os << "[Map]";
                break;
            case Element::TYPE_LIST:
                os << "[List]";
                break;
            default:
                os << "[Atlas Element]";
        }
        return os;
    }

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

        virtual std::unique_ptr<PropertyBase> addProperty(const std::string& name, int type);
};

TestPropertyManager::~TestPropertyManager()
{
}

std::unique_ptr<PropertyBase> TestPropertyManager::addProperty(const std::string& name,
                                                               int type)
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
    explicit TestEntity(const std::string& id, long intId) : Entity(id, intId)
    {}

    std::map<std::string, ModifiableProperty>& modProperties()
    { return m_properties; }

    void sendWorld(Operation op) override
    {
        //no-op
    }

};


struct PropertyEntityintegration : public Cyphesis::TestBase
{
    TypeNode* m_type;
    Ref<TestEntity> m_entity;

    PropertyEntityintegration()
    {
        ADD_TEST(PropertyEntityintegration::test_setAttrInt);
        ADD_TEST(PropertyEntityintegration::test_setAttrIntWithModifiers);
        ADD_TEST(PropertyEntityintegration::test_setAttrList);


        ADD_TEST(PropertyEntityintegration::test_requirePropertyClass<long>);
        ADD_TEST(PropertyEntityintegration::test_requirePropertyClass<double>);
        ADD_TEST(PropertyEntityintegration::test_requirePropertyClass<std::string>);
        ADD_TEST(PropertyEntityintegration::test_requirePropertyClass<MapType>);

        ADD_TEST(PropertyEntityintegration::test_requirePropertyClass_default<long>);
        ADD_TEST(PropertyEntityintegration::test_requirePropertyClass_default<double>);
        ADD_TEST(PropertyEntityintegration::test_requirePropertyClass_default<std::string>);
        ADD_TEST(PropertyEntityintegration::test_requirePropertyClass_default<MapType>);

        ADD_TEST(PropertyEntityintegration::test_modProperty<long>);
        ADD_TEST(PropertyEntityintegration::test_modProperty<double>);
        ADD_TEST(PropertyEntityintegration::test_modProperty<std::string>);
        ADD_TEST(PropertyEntityintegration::test_modProperty<MapType>);

        ADD_TEST(PropertyEntityintegration::test_modPropertyClass<long>);
        ADD_TEST(PropertyEntityintegration::test_modPropertyClass<double>);
        ADD_TEST(PropertyEntityintegration::test_modPropertyClass<std::string>);
        ADD_TEST(PropertyEntityintegration::test_modPropertyClass<MapType>);
    }


    void setup()
    {
        propertyManager = new TestPropertyManager;
        m_type = new TypeNode("test_type");
        m_type->addProperties(MapType{
                std::make_pair(test_values<long>::name,
                               test_values<long>::initial_value),
                std::make_pair(test_values<double>::name,
                               test_values<double>::initial_value),
                std::make_pair(test_values<std::string>::name,
                               test_values<std::string>::initial_value),
                std::make_pair(test_values<MapType>::name,
                               test_values<MapType>::initial_value)
        });
        m_entity = new TestEntity("1", 1L);
        m_entity->setType(m_type);
    }


    void teardown()
    {
        m_entity = nullptr;
        delete m_type;
        delete propertyManager;
    }

    void test_setAttrInt()
    {
        Atlas::Message::Element element;

        m_entity->setAttr("foo", 1);
        m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 1);

        m_entity->setAttr("foo", 2);
        m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 2);

        m_entity->setAttr("foo!append", 2);
        m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 4);

        m_entity->setAttr("foo!prepend", 2);
        m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 6);

        m_entity->setAttr("foo!subtract", 3);
        m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 3);
    }

    void test_setAttrIntWithModifiers()
    {
        Atlas::Message::Element element;

        AppendModifier appendModifier(1);

        m_entity->addModifier("foo", &appendModifier, m_entity.get());

        m_entity->setAttr("foo", 1);
        m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 2);


        AddFractionModifier addFractionModifier(2.0);
        m_entity->addModifier("foo", &addFractionModifier, m_entity.get());

        m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 4);


        m_entity->setAttr("foo", 6);
        m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 19);


        m_entity->removeModifier("foo", &addFractionModifier);
        m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, 7);
    }


    void test_setAttrList()
    {
        Atlas::Message::Element element;

        Atlas::Message::ListType list;
        list.emplace_back(1);
        m_entity->setAttr("foo", list);
        m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, list);

        list.emplace_back(2);

        m_entity->setAttr("foo", list);
        m_entity->getAttr("foo", element);
        ASSERT_EQUAL(element, list);

        list.clear();
        list.emplace_back(4);
        m_entity->setAttr("foo!append", list);
        m_entity->getAttr("foo", element);
        Atlas::Message::ListType correctList;
        correctList.emplace_back(1);
        correctList.emplace_back(2);
        correctList.emplace_back(4);
        ASSERT_EQUAL(element, correctList);

        list.clear();
        list.emplace_back(2);
        m_entity->setAttr("foo!prepend", list);
        m_entity->getAttr("foo", element);
        correctList.clear();
        correctList.emplace_back(2);
        correctList.emplace_back(1);
        correctList.emplace_back(2);
        correctList.emplace_back(4);
        ASSERT_EQUAL(element, correctList);


        list.clear();
        list.emplace_back(4);
        m_entity->setAttr("foo!subtract", list);
        m_entity->getAttr("foo", element);
        correctList.clear();
        correctList.emplace_back(2);
        correctList.emplace_back(1);
        correctList.emplace_back(2);
        ASSERT_EQUAL(element, correctList);
    }

    template<class T>
    void test_requirePropertyClass()
    {
        auto p = m_entity->requirePropertyClass<Property<T>>("bill");
        ASSERT_NOT_NULL(p);
    }

    template<class T>
    void test_requirePropertyClass_default()
    {
        auto p = m_entity->requirePropertyClass<Property<T>>("bill",
                                                             Element(test_values<T>::default_value));
        ASSERT_EQUAL(p->data(), test_values<T>::default_value);
    }

    template<class T>
    void test_modProperty()
    {
        // Get a pointer to the types default property
        auto& dflt = m_type->defaults().find(test_values<T>::name)->second;
        ASSERT_TRUE(dflt);
        ASSERT_TRUE(dflt->flags().m_flags & prop_flag_class);

        // The entity instance should not have a property by this name
        ASSERT_EQUAL(m_entity->getProperties().find(test_values<T>::name),
                     m_entity->getProperties().end());

        PropertyBase* p = m_entity->modProperty(test_values<T>::name);
        ASSERT_NOT_NULL(p);
        ASSERT_TRUE((p->flags().m_flags & prop_flag_class) == 0);
        // modProperty should have forced a new object
        ASSERT_NOT_EQUAL(p, dflt.get());

        auto subp = dynamic_cast<Property<T>*>(p);
        ASSERT_NOT_NULL(subp);
    }

    template<class T>
    void test_modPropertyClass()
    {
        // Get a pointer to the types default property
        auto& dflt = m_type->defaults().find(test_values<T>::name)->second;
        ASSERT_TRUE(dflt);
        ASSERT_TRUE(dflt->flags().m_flags & prop_flag_class);

        // The entity instance should not have a property by this name
        ASSERT_EQUAL(m_entity->getProperties().find(test_values<T>::name),
                     m_entity->getProperties().end());

        auto p = m_entity->modPropertyClass<Property<T>>(
                test_values<T>::name
        );
        ASSERT_NOT_NULL(p);
        ASSERT_TRUE((p->flags().m_flags & prop_flag_class) == 0);
        // modProperty should have forced a new object
        ASSERT_NOT_EQUAL(p, dflt.get());

        ASSERT_EQUAL(p->data(), test_values<T>::initial_value);
    }


    TestPropertyManager* propertyManager;
};


int main()
{
    PropertyEntityintegration t;

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
