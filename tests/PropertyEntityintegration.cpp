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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"
#include "TestWorld.h"

#include "rulesets/Entity.h"

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
std::ostream & operator<<(std::ostream & os, const MapType & v)
{
    os << "[ATLAS_MAP]";
    return os;
}

std::ostream & operator<<(std::ostream & os,
                          const PropertyDict::const_iterator &)
{
    os << "[iterator]";
    return os;
}

template<typename T>
class test_values
{
  public:
    static const char * name;
    static T initial_value;
    static T default_value;
};

template<>
const char * test_values<long>::name = "test_int";

template<>
long test_values<long>::initial_value = 42;

template<>
long test_values<long>::default_value = 23;

template<>
const char * test_values<double>::name = "test_float";

template<>
double test_values<double>::initial_value = 69.5;

template<>
double test_values<double>::default_value = 17.5;

template<>
const char * test_values<std::string>::name = "test_string";

template<>
std::string test_values<std::string>::initial_value =
      "1356ebe4-220d-46a7-8c69-73f787f6b1ff";

template<>
std::string test_values<std::string>::default_value =
      "1423a858-f7c6-4b3c-8b0b-726af6dcbeff";

template<>
const char * test_values<MapType>::name = "test_map";

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

    virtual PropertyBase * addProperty(const std::string & name, int type);
};

TestPropertyManager::~TestPropertyManager()
{
}

PropertyBase * TestPropertyManager::addProperty(const std::string & name,
                                                int type)
{
    if (name == test_values<long>::name) {
        return new Property<long>;
    } else if (name == test_values<double>::name) {
        return new Property<double>;
    } else if (name == test_values<std::string>::name) {
        return new Property<std::string>;
    } else if (name == test_values<MapType>::name) {
        return new Property<MapType>;
    } else {
        return new SoftProperty;
    }
}

class PropertyEntityintegration : public Cyphesis::TestBase
{
  private:
    TypeNode * m_type;
    Entity * m_entity;
  public:
    PropertyEntityintegration();

    void setup();
    void teardown();

    template<class T>
    void test_requirePropertyClass();

    template<class T>
    void test_requirePropertyClass_default();

    template<class T>
    void test_modProperty();

    template<class T>
    void test_modPropertyClass();
};

template<class T>
void PropertyEntityintegration::test_requirePropertyClass()
{
    auto p = m_entity->requirePropertyClass<Property<T>>("bill");
    ASSERT_NOT_NULL(p);
}

template<class T>
void PropertyEntityintegration::test_requirePropertyClass_default()
{
    auto p = m_entity->requirePropertyClass<Property<T>>("bill",
          Element(test_values<T>::default_value));
    ASSERT_EQUAL(p->data(), test_values<T>::default_value);
}

template<class T>
void PropertyEntityintegration::test_modProperty()
{
    // Get a pointer to the types default property
    PropertyBase * dflt = m_type->defaults().find(test_values<T>::name)->second;
    ASSERT_NOT_NULL(dflt);
    ASSERT_TRUE(dflt->flags() & flag_class);

    // The entity instance should not have a property by this name
    ASSERT_EQUAL(m_entity->m_properties.find(test_values<T>::name),
                 m_entity->m_properties.end());

    PropertyBase * p = m_entity->modProperty(test_values<T>::name);
    ASSERT_NOT_NULL(p);
    ASSERT_TRUE((p->flags() & flag_class) == 0);
    // modProperty should have forced a new object
    ASSERT_NOT_EQUAL(p, dflt);

    auto subp = dynamic_cast<Property<T> *>(p);
    ASSERT_NOT_NULL(subp);
}

template<class T>
void PropertyEntityintegration::test_modPropertyClass()
{
    // Get a pointer to the types default property
    PropertyBase * dflt = m_type->defaults().find(test_values<T>::name)->second;
    ASSERT_NOT_NULL(dflt);
    ASSERT_TRUE(dflt->flags() & flag_class);

    // The entity instance should not have a property by this name
    ASSERT_EQUAL(m_entity->m_properties.find(test_values<T>::name),
                 m_entity->m_properties.end());

    auto p = m_entity->modPropertyClass<Property<T>>(
        test_values<T>::name
    );
    ASSERT_NOT_NULL(p);
    ASSERT_TRUE((p->flags() & flag_class) == 0);
    // modProperty should have forced a new object
    ASSERT_NOT_EQUAL(p, dflt);
    
    ASSERT_EQUAL(p->data(), test_values<T>::initial_value);
}

PropertyEntityintegration::PropertyEntityintegration()
{
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

void PropertyEntityintegration::setup()
{
    new TestPropertyManager;
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
    m_entity = new Entity("1", 1L);
    m_entity->setType(m_type);
}

void PropertyEntityintegration::teardown()
{
    delete m_entity;
    delete m_type;
    delete PropertyManager::instance();
}

int main()
{
    PropertyEntityintegration t;

    return t.run();
}

// stubs

#include "rulesets/AtlasProperties.h"
#include "rulesets/Domain.h"
#include "rulesets/Script.h"

#include "common/id.h"

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}

namespace Atlas { namespace Objects { namespace Operation {
int ACTUATE_NO = -1;
int ATTACK_NO = -1;
int EAT_NO = -1;
int NOURISH_NO = -1;
int TICK_NO = -1;
int UPDATE_NO = -1;
} } }

Domain * Domain::m_instance = new Domain();

Domain::Domain() : m_refCount(0)
{
}

Domain::~Domain()
{
}

float Domain::constrainHeight(LocatedEntity * parent,
                              const Point3D & pos,
                              const std::string & mode)
{
    return 0.f;
}

void Domain::tick(double t)
{
}

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}

Router::Router(const std::string & id, long intId) : m_id(id),
                                                             m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    long intId = integerId(id);

    EntityDict::const_iterator I = m_eobjects.find(intId);
    if (I != m_eobjects.end()) {
        assert(I->second != 0);
        return I->second;
    } else {
        return 0;
    }
}

LocatedEntity * BaseWorld::getEntity(long id) const
{
    EntityDict::const_iterator I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second != 0);
        return I->second;
    } else {
        return 0;
    }
}

Script::Script()
{
}

/// \brief Script destructor
Script::~Script()
{
}

bool Script::operation(const std::string & opname,
                       const Atlas::Objects::Operation::RootOperation & op,
                       OpVector & res)
{
   return false;
}

void Script::hook(const std::string & function, LocatedEntity * entity)
{
}

void Location::addToMessage(MapType & omap) const
{
}

Location::Location() : m_loc(0)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

IdProperty::IdProperty(const std::string & data) : PropertyBase(per_ephem),
                                                   m_data(data)
{
}

int IdProperty::get(Atlas::Message::Element & e) const
{
    return 0;
}

void IdProperty::set(const Atlas::Message::Element & e)
{
}

void IdProperty::add(const std::string & key,
                     Atlas::Message::MapType & ent) const
{
}

void IdProperty::add(const std::string & key,
                     const Atlas::Objects::Entity::RootEntity & ent) const
{
}

IdProperty * IdProperty::copy() const
{
    return 0;
}

ContainsProperty::ContainsProperty(LocatedEntitySet & data) :
      PropertyBase(per_ephem), m_data(data)
{
}

int ContainsProperty::get(Atlas::Message::Element & e) const
{
    return 0;
}

void ContainsProperty::set(const Atlas::Message::Element & e)
{
}

void ContainsProperty::add(const std::string & s,
                           const Atlas::Objects::Entity::RootEntity & ent) const
{
}

ContainsProperty * ContainsProperty::copy() const
{
    return 0;
}

PropertyManager * PropertyManager::m_instance = 0;

PropertyManager::PropertyManager()
{
    assert(m_instance == 0);
    m_instance = this;
}

PropertyManager::~PropertyManager()
{
   m_instance = 0;
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}
