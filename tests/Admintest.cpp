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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"
#include "TestWorld.h"

#include "server/Admin.h"

#include "server/CommSocket.h"
#include "server/Connection.h"
#include "server/Ruleset.h"
#include "server/ServerRouting.h"

#include "rulesets/Entity.h"

#include "common/compose.hpp"
#include "common/debug.h"
#include "common/Inheritance.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <sigc++/functors/mem_fun.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

std::ostream & operator<<(std::ostream & os,
                          const MapType::const_iterator &)
{
    os << "[iterator]";
    return os;
}

std::ostream & operator<<(std::ostream & os,
                          const EntityDict::const_iterator &)
{
    os << "[iterator]";
    return os;
}

std::ostream & operator<<(std::ostream & os,
                          const Element & e)
{
    debug_dump(e, os);
    return os;
}

class Admintest : public Cyphesis::TestBase
{
  protected:
    static long m_id_counter;

    ServerRouting * m_server;
    Connection * m_connection;
    Admin * m_account;

    bool m_monitor_flag;

    sigc::signal<void, Operation> null_signal;

    void null_method(Operation op) { }

    static bool Link_sent_called;
  public:
    Admintest();

    static long newId();

    void setup();
    void teardown();

    void test_null();
    void test_getType();
    void test_addToMessage();
    void test_addToMessage_tree();
    void test_addToEntity();
    void test_addToEntity_tree();
    void test_opDispatched();
    void test_opDispatched_unconnected();
    void test_opDispatched_unconnected_monitored();
    void test_characterError_default_parents();
    void test_characterError_empty_parents();
    void test_characterError_valid();
    void test_LogoutOperation_no_args();
    void test_LogoutOperation_no_id();
    void test_LogoutOperation_self();
    void test_LogoutOperation_unknwon();
    void test_LogoutOperation_knwon();

    static void set_Link_sent_called();
};

bool Admintest::Link_sent_called = false;

void Admintest::set_Link_sent_called()
{
    Link_sent_called = true;
}

long Admintest::m_id_counter = 0L;

Admintest::Admintest() : m_server(0),
                         m_connection(0),
                         m_account(0)
{
    ADD_TEST(Admintest::test_null);
    ADD_TEST(Admintest::test_getType);
    ADD_TEST(Admintest::test_addToMessage);
    ADD_TEST(Admintest::test_addToMessage_tree);
    ADD_TEST(Admintest::test_addToEntity);
    ADD_TEST(Admintest::test_addToEntity_tree);
    ADD_TEST(Admintest::test_opDispatched);
    ADD_TEST(Admintest::test_opDispatched_unconnected);
    ADD_TEST(Admintest::test_opDispatched_unconnected_monitored);
    ADD_TEST(Admintest::test_characterError_default_parents);
    ADD_TEST(Admintest::test_characterError_empty_parents);
    ADD_TEST(Admintest::test_characterError_valid);
    ADD_TEST(Admintest::test_LogoutOperation_no_args);
    ADD_TEST(Admintest::test_LogoutOperation_no_id);
    ADD_TEST(Admintest::test_LogoutOperation_self);
    ADD_TEST(Admintest::test_LogoutOperation_unknwon);
    ADD_TEST(Admintest::test_LogoutOperation_knwon);
}

long Admintest::newId()
{
    return ++m_id_counter;
}

void Admintest::setup()
{
    Entity * gw = new Entity(compose("%1", m_id_counter),
                             m_id_counter++);
    m_server = new ServerRouting(*new TestWorld(*gw),
                                 "5529d7a4-0158-4dc1-b4a5-b5f260cac635",
                                 "bad621d4-616d-4faf-b9e6-471d12b139a9",
                                 compose("%1", m_id_counter), m_id_counter++,
                                 compose("%1", m_id_counter), m_id_counter++);
    m_connection = new Connection(*(CommSocket*)0, *m_server,
                                  "8d18a4e8-f14f-4a46-997e-ada120d5438f",
                                  compose("%1", m_id_counter), m_id_counter++);
    m_account = new Admin(m_connection,
                          "6c9f3236-5de7-4ba4-8b7a-b0222df0af38",
                          "fa1a03a2-a745-4033-85cb-bb694e921e62",
                          compose("%1", m_id_counter), m_id_counter++);
}

void Admintest::teardown()
{
    Inheritance::clear();

    delete m_server;
    delete m_account;
    delete m_connection;
}

void Admintest::test_null()
{
    ASSERT_TRUE(m_account != 0);
}

void Admintest::test_getType()
{
    const char * type_string = m_account->getType();

    ASSERT_EQUAL(std::string("admin"), type_string);
}

void Admintest::test_addToMessage()
{
    MapType data;

    m_account->addToMessage(data);

    ASSERT_NOT_EQUAL(data.find("character_types"), data.end());
    ASSERT_EQUAL(data["character_types"], ListType());
}

void Admintest::test_addToMessage_tree()
{
    Inheritance::instance().addChild(atlasClass("character", "root"));
    Inheritance::instance().addChild(atlasClass("human", "character"));
    Inheritance::instance().addChild(atlasClass("settler", "human"));
    Inheritance::instance().addChild(atlasClass("goblin", "character"));

    MapType data;

    m_account->addToMessage(data);

    ListType expected_character_types;
    expected_character_types.push_back("character");
    expected_character_types.push_back("human");
    expected_character_types.push_back("settler");
    expected_character_types.push_back("goblin");

    // FIXME How do we know this order is consistent
    ASSERT_NOT_EQUAL(data.find("character_types"), data.end());
    ASSERT_EQUAL(data["character_types"], expected_character_types);
}

void Admintest::test_addToEntity_tree()
{
    Anonymous data;

    m_account->addToEntity(data);

    ASSERT_TRUE(data->hasAttr("character_types"));
    ASSERT_EQUAL(data->getAttr("character_types"), ListType());
}

void Admintest::test_addToEntity()
{
    Inheritance::instance().addChild(atlasClass("character", "root"));
    Inheritance::instance().addChild(atlasClass("human", "character"));
    Inheritance::instance().addChild(atlasClass("settler", "human"));
    Inheritance::instance().addChild(atlasClass("goblin", "character"));

    Anonymous data;

    m_account->addToEntity(data);

    ListType expected_character_types;
    expected_character_types.push_back("character");
    expected_character_types.push_back("human");
    expected_character_types.push_back("settler");
    expected_character_types.push_back("goblin");

    ASSERT_TRUE(data->hasAttr("character_types"));
    ASSERT_EQUAL(data->getAttr("character_types"),
                 expected_character_types);
}

void Admintest::test_opDispatched()
{
    Link_sent_called = false;

    m_account->m_monitorConnection =
          null_signal.connect(sigc::mem_fun(this, &Admintest::null_method));
    ASSERT_TRUE(m_account->m_monitorConnection.connected());

    Operation op;

    m_account->opDispatched(op);

    // The account is connected, so calling this should not affect the signal
    ASSERT_TRUE(m_account->m_monitorConnection.connected());

    // The operation should have been sent here
    ASSERT_TRUE(Link_sent_called);
}

void Admintest::test_opDispatched_unconnected()
{
    m_account->m_connection = 0;

    Link_sent_called = false;

    ASSERT_TRUE(!m_account->m_monitorConnection.connected());

    Operation op;

    m_account->opDispatched(op);

    // The operation should not have been sent here
    ASSERT_TRUE(!Link_sent_called);
}

void Admintest::test_opDispatched_unconnected_monitored()
{
    m_account->m_connection = 0;

    Link_sent_called = false;

    m_account->m_monitorConnection =
          null_signal.connect(sigc::mem_fun(this, &Admintest::null_method));
    ASSERT_TRUE(m_account->m_monitorConnection.connected());

    Operation op;

    m_account->opDispatched(op);

    // The account is unconnected, so calling opDispatched should not
    // cause the signal to get cut off
    ASSERT_TRUE(!m_account->m_monitorConnection.connected());

    // The operation should not have been sent here
    ASSERT_TRUE(!Link_sent_called);
}

void Admintest::test_characterError_default_parents()
{
    Operation op;
    Root ent;
    OpVector res;

    int ret = m_account->characterError(op, ent, res);

    ASSERT_NOT_EQUAL(ret, 0);
    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_characterError_empty_parents()
{
    Operation op;
    Root ent;
    OpVector res;

    ent->setParents(std::list<std::string>());

    int ret = m_account->characterError(op, ent, res);

    ASSERT_NOT_EQUAL(ret, 0);
    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_characterError_valid()
{
    Operation op;
    Root ent;
    OpVector res;

    ent->setParents(std::list<std::string>(1, "settler"));

    int ret = m_account->characterError(op, ent, res);

    ASSERT_EQUAL(ret, 0);
    ASSERT_EQUAL(res.size(), 0u);
}

void Admintest::test_LogoutOperation_no_args()
{
}

void Admintest::test_LogoutOperation_no_id()
{
}

void Admintest::test_LogoutOperation_self()
{
}

void Admintest::test_LogoutOperation_unknwon()
{
}

void Admintest::test_LogoutOperation_knwon()
{
}

void TestWorld::message(const Operation & op, Entity & ent)
{
}

Entity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}

int main()
{
    Ruleset::init("");

    Admintest t;

    return t.run();
}

// stubs

#include "server/Connection.h"
#include "server/Juncture.h"
#include "server/Persistence.h"
#include "server/TeleportAuthenticator.h"

#include "rulesets/Character.h"

#include "common/globals.h"
#include "common/id.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <cstdlib>
#include <cstdio>

Account::Account(Connection * conn,
                 const std::string & uname,
                 const std::string & passwd,
                 const std::string & id,
                 long intId) :
         ConnectedRouter(id, intId, conn),
         m_username(uname), m_password(passwd)
{
}

Account::~Account()
{
}

Entity * Account::addNewCharacter(const std::string & typestr,
                                  const RootEntity & ent,
                                  const Root & arg)
{
    return 0;
}

int Account::connectCharacter(Entity *chr)
{
    return 0;
}

const char * Account::getType() const
{
    return "account";
}

void Account::store() const
{
}

void Account::createObject(const std::string & type_str,
                           const Root & arg,
                           const Operation & op,
                           OpVector & res)
{
}

void Account::addToMessage(MapType & omap) const
{
}

void Account::addToEntity(const RootEntity & ent) const
{
}

void Account::operation(const Operation & op, OpVector & res)
{
}

void Account::LogoutOperation(const Operation &, OpVector &)
{
}

void Account::CreateOperation(const Operation &, OpVector &)
{
}

void Account::SetOperation(const Operation &, OpVector &)
{
}

void Account::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Account::TalkOperation(const Operation &, OpVector &)
{
}

void Account::LookOperation(const Operation &, OpVector &)
{
}

void Account::GetOperation(const Operation &, OpVector &)
{
}

void Account::OtherOperation(const Operation &, OpVector &)
{
}

Connection::Connection(CommSocket & client,
                       ServerRouting & svr,
                       const std::string & addr,
                       const std::string & id, long iid) :
            Link(client, id, iid), m_obsolete(false),
                                                m_server(svr)
{
}

Account * Connection::newAccount(const std::string & type,
                                 const std::string & username,
                                 const std::string & passwd,
                                 const std::string & id, long intId)
{
    return 0;
}

int Connection::verifyCredentials(const Account &,
                                  const Atlas::Objects::Root &) const
{
    return 0;
}

Connection::~Connection()
{
}

void Connection::addObject(Router * obj)
{
}

void Connection::operation(const Operation &, OpVector &)
{
}

void Connection::LoginOperation(const Operation &, OpVector &)
{
}

void Connection::LogoutOperation(const Operation &, OpVector &)
{
}

void Connection::CreateOperation(const Operation &, OpVector &)
{
}

void Connection::GetOperation(const Operation &, OpVector &)
{
}

void Connection::connectAvatar(Character * chr)
{
}

void Connection::addEntity(Entity * ent)
{
}

ConnectedRouter::ConnectedRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}

ConnectedRouter::~ConnectedRouter()
{
}

Ruleset::Ruleset(EntityBuilder * eb) :
      m_taskHandler(0),
      m_entityHandler(0),
      m_opHandler(0)
{
}

Ruleset * Ruleset::m_instance = NULL;

void Ruleset::init(const std::string & ruleset)
{
    m_instance = new Ruleset(0);
}

int Ruleset::modifyRule(const std::string & class_name,
                        const Root & class_desc)
{
    return 0;
}

int Ruleset::installRule(const std::string & class_name,
                         const std::string & section,
                         const Root & class_desc)
{
    return 0;
}

Juncture::Juncture(Connection * c, const std::string & id, long iid) :
          ConnectedRouter(id, iid, c),
          m_address(0),
          m_socket(0),
          m_peer(0),
          m_connectRef(0)
{
}

Juncture::~Juncture()
{
}

void Juncture::operation(const Operation & op, OpVector & res)
{
}

void Juncture::addToMessage(MapType & omap) const
{
}

void Juncture::addToEntity(const RootEntity & ent) const
{
}

ServerRouting::ServerRouting(BaseWorld & wrld,
                             const std::string & ruleset,
                             const std::string & name,
                             const std::string & id, long intId,
                             const std::string & lId, long lIntId) :
        Router(id, intId),
        m_svrRuleset(ruleset), m_svrName(name),
        m_numClients(0), m_world(wrld), m_lobby(*(Lobby*)0)
{
}

ServerRouting::~ServerRouting()
{
    delete &m_world;
}

void ServerRouting::addObject(Router * obj)
{
}

Router * ServerRouting::getObject(const std::string & id) const
{
    return 0;
}

void ServerRouting::addToMessage(Atlas::Message::MapType & omap) const
{
}

void ServerRouting::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

Account * ServerRouting::getAccountByName(const std::string & username)
{
    return 0;
}

void ServerRouting::addAccount(Account * a)
{
}

TeleportAuthenticator * TeleportAuthenticator::m_instance = NULL;

int TeleportAuthenticator::removeTeleport(const std::string &entity_id)
{
    return 0;
}

Entity *TeleportAuthenticator::authenticateTeleport(const std::string &entity_id,
                                            const std::string &possess_key)
{
    return 0;
}

Persistence * Persistence::m_instance = NULL;

Persistence::Persistence() : m_db(*(Database*)0)
{
}

Persistence * Persistence::instance()
{
    if (m_instance == NULL) {
        m_instance = new Persistence();
    }
    return m_instance;
}

void Persistence::putAccount(const Account & ac)
{
}

Character::Character(const std::string & id, long intId) :
           Character_parent(id, intId),
               m_movement(*(Movement*)0),
               m_mind(0), m_externalMind(0)
{
}

Character::~Character()
{
}

void Character::operation(const Operation & op, OpVector &)
{
}

void Character::externalOperation(const Operation & op)
{
}


void Character::ImaginaryOperation(const Operation & op, OpVector &)
{
}

void Character::InfoOperation(const Operation & op, OpVector &)
{
}

void Character::TickOperation(const Operation & op, OpVector &)
{
}

void Character::TalkOperation(const Operation & op, OpVector &)
{
}

void Character::NourishOperation(const Operation & op, OpVector &)
{
}

void Character::UseOperation(const Operation & op, OpVector &)
{
}

void Character::WieldOperation(const Operation & op, OpVector &)
{
}

void Character::AttackOperation(const Operation & op, OpVector &)
{
}

void Character::ActuateOperation(const Operation & op, OpVector &)
{
}

void Character::mindActuateOperation(const Operation &, OpVector &)
{
}

void Character::mindAttackOperation(const Operation &, OpVector &)
{
}

void Character::mindCombineOperation(const Operation &, OpVector &)
{
}

void Character::mindCreateOperation(const Operation &, OpVector &)
{
}

void Character::mindDeleteOperation(const Operation &, OpVector &)
{
}

void Character::mindDivideOperation(const Operation &, OpVector &)
{
}

void Character::mindEatOperation(const Operation &, OpVector &)
{
}

void Character::mindGoalInfoOperation(const Operation &, OpVector &)
{
}

void Character::mindImaginaryOperation(const Operation &, OpVector &)
{
}

void Character::mindLookOperation(const Operation &, OpVector &)
{
}

void Character::mindMoveOperation(const Operation &, OpVector &)
{
}

void Character::mindSetOperation(const Operation &, OpVector &)
{
}

void Character::mindSetupOperation(const Operation &, OpVector &)
{
}

void Character::mindTalkOperation(const Operation &, OpVector &)
{
}

void Character::mindThoughtOperation(const Operation &, OpVector &)
{
}

void Character::mindTickOperation(const Operation &, OpVector &)
{
}

void Character::mindTouchOperation(const Operation &, OpVector &)
{
}

void Character::mindUpdateOperation(const Operation &, OpVector &)
{
}

void Character::mindUseOperation(const Operation &, OpVector &)
{
}

void Character::mindWieldOperation(const Operation &, OpVector &)
{
}


void Character::mindOtherOperation(const Operation &, OpVector &)
{
}

void Character::sendMind(const Operation & op, OpVector & res)
{
}

Thing::Thing(const std::string & id, long intId) :
       Thing_parent(id, intId)
{
}

Thing::~Thing()
{
}

void Thing::DeleteOperation(const Operation & op, OpVector & res)
{
}

void Thing::MoveOperation(const Operation & op, OpVector & res)
{
}

void Thing::SetOperation(const Operation & op, OpVector & res)
{
}

void Thing::LookOperation(const Operation & op, OpVector & res)
{
}

void Thing::CreateOperation(const Operation & op, OpVector & res)
{
}

void Thing::UpdateOperation(const Operation & op, OpVector & res)
{
}

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId), m_motion(0), m_flags(0)
{
}

Entity::~Entity()
{
}

void Entity::destroy()
{
    destroyed.emit();
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

void Entity::externalOperation(const Operation & op)
{
}

void Entity::operation(const Operation & op, OpVector & res)
{
}

void Entity::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
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

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

void Entity::callOperation(const Operation & op, OpVector & res)
{
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_contains(0)
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

Link::Link(CommSocket & socket, const std::string & id, long iid) :
            Router(id, iid), m_encoder(0), m_commSocket(socket)
{
}

Link::~Link()
{
}

void Link::send(const Operation & op) const
{
    Admintest::set_Link_sent_called();
}

void Link::disconnect()
{
}

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance() : noClass(0)
{
    Atlas::Objects::Entity::Anonymous root_desc;

    root_desc->setParents(std::list<std::string>(0));
    root_desc->setObjtype("meta");
    root_desc->setId("root");

    TypeNode * root = new TypeNode("root", root_desc);

    atlasObjects["root"] = root;
}

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
    }
    return *m_instance;
}

void Inheritance::clear()
{
    if (m_instance != NULL) {
        m_instance->flush();
        delete m_instance;
        m_instance = NULL;
    }
}

const TypeNode * Inheritance::getType(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second;
}

const Root & Inheritance::getClass(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return noClass;
    }
    return I->second->description();
}

bool Inheritance::hasClass(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return false;
    }
    return true;
}

TypeNode * Inheritance::addChild(const Root & obj)
{
    const std::string & child = obj->getId();
    const std::string & parent = obj->getParents().front();
    assert(atlasObjects.find(child) == atlasObjects.end());

    TypeNodeDict::iterator I = atlasObjects.find(parent);
    assert(I != atlasObjects.end());

    Element children(ListType(1, child));
    if (I->second->description()->copyAttr("children", children) == 0) {
        assert(children.isList());
        children.asList().push_back(child);
    }
    I->second->description()->setAttr("children", children);

    TypeNode * type = new TypeNode(child, obj);
    type->setParent(I->second);

    atlasObjects[child] = type;

    return type;
}

void Inheritance::flush()
{
    TypeNodeDict::const_iterator I = atlasObjects.begin();
    TypeNodeDict::const_iterator Iend = atlasObjects.end();
    for (; I != Iend; ++I) {
        delete I->second;
    }
    atlasObjects.clear();
}

Root atlasClass(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;

    r->setParents(std::list<std::string>(1, parent));
    r->setObjtype("class");
    r->setId(name);

    return r;
}

TypeNode::TypeNode(const std::string & name,
                   const Atlas::Objects::Root & d) : m_name(name),
                                                     m_description(d),
                                                     m_parent(0)
{
}

TypeNode::~TypeNode()
{
}

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(Entity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
    delete &m_gameWorld;
}

Entity * BaseWorld::getEntity(const std::string & id) const
{
    return 0;
}

Entity * BaseWorld::getEntity(long id) const
{
    return 0;
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

void Router::clientError(const Operation & op,
                         const std::string & errstring,
                         OpVector & res,
                         const std::string & to) const
{
}

void Router::error(const Operation & op,
                   const std::string & errstring,
                   OpVector & res,
                   const std::string & to) const
{
    res.push_back(Atlas::Objects::Operation::Error());
}

Location::Location() : m_loc(0)
{
}

long newId(std::string & id)
{
    static char buf[32];
    long new_id = Admintest::newId();
    sprintf(buf, "%ld", new_id);
    id = buf;
    assert(!id.empty());
    return new_id;
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

void logEvent(LogEvent lev, const std::string & msg)
{
}

void log(LogLevel lvl, const std::string & msg)
{
}

bool database_flag = false;

namespace Atlas { namespace Objects { namespace Operation {
int MONITOR_NO = -1;
} } }
