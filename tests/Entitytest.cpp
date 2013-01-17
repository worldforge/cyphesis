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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"
#include "IGEntityExerciser.h"
#include "allOperations.h"

#include "rulesets/Entity.h"

#include "rulesets/AtlasProperties.h"
#include "rulesets/Domain.h"
#include "rulesets/Script.h"

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
    Entity * m_entity;

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
        virtual void install(LocatedEntity *);
        virtual void apply(LocatedEntity *);
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

void Entitytest::TestProperty::install(LocatedEntity *)
{
    Entitytest::TestProperty_install_called();
}

void Entitytest::TestProperty::apply(LocatedEntity *)
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
    m_pm->installPropertyFactory("test_int_property", new PropertyFactory<TestProperty>);
    m_type = new TypeNode("test_type");
    m_entity = new Entity("1", 1);
    m_entity->setType(m_type);

    m_TestProperty_install_called = false;
}

void Entitytest::teardown()
{
    delete m_entity;
    delete m_type;
    delete m_pm;
}

void Entitytest::test_setAttr_new()
{
    ASSERT_TRUE(!m_TestProperty_install_called);

    PropertyBase * pb = m_entity->setAttr("test_int_property", 24);
    ASSERT_NOT_NULL(pb);

    ASSERT_TRUE((pb->flags() & flag_class) == 0);

    auto * int_property = dynamic_cast<TestProperty *>(pb);
    ASSERT_NOT_NULL(int_property);
    ASSERT_EQUAL(int_property->data(), 24);
    ASSERT_TRUE(m_TestProperty_install_called);
    ASSERT_TRUE(m_TestProperty_apply_called);
}

void Entitytest::test_setAttr_existing()
{
    PropertyBase * initial_property = m_entity->setProperty("test_int_property",
                                                            new TestProperty);

    PropertyBase * pb = m_entity->setAttr("test_int_property", 24);
    ASSERT_NOT_NULL(pb);
    ASSERT_EQUAL(initial_property, pb);

    ASSERT_TRUE((pb->flags() & flag_class) == 0);

    auto * int_property = dynamic_cast<TestProperty *>(pb);
    ASSERT_NOT_NULL(int_property);
    ASSERT_EQUAL(int_property->data(), 24);
    ASSERT_TRUE(!m_TestProperty_install_called);
    ASSERT_TRUE(m_TestProperty_apply_called);
}

void Entitytest::test_setAttr_type()
{
    TestProperty * type_property = new TestProperty;
    type_property->data() = 17;
    type_property->flags() &= flag_class;
    m_type->addProperty("test_int_property", type_property);

    PropertyBase * pb = m_entity->setAttr("test_int_property", 24);
    ASSERT_NOT_NULL(pb);
    ASSERT_NOT_EQUAL(type_property, pb);

    ASSERT_TRUE((pb->flags() & flag_class) == 0);

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

    IGEntityExerciser ee(*m_entity);

    // Throw an op of every type at the entity
    ee.runOperations();

    // Subscribe the entity to every class of op
    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    // Add the test attributes
    m_entity->setAttr("test_int", 1);
    m_entity->setAttr("test_float", 1.f);
    m_entity->setAttr("test_list_string", "test_value");
    m_entity->setAttr("test_list_int", ListType(1, 1));
    m_entity->setAttr("test_list_float", ListType(1, 1.f));
    m_entity->setAttr("test_map_string", ListType(1, "test_value"));
    MapType test_map;
    test_map["test_key"] = 1;
    m_entity->setAttr("test_map_int", test_map);
    test_map["test_key"] = 1.f;
    m_entity->setAttr("test_map_float", test_map);
    test_map["test_key"] = "test_value";
    m_entity->setAttr("test_map_string", test_map);
    
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
        m_entity->setProperty("new_test_prop", new SoftProperty);
    }
}

int main()
{
    Entitytest t;

    return t.run();
}

// stubs

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
int SETUP_NO = -1;
int TICK_NO = -1;
int UPDATE_NO = -1;
} } }

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

int LocatedEntity::getAttr(const std::string & name,
                           Atlas::Message::Element & attr) const
{
    return -1;
}

int LocatedEntity::getAttrType(const std::string & name,
                               Atlas::Message::Element & attr,
                               int type) const
{
    return -1;
}

PropertyBase * LocatedEntity::setAttr(const std::string & name,
                                      const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * LocatedEntity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * LocatedEntity::setProperty(const std::string & name,
                                          PropertyBase * prop)
{
    return 0;
}

void LocatedEntity::installHandler(int, Handler)
{
}

void LocatedEntity::installDelegate(int, const std::string &)
{
}

void LocatedEntity::destroy()
{
}

Domain * LocatedEntity::getMovementDomain()
{
    return 0;
}

void LocatedEntity::sendWorld(const Operation & op)
{
}

void LocatedEntity::onContainered()
{
}

void LocatedEntity::onUpdated()
{
}

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
}

void LocatedEntity::merge(const MapType & ent)
{
}

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

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

TypeNode::~TypeNode()
{
    PropertyDict::const_iterator I = m_defaults.begin();
    PropertyDict::const_iterator Iend = m_defaults.end();
    for (; I != Iend; ++I) {
        delete I->second;
    }
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

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(LocatedEntity *)
{
}

void PropertyBase::apply(LocatedEntity *)
{
}

void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
    get(ent[s]);
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}

HandlerResult PropertyBase::operation(LocatedEntity *,
                                      const Operation &,
                                      OpVector &)
{
    return OPERATION_IGNORED;
}

template<>
void Property<int>::set(const Atlas::Message::Element & e)
{
    if (e.isInt()) {
        this->m_data = e.asInt();
    }
}

template<>
void Property<double>::set(const Atlas::Message::Element & e)
{
    if (e.isNum()) {
        this->m_data = e.asNum();
    }
}

template<>
void Property<std::string>::set(const Atlas::Message::Element & e)
{
    if (e.isString()) {
        this->m_data = e.String();
    }
}

template class Property<int>;
template class Property<double>;
template class Property<std::string>;

SoftProperty::SoftProperty()
{
}

SoftProperty::SoftProperty(const Atlas::Message::Element & data) :
              PropertyBase(0), m_data(data)
{
}

int SoftProperty::get(Atlas::Message::Element & val) const
{
    val = m_data;
    return 0;
}

void SoftProperty::set(const Atlas::Message::Element & val)
{
}

SoftProperty * SoftProperty::copy() const
{
    return new SoftProperty(*this);
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

PropertyKit::~PropertyKit()
{
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
