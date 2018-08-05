// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "server/PossessionAuthenticator.h"

#include "server/PendingPossession.h"

#include "rulesets/BaseWorld.h"
#include "common/id.h"
#include "common/log.h"

#include "rulesets/Entity.h"

#include <Atlas/Objects/Operation.h>

#include <cstdlib>

#include <cassert>

using Atlas::Objects::Entity::RootEntity;

int stub_baseworld_receieved_op = -1;
int stub_connection_send_op = -1;
int stub_connection_send_count = 0;

#include "TestWorld.h"

class MyTestWorld : public TestWorld {
  public:
    explicit MyTestWorld() {
    }

    LocatedEntity * test_addEntity(LocatedEntity * ent, long intId) { 
        m_eobjects[intId] = ent;
        return ent;
    }
};

class TeleportAuthenticatortest : public Cyphesis::TestBase
{
  private:
    MyTestWorld * m_world;
    PossessionAuthenticator* m_possessionAuthenticator;
  public:
    TeleportAuthenticatortest();

    void setup();
    void teardown();

    void test_sequence();
    void test_authenticatePossession();
    void test_authenticatePossession_nonexist();
    void test_removePossession_id();
    void test_removePossession_iterator();
};

TeleportAuthenticatortest::TeleportAuthenticatortest()
{
    ADD_TEST(TeleportAuthenticatortest::test_sequence);
    ADD_TEST(TeleportAuthenticatortest::test_authenticatePossession);
    ADD_TEST(TeleportAuthenticatortest::test_authenticatePossession_nonexist);
    ADD_TEST(TeleportAuthenticatortest::test_removePossession_id);
    ADD_TEST(TeleportAuthenticatortest::test_removePossession_iterator);
}

void TeleportAuthenticatortest::setup()
{
    m_world = new MyTestWorld;
    m_possessionAuthenticator = new PossessionAuthenticator();
}

void TeleportAuthenticatortest::teardown()
{
    delete m_possessionAuthenticator;
    delete m_world;
}

void TeleportAuthenticatortest::test_sequence()
{
    // Check for correct singleton instancing

    // Test isPending() function
    assert(!PossessionAuthenticator::instance().isPending("test_non_existent_entity_id"));

    // Test adding of teleport entries
    assert(PossessionAuthenticator::instance().addPossession("test_entity_id", "test_possess_key") == 0);
    assert(PossessionAuthenticator::instance().isPending("test_entity_id"));
    assert(PossessionAuthenticator::instance().addPossession("test_entity_id", "test_possess_key") == -1);

    // Test removal of teleport entries
    assert(PossessionAuthenticator::instance().removePossession("test_non_existent_entity_id") == -1);
    assert(PossessionAuthenticator::instance().removePossession("test_entity_id") == 0);
    assert(!PossessionAuthenticator::instance().isPending("test_entity_id"));

}

void TeleportAuthenticatortest::test_authenticatePossession()
{
    Ref<Entity> ent(new Entity("100", 100));
    m_world->test_addEntity(ent, 100);

    PossessionAuthenticator::instance().addPossession("100", "test_possess_key");

    // Test non-existent ID authentication request
    assert(PossessionAuthenticator::instance().authenticatePossession(
               "101", "test_possess_key") == nullptr);

    // Test incorrect possess key authentication request
    assert(PossessionAuthenticator::instance().authenticatePossession("100",
                                        "test_wrong_possess_key") == nullptr);

    // Test valid authentication request
    assert(PossessionAuthenticator::instance().authenticatePossession("100",
                                        "test_possess_key") != nullptr);

}

void TeleportAuthenticatortest::test_authenticatePossession_nonexist()
{
    Ref<Entity> ent(new Entity("100", 100));
    m_world->test_addEntity(ent, 100);

    PossessionAuthenticator::instance().addPossession("101", "test_possess_key");

    // Test ID authentication request, that we added, for a non existant
    // entity
    assert(PossessionAuthenticator::instance().authenticatePossession(
                   "101", "test_possess_key") == nullptr);
}

void TeleportAuthenticatortest::test_removePossession_id()
{
    PossessionAuthenticator * ta = PossessionAuthenticator::instancePtr();

    ta->m_possessions.insert(std::make_pair("1",
          new PendingPossession("1", "e146db28-1058-46e6-a9b3-601ab6ef07a7")));

    ASSERT_TRUE(ta->m_possessions.find("1") != ta->m_possessions.end());

    int ret = ta->removePossession("1");
    ASSERT_EQUAL(ret, 0);

    ASSERT_TRUE(ta->m_possessions.find("1") == ta->m_possessions.end());
}

void TeleportAuthenticatortest::test_removePossession_iterator()
{
    PossessionAuthenticator * ta = PossessionAuthenticator::instancePtr();

    ta->m_possessions.insert(std::make_pair("2",
          new PendingPossession("2", "b769b7a4-32d3-477d-9803-a53fd9ad49c7")));

    ASSERT_TRUE(ta->m_possessions.find("2") != ta->m_possessions.end());

    PendingPossessionsMap::iterator I = ta->m_possessions.find("2");

    ta->removePossession(I);

    ASSERT_TRUE(ta->m_possessions.find("2") == ta->m_possessions.end());
}

int main()
{
    TeleportAuthenticatortest t;

    return t.run();
}

// Stubs

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

void log(LogLevel lvl, const std::string & msg)
{
}

#ifndef STUB_BaseWorld_getEntity
#define STUB_BaseWorld_getEntity
LocatedEntity* BaseWorld::getEntity(const std::string & id) const
{
    return getEntity(integerId(id));
}

LocatedEntity* BaseWorld::getEntity(long id) const
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

#include "stubs/rulesets/stubBaseWorld.h"
PendingPossession::PendingPossession(const std::string &id, const std::string &key)
                                            :   m_entity_id(id),
                                                m_possess_key(key),
                                                m_valid(false)
{
}

bool PendingPossession::validate(const std::string &entity_id,
                               const std::string &possess_key) const
{
    if(m_entity_id == entity_id && m_possess_key == possess_key) {
        return true;
    } else {
        return false;
    }
}

void PendingPossession::setValidated()
{
    m_valid = true;
}

#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/rulesets/stubLocation.h"
#include "stubs/common/stubRouter.h"

