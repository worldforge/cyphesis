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

#include "rulesets/MemMap.h"

#include "rulesets/MemEntity.h"
#include "rulesets/Script.h"

#include "common/Inheritance.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cstdlib>

#include <cassert>

using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Root;

class MemMaptest : public Cyphesis::TestBase
{
  private:
    TypeNode * m_sampleType;
    Script * m_script;
    MemMap * m_memMap;

    static std::string m_Script_hook_called;
    static LocatedEntity * m_Script_hook_called_with;
  public:
    MemMaptest();

    void setup();
    void teardown();

    void test_addId();
    void test_sendLooks();
    void test_del();
    void test_addEntity();
    void test_addEntity_script();
    void test_addEntity_script_hook();
    void test_readEntity();
    void test_readEntity_type();
    void test_readEntity_type_nonexist();
    void test_addEntityMemory();
    void test_recallEntityMemory();
    void test_getEntityRelatedMemory();
    void test_findByLoc();
    void test_findByLoc_results();
    void test_findByLoc_invalid();
    void test_findByLoc_consistency_check();

    static void Script_hook_called(const std::string &, LocatedEntity *);
};

std::string MemMaptest::m_Script_hook_called;
LocatedEntity * MemMaptest::m_Script_hook_called_with = 0;

void MemMaptest::Script_hook_called(const std::string & hook,
                                    LocatedEntity * ent)
{
    m_Script_hook_called = hook;
    m_Script_hook_called_with = ent;
}

class TestScript : public Script
{
  public:
    virtual void hook(const std::string & function, LocatedEntity * entity);
};

void TestScript::hook(const std::string & function, LocatedEntity * entity)
{
    MemMaptest::Script_hook_called(function, entity);
}

MemMaptest::MemMaptest()
{
    ADD_TEST(MemMaptest::test_addId);
    ADD_TEST(MemMaptest::test_sendLooks);
    ADD_TEST(MemMaptest::test_del);
    ADD_TEST(MemMaptest::test_addEntity);
    ADD_TEST(MemMaptest::test_addEntity_script);
    ADD_TEST(MemMaptest::test_addEntity_script_hook);
    ADD_TEST(MemMaptest::test_readEntity);
    ADD_TEST(MemMaptest::test_readEntity_type);
    ADD_TEST(MemMaptest::test_readEntity_type_nonexist);
    ADD_TEST(MemMaptest::test_addEntityMemory);
    ADD_TEST(MemMaptest::test_recallEntityMemory);
    ADD_TEST(MemMaptest::test_getEntityRelatedMemory)
    ADD_TEST(MemMaptest::test_findByLoc);
    ADD_TEST(MemMaptest::test_findByLoc_results);
    ADD_TEST(MemMaptest::test_findByLoc_invalid);
    ADD_TEST(MemMaptest::test_findByLoc_consistency_check);
}

void MemMaptest::setup()
{
    m_Script_hook_called = "";
    m_Script_hook_called_with = 0;

    Root type_desc;
    type_desc->setId("sample_type");
    m_sampleType = Inheritance::instance().addChild(type_desc);

    m_script = 0;
    m_memMap = new MemMap(m_script);
}

void MemMaptest::teardown()
{
    delete m_memMap;
    Inheritance::clear();
}

void MemMaptest::test_addId()
{
    m_memMap->addId("2", 2);
}

void MemMaptest::test_sendLooks()
{
    OpVector res;
    m_memMap->sendLooks(res);
}

void MemMaptest::test_del()
{
    m_memMap->del("2");
}

void MemMaptest::test_addEntity()
{
    const std::string new_id("3");
    ASSERT_NULL(m_memMap->get(new_id));

    MemEntity * ent = new MemEntity(new_id, 3);
    ent->setType(MemMap::m_entity_type);
    m_memMap->addEntity(ent);

    ASSERT_NOT_NULL(m_memMap->get(new_id));
    ASSERT_NULL(m_Script_hook_called_with);
}

void MemMaptest::test_addEntity_script()
{
    m_script = new TestScript;
    const std::string new_id("3");
    ASSERT_NULL(m_memMap->get(new_id));

    MemEntity * ent = new MemEntity(new_id, 3);
    ent->setType(MemMap::m_entity_type);
    m_memMap->addEntity(ent);

    ASSERT_NOT_NULL(m_memMap->get(new_id));
    ASSERT_NULL(m_Script_hook_called_with);
}

void MemMaptest::test_addEntity_script_hook()
{
    const std::string new_id("3");
    const std::string test_add_hook_name("test_add_hook");

    m_script = new TestScript;
    m_memMap->m_addHooks.push_back(test_add_hook_name);

    ASSERT_NULL(m_memMap->get(new_id));

    MemEntity * ent = new MemEntity(new_id, 3);
    ent->setType(MemMap::m_entity_type);
    m_memMap->addEntity(ent);

    ASSERT_NOT_NULL(m_memMap->get(new_id));
    ASSERT_NOT_NULL(m_Script_hook_called_with);
    ASSERT_EQUAL(m_Script_hook_called, test_add_hook_name);
}

void MemMaptest::test_readEntity()
{
    const std::string new_id("3");

    Anonymous data;

    MemEntity * ent = new MemEntity(new_id, 3);
    ent->setType(MemMap::m_entity_type);

    m_memMap->readEntity(ent, data);
}

void MemMaptest::test_readEntity_type()
{
    const std::string new_id("3");

    Anonymous data;
    data->setParents(std::list<std::string>(1, "sample_type"));

    MemEntity * ent = new MemEntity(new_id, 3);
    ent->setType(MemMap::m_entity_type);

    m_memMap->readEntity(ent, data);

    ASSERT_NOT_EQUAL(ent->getType(), MemMap::m_entity_type);
    ASSERT_EQUAL(ent->getType(), m_sampleType);
}

void MemMaptest::test_readEntity_type_nonexist()
{
    const std::string new_id("3");

    Anonymous data;
    data->setParents(std::list<std::string>(1, "non_sample_type"));

    MemEntity * ent = new MemEntity(new_id, 3);
    ent->setType(MemMap::m_entity_type);

    m_memMap->readEntity(ent, data);

    ASSERT_EQUAL(ent->getType(), MemMap::m_entity_type);
    ASSERT_NOT_EQUAL(ent->getType(), m_sampleType);
}

void MemMaptest::test_addEntityMemory(){
    using Atlas::Message::Element;

    //The entities that we have memories about don't actually have to exist

    //Add some new memories
    m_memMap->addEntityMemory("1", "disposition", 25);
    m_memMap->addEntityMemory("1", "have_met", true);

    //Check if they were added properly
    assert(m_memMap->m_entityRelatedMemory["1"]["disposition"] == Element(25));
    assert(m_memMap->m_entityRelatedMemory["1"]["have_met"] == Element(true));

    //update an existing memory
    m_memMap->addEntityMemory("1", "disposition", 30);
    assert(m_memMap->m_entityRelatedMemory["1"]["disposition"] == Element(30));
}

void MemMaptest::test_recallEntityMemory(){
    using Atlas::Message::Element;

    //set up a map with 1 memory: disposition with value 25 related to entity with id 1
    std::map<std::string, std::map<std::string, Element>> memories{{"1", std::map<std::string, Element>{{"disposition", 25}}}};
    Element val1, val2;
    m_memMap->m_entityRelatedMemory = memories;

    //try recalling an existing memory
    m_memMap->recallEntityMemory("1", "disposition", val1);
    assert(val1 == Element(25));

    //try recalling a non-existing memory about known entity
    m_memMap->recallEntityMemory("1", "foo", val2);
    assert(val2.isNone());
    //try recalling about an unknown entity
    m_memMap->recallEntityMemory("2", "disposition", val2);
    assert(val2.isNone());
}

void MemMaptest::test_getEntityRelatedMemory(){
    using Atlas::Message::Element;

    std::map<std::string, std::map<std::string, Element>> memories{{"1", std::map<std::string, Element>{{"disposition", 25}}}};
    m_memMap->m_entityRelatedMemory = memories;
    assert(m_memMap->getEntityRelatedMemory() == memories);
}

void MemMaptest::test_findByLoc()
{
    MemEntity * tlve = new MemEntity("3", 3);
    tlve->setVisible();
    m_memMap->m_entities[3] = tlve;
    tlve->m_contains = new LocatedEntitySet;

    MemEntity * e4 = new MemEntity("4", 4);
    e4->setVisible();
    e4->setType(m_sampleType);
    m_memMap->m_entities[4] = tlve;
    e4->m_location.m_loc = tlve;
    e4->m_location.m_pos = Point3D(1,1,0);
    tlve->m_contains->insert(e4);

    MemEntity * e5 = new MemEntity("5", 5);
    e5->setVisible();
    e5->setType(m_sampleType);
    m_memMap->m_entities[5] = tlve;
    e5->m_location.m_loc = tlve;
    e5->m_location.m_pos = Point3D(2,2,0);
    tlve->m_contains->insert(e5);

    Location find_here(tlve);

    // Radius too small
    EntityVector res = m_memMap->findByLocation(find_here,
                                                1.f,
                                                "sample_type");

    ASSERT_TRUE(res.empty());
}

void MemMaptest::test_findByLoc_results()
{
    MemEntity * tlve = new MemEntity("3", 3);
    tlve->setVisible();
    m_memMap->m_entities[3] = tlve;
    tlve->m_contains = new LocatedEntitySet;

    MemEntity * e4 = new MemEntity("4", 4);
    e4->setVisible();
    e4->setType(m_sampleType);
    m_memMap->m_entities[4] = tlve;
    e4->m_location.m_loc = tlve;
    e4->m_location.m_pos = Point3D(1,1,0);
    tlve->m_contains->insert(e4);

    MemEntity * e5 = new MemEntity("5", 5);
    e5->setVisible();
    e5->setType(m_sampleType);
    m_memMap->m_entities[5] = tlve;
    e5->m_location.m_loc = tlve;
    e5->m_location.m_pos = Point3D(2,2,0);
    tlve->m_contains->insert(e5);

    Location find_here(tlve);

    EntityVector res = m_memMap->findByLocation(find_here,
                                                5.f,
                                                "sample_type");

    ASSERT_TRUE(!res.empty());
    ASSERT_EQUAL(res.size(), 2u);
}

void MemMaptest::test_findByLoc_invalid()
{
    MemEntity * tlve = new MemEntity("3", 3);
    tlve->setVisible();
    m_memMap->m_entities[3] = tlve;
    tlve->m_contains = new LocatedEntitySet;

    MemEntity * e4 = new MemEntity("4", 4);
    e4->setVisible();
    e4->setType(m_sampleType);
    m_memMap->m_entities[4] = tlve;
    e4->m_location.m_loc = tlve;
    e4->m_location.m_pos = Point3D(1,1,0);
    tlve->m_contains->insert(e4);

    MemEntity * e5 = new MemEntity("5", 5);
    e5->setVisible();
    e5->setType(m_sampleType);
    m_memMap->m_entities[5] = tlve;
    e5->m_location.m_loc = tlve;
    e5->m_location.m_pos = Point3D(2,2,0);
    tlve->m_contains->insert(e5);

    // Look in a location where these is nothing - no contains at all
    Location find_here(e4);

    EntityVector res = m_memMap->findByLocation(find_here,
                                                5.f,
                                                "sample_type");

    ASSERT_TRUE(res.empty());
}

void MemMaptest::test_findByLoc_consistency_check()
{
    MemEntity * tlve = new MemEntity("3", 3);
    tlve->setVisible();
    tlve->setType(m_sampleType);
    m_memMap->m_entities[3] = tlve;
    tlve->m_contains = new LocatedEntitySet;

    MemEntity * e4 = new MemEntity("4", 4);
    e4->setVisible();
    e4->setType(m_sampleType);
    m_memMap->m_entities[4] = tlve;
    e4->m_location.m_loc = tlve;
    e4->m_location.m_pos = Point3D(1,1,0);
    tlve->m_contains->insert(e4);

    MemEntity * e5 = new MemEntity("5", 5);
    e5->setVisible();
    e5->setType(m_sampleType);
    m_memMap->m_entities[5] = tlve;
    e5->m_location.m_loc = tlve;
    e5->m_location.m_pos = Point3D(2,2,0);
    tlve->m_contains->insert(e5);

    // Duplicated of tlve. Same ID, but not the same entity as in
    // memmap. This will fail, but via a different path depending on
    // DEBUG/NDEBUG. In debug build, the check in findByLoc will fail
    // resulting in early return. In ndebug build, it will return empty
    // by a longer path, as e3_dup contains no other entities.
    MemEntity * e3_dup = new MemEntity("3", 3);
    e3_dup->setType(m_sampleType);
    e3_dup->m_contains = new LocatedEntitySet;

    Location find_here(e3_dup);

    EntityVector res = m_memMap->findByLocation(find_here,
                                                5.f,
                                                "sample_type");

    ASSERT_TRUE(res.empty());
}

int main()
{
    MemMaptest t;

    return t.run();
}

// stubs

#include "stubs/rulesets/stubMemEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/common/stubRouter.h"
#include "stubs/modules/stubLocation.h"

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance() : noClass(0)
{
}

const TypeNode * Inheritance::getType(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second;
}

#include "stubs/rulesets/stubScript.h"

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
    }
    return *m_instance;
}

TypeNode * Inheritance::addChild(const Root & obj)
{
    const std::string & child = obj->getId();

    TypeNode * type = new TypeNode(child);

    atlasObjects[child] = type;

    return type;
}

void Inheritance::clear()
{
    if (m_instance != NULL) {
        delete m_instance;
        m_instance = NULL;
    }
}

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

float squareDistance(const Point3D & u, const Point3D & v)
{
    return 1.f;
}

void log(LogLevel lvl, const std::string & msg)
{
    std::cout << msg << std::endl;
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}


