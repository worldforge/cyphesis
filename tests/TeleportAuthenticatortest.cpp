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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"

#include "server/TeleportAuthenticator.h"

#include "server/PendingTeleport.h"

#include "common/BaseWorld.h"
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

class TestWorld : public BaseWorld {
  public:
    explicit TestWorld() : BaseWorld(*(LocatedEntity*)0) {
        m_realTime = 100000;
    }

    virtual bool idle(const SystemTime &) { return false; }

    virtual LocatedEntity * addEntity(LocatedEntity * ent) { 
        return 0;
    }

    LocatedEntity * test_addEntity(LocatedEntity * ent, long intId) { 
        m_eobjects[intId] = ent;
        return 0;
    }
    virtual LocatedEntity * addNewEntity(const std::string &,
                                         const Atlas::Objects::Entity::RootEntity &) {
        return 0;
    }
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         LocatedEntity *) { return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    LocatedEntity * spawnNewEntity(const std::string & name,
                                   const std::string & type,
                                   const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
    virtual Task * newTask(const std::string &, LocatedEntity &) { return 0; }
    virtual Task * activateTask(const std::string &, const std::string &,
                                LocatedEntity *, LocatedEntity &) { return 0; }
    virtual ArithmeticScript * newArithmetic(const std::string &,
                                             LocatedEntity *) {
        return 0;
    }
    virtual void message(const Operation & op, LocatedEntity & ent) {
        stub_baseworld_receieved_op = op->getClassNo();
    }
    virtual LocatedEntity * findByName(const std::string & name) { return 0; }
    virtual LocatedEntity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(LocatedEntity *) { }
};

class TeleportAuthenticatortest : public Cyphesis::TestBase
{
  private:
    TestWorld * m_world;
  public:
    TeleportAuthenticatortest();

    void setup();
    void teardown();

    void test_sequence();
    void test_authenticateTeleport();
    void test_authenticateTeleport_nonexist();
    void test_removeTeleport();
};

TeleportAuthenticatortest::TeleportAuthenticatortest()
{
    ADD_TEST(TeleportAuthenticatortest::test_sequence);
    ADD_TEST(TeleportAuthenticatortest::test_authenticateTeleport);
    ADD_TEST(TeleportAuthenticatortest::test_authenticateTeleport_nonexist);
    ADD_TEST(TeleportAuthenticatortest::test_removeTeleport);
}

void TeleportAuthenticatortest::setup()
{
    m_world = new TestWorld;

    assert(TeleportAuthenticator::instance() == NULL);
    TeleportAuthenticator::init();
    assert(TeleportAuthenticator::instance() != NULL);
}

void TeleportAuthenticatortest::teardown()
{
    TeleportAuthenticator::del();
    assert(TeleportAuthenticator::instance() == NULL);

    delete m_world;
}

void TeleportAuthenticatortest::test_sequence()
{
    // Check for correct singleton instancing
    assert(TeleportAuthenticator::instance() != NULL);
    
    // Test isPending() function
    assert(!TeleportAuthenticator::instance()->isPending("test_non_existent_entity_id"));

    // Test adding of teleport entries
    assert(TeleportAuthenticator::instance()->addTeleport("test_entity_id", "test_possess_key") == 0);
    assert(TeleportAuthenticator::instance()->isPending("test_entity_id"));
    assert(TeleportAuthenticator::instance()->addTeleport("test_entity_id", "test_possess_key") == -1);

    // Test removal of teleport entries
    assert(TeleportAuthenticator::instance()->removeTeleport("test_non_existent_entity_id") == -1);
    assert(TeleportAuthenticator::instance()->removeTeleport("test_entity_id") == 0);
    assert(!TeleportAuthenticator::instance()->isPending("test_entity_id"));

}

void TeleportAuthenticatortest::test_authenticateTeleport()
{
    Entity ent("100", 100);
    m_world->test_addEntity(&ent, 100);
    assert(TeleportAuthenticator::instance() != NULL);

    TeleportAuthenticator::instance()->addTeleport("100", "test_possess_key");

    // Test non-existent ID authentication request
    assert(TeleportAuthenticator::instance()->authenticateTeleport(
               "101", "test_possess_key") == NULL);

    // Test incorrect possess key authentication request
    assert(TeleportAuthenticator::instance()->authenticateTeleport("100",
                                        "test_wrong_possess_key") == NULL);

    // Test valid authentication request
    assert(TeleportAuthenticator::instance()->authenticateTeleport("100",
                                        "test_possess_key") != NULL);

}

void TeleportAuthenticatortest::test_authenticateTeleport_nonexist()
{
    Entity ent("100", 100);
    m_world->test_addEntity(&ent, 100);
    assert(TeleportAuthenticator::instance() != NULL);

    TeleportAuthenticator::instance()->addTeleport("101", "test_possess_key");

    // Test ID authentication request, that we added, for a non existant
    // entity
    assert(TeleportAuthenticator::instance()->authenticateTeleport(
                   "101", "test_possess_key") == NULL);
}

void TeleportAuthenticatortest::test_removeTeleport()
{
    int ret = TeleportAuthenticator::instance()->removeTeleport(
          TeleportAuthenticator::instance()->m_teleports.end());
    ASSERT_EQUAL(ret, -1);
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

PendingTeleport::PendingTeleport(const std::string &id, const std::string &key) 
                                            :   m_entity_id(id),
                                                m_possess_key(key),
                                                m_valid(false)
{
}

bool PendingTeleport::validate(const std::string &entity_id,
                               const std::string &possess_key) const
{
    if(m_entity_id == entity_id && m_possess_key == possess_key) {
        return true;
    } else {
        return false;
    }
}

void PendingTeleport::setValidated()
{
    m_valid = true;
}

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId), m_motion(0)
{
}

Entity::~Entity()
{
}

void Entity::ActuateOperation(const Operation &, OpVector &)
{
}

void Entity::AppearanceOperation(const Operation &, OpVector &)
{
}

void Entity::AttackOperation(const Operation &, OpVector &)
{
}

void Entity::CombineOperation(const Operation &, OpVector &)
{
}

void Entity::CreateOperation(const Operation &, OpVector &)
{
}

void Entity::DeleteOperation(const Operation &, OpVector &)
{
}

void Entity::DisappearanceOperation(const Operation &, OpVector &)
{
}

void Entity::DivideOperation(const Operation &, OpVector &)
{
}

void Entity::EatOperation(const Operation &, OpVector &)
{
}

void Entity::GetOperation(const Operation &, OpVector &)
{
}

void Entity::InfoOperation(const Operation &, OpVector &)
{
}

void Entity::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Entity::LookOperation(const Operation &, OpVector &)
{
}

void Entity::MoveOperation(const Operation &, OpVector &)
{
}

void Entity::NourishOperation(const Operation &, OpVector &)
{
}

void Entity::SetOperation(const Operation &, OpVector &)
{
}

void Entity::SightOperation(const Operation &, OpVector &)
{
}

void Entity::SoundOperation(const Operation &, OpVector &)
{
}

void Entity::TalkOperation(const Operation &, OpVector &)
{
}

void Entity::TickOperation(const Operation &, OpVector &)
{
}

void Entity::TouchOperation(const Operation &, OpVector &)
{
}

void Entity::UpdateOperation(const Operation &, OpVector &)
{
}

void Entity::UseOperation(const Operation &, OpVector &)
{
}

void Entity::WieldOperation(const Operation &, OpVector &)
{
}

void Entity::externalOperation(const Operation & op, Link &)
{
}

void Entity::operation(const Operation & op, OpVector & res)
{
}

void Entity::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Entity::addToEntity(const RootEntity & ent) const
{
}

PropertyBase * Entity::setAttr(const std::string & name,
                               const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return 0;
}

void Entity::installHandler(int class_no, Handler handler)
{
}

void Entity::installDelegate(int class_no, const std::string & delegate)
{
}

void Entity::destroy()
{
}

Domain * Entity::getMovementDomain()
{
    return 0;
}

void Entity::sendWorld(const Operation & op)
{
}

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
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

Location::Location()
{
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

void Router::addToEntity(const RootEntity & ent) const
{
}
