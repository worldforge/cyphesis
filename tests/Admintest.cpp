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

#include "server/Connection.h"
#include "server/Ruleset.h"
#include "server/ServerRouting.h"

#include "rulesets/Entity.h"

#include "common/CommSocket.h"
#include "common/compose.hpp"
#include "common/debug.h"
#include "common/Inheritance.h"
#include "common/Monitor.h"

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

class TestObject : public Router
{
  public:
    explicit TestObject(const std::string & id, long intId);

    virtual void externalOperation(const Operation &, Link &);
    virtual void operation(const Operation &, OpVector &);
};

TestObject::TestObject(const std::string & id, long intId) : Router(id, intId)
{
}

void TestObject::externalOperation(const Operation & op, Link &)
{
}

void TestObject::operation(const Operation &, OpVector &)
{
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
    static Account * Account_LogoutOperation_called;
    static Account * Account_SetOperation_called;
    static Account * Account_createObject_called;
    static bool Ruleset_modifyRule_called;
    static int Ruleset_modifyRule_retval;
    static bool Ruleset_installRule_called;
    static int Ruleset_installRule_retval;
    static bool newId_fail;
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
    void test_LogoutOperation_unknown();
    void test_LogoutOperation_known();
    void test_LogoutOperation_other_but_unconnected();
    void test_GetOperation_no_args();
    void test_GetOperation_no_objtype();
    void test_GetOperation_no_id();
    void test_GetOperation_empty_id();
    void test_GetOperation_obj_unconnected();
    void test_GetOperation_obj_OOG();
    void test_GetOperation_obj_IG();
    void test_GetOperation_obj_not_found();
    void test_GetOperation_rule_found();
    void test_GetOperation_rule_not_found();
    void test_GetOperation_unknown();
    void test_SetOperation_no_args();
    void test_SetOperation_no_objtype();
    void test_SetOperation_no_id();
    void test_SetOperation_obj_IG();
    void test_SetOperation_obj_not_found();
    void test_SetOperation_rule_unknown();
    void test_SetOperation_rule_fail();
    void test_SetOperation_rule_success();
    void test_SetOperation_unknown();
    void test_OtherOperation_known();
    void test_OtherOperation_monitor();
    void test_customMonitorOperation_succeed();
    void test_customMonitorOperation_monitorin();
    void test_customMonitorOperation_unconnected();
    void test_customMonitorOperation_no_args();
    void test_createObject_class_no_id();
    void test_createObject_class_exists();
    void test_createObject_class_parent_absent();
    void test_createObject_class_fail();
    void test_createObject_class_succeed();
    void test_createObject_juncture_id_fail();
    void test_createObject_juncture();
    void test_createObject_juncture_serialno();
    void test_createObject_fallthrough();

    static void set_Link_sent_called();
    static void set_Account_LogoutOperation_called(Account * );
    static void set_Account_SetOperation_called(Account * );
    static void set_Account_createObject_called(Account * );
    static void set_Ruleset_modifyRule_called();
    static int get_Ruleset_modifyRule_retval();
    static void set_Ruleset_installRule_called();
    static int get_Ruleset_installRule_retval();
    static bool get_newId_fail();
};

bool Admintest::Link_sent_called = false;
Account * Admintest::Account_LogoutOperation_called = 0;
Account * Admintest::Account_SetOperation_called = 0;
Account * Admintest::Account_createObject_called = 0;
bool Admintest::Ruleset_modifyRule_called = false;
int Admintest::Ruleset_modifyRule_retval = 0;
bool Admintest::Ruleset_installRule_called = false;
int Admintest::Ruleset_installRule_retval = 0;
bool Admintest::newId_fail = false;

void Admintest::set_Link_sent_called()
{
    Link_sent_called = true;
}

void Admintest::set_Account_LogoutOperation_called(Account * ac)
{
    Account_LogoutOperation_called = ac;
}

void Admintest::set_Account_SetOperation_called(Account * ac)
{
    Account_SetOperation_called = ac;
}

void Admintest::set_Account_createObject_called(Account * ac)
{
    Account_createObject_called = ac;
}

void Admintest::set_Ruleset_modifyRule_called()
{
    Ruleset_modifyRule_called = true;
}

int Admintest::get_Ruleset_modifyRule_retval()
{
    return Ruleset_modifyRule_retval;
}

void Admintest::set_Ruleset_installRule_called()
{
    Ruleset_installRule_called = true;
}

int Admintest::get_Ruleset_installRule_retval()
{
    return Ruleset_installRule_retval;
}

bool Admintest::get_newId_fail()
{
    return newId_fail;
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
    ADD_TEST(Admintest::test_LogoutOperation_unknown);
    ADD_TEST(Admintest::test_LogoutOperation_known);
    ADD_TEST(Admintest::test_LogoutOperation_other_but_unconnected);
    ADD_TEST(Admintest::test_GetOperation_no_args);
    ADD_TEST(Admintest::test_GetOperation_no_objtype);
    ADD_TEST(Admintest::test_GetOperation_no_id);
    ADD_TEST(Admintest::test_GetOperation_empty_id);
    ADD_TEST(Admintest::test_GetOperation_obj_unconnected);
    ADD_TEST(Admintest::test_GetOperation_obj_OOG);
    ADD_TEST(Admintest::test_GetOperation_obj_IG);
    ADD_TEST(Admintest::test_GetOperation_obj_not_found);
    ADD_TEST(Admintest::test_GetOperation_rule_found);
    ADD_TEST(Admintest::test_GetOperation_rule_not_found);
    ADD_TEST(Admintest::test_GetOperation_unknown);
    ADD_TEST(Admintest::test_SetOperation_no_args);
    ADD_TEST(Admintest::test_SetOperation_no_objtype);
    ADD_TEST(Admintest::test_SetOperation_no_id);
    ADD_TEST(Admintest::test_SetOperation_obj_IG);
    ADD_TEST(Admintest::test_SetOperation_obj_not_found);
    ADD_TEST(Admintest::test_SetOperation_rule_unknown);
    ADD_TEST(Admintest::test_SetOperation_rule_fail);
    ADD_TEST(Admintest::test_SetOperation_rule_success);
    ADD_TEST(Admintest::test_SetOperation_unknown);
    ADD_TEST(Admintest::test_OtherOperation_known);
    ADD_TEST(Admintest::test_OtherOperation_monitor);
    ADD_TEST(Admintest::test_customMonitorOperation_succeed);
    ADD_TEST(Admintest::test_customMonitorOperation_monitorin);
    ADD_TEST(Admintest::test_customMonitorOperation_unconnected);
    ADD_TEST(Admintest::test_customMonitorOperation_no_args);
    ADD_TEST(Admintest::test_createObject_class_no_id);
    ADD_TEST(Admintest::test_createObject_class_exists);
    ADD_TEST(Admintest::test_createObject_class_parent_absent);
    ADD_TEST(Admintest::test_createObject_class_fail);
    ADD_TEST(Admintest::test_createObject_class_succeed);
    ADD_TEST(Admintest::test_createObject_juncture_id_fail);
    ADD_TEST(Admintest::test_createObject_juncture);
    ADD_TEST(Admintest::test_createObject_juncture_serialno);
    ADD_TEST(Admintest::test_createObject_fallthrough);
}

long Admintest::newId()
{
    return ++m_id_counter;
}

void Admintest::setup()
{
    Atlas::Objects::Operation::MONITOR_NO = m_id_counter++;

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
    Account_LogoutOperation_called = 0;

    Operation op;
    OpVector res;

    m_account->LogoutOperation(op, res);

    ASSERT_EQUAL(res.size(), 0u);
    ASSERT_EQUAL(Account_LogoutOperation_called, m_account);
}

void Admintest::test_LogoutOperation_no_id()
{
    Operation op;
    OpVector res;

    Anonymous arg;
    op->setArgs1(arg);

    m_account->LogoutOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_LogoutOperation_self()
{
    Account_LogoutOperation_called = 0;

    Operation op;
    OpVector res;

    Anonymous arg;
    arg->setId(m_account->getId());
    op->setArgs1(arg);

    m_account->LogoutOperation(op, res);

    ASSERT_EQUAL(res.size(), 0u);

    ASSERT_EQUAL(Account_LogoutOperation_called, m_account);
}

void Admintest::test_LogoutOperation_unknown()
{
    long cid = m_id_counter++;

    Operation op;
    OpVector res;

    Anonymous arg;
    arg->setId(String::compose("%1", cid));
    op->setArgs1(arg);

    m_account->LogoutOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_LogoutOperation_known()
{
    Account_LogoutOperation_called = 0;

    long cid = m_id_counter++;
    std::string cid_str = String::compose("%1", cid);
    Account * ac2 = new Admin(0,
                              "f3332c00-5d2b-45c1-8cf4-3429bdf2845f",
                              "c0e095f0-575c-477c-bafd-2055d6958d4d",
                              cid_str, cid);

    m_server->addObject(ac2);

    ASSERT_EQUAL(m_server->getObject(cid_str), ac2);

    Atlas::Objects::Operation::Logout op;
    OpVector res;

    Anonymous arg;
    arg->setId(cid_str);
    op->setArgs1(arg);

    m_account->LogoutOperation(op, res);

    ASSERT_EQUAL(res.size(), 0u);

    ASSERT_EQUAL(Account_LogoutOperation_called, ac2);

    delete ac2;
}

void Admintest::test_LogoutOperation_other_but_unconnected()
{
    Account_LogoutOperation_called = 0;
    m_account->m_connection = 0;

    long cid = m_id_counter++;
    std::string cid_str = String::compose("%1", cid);
    Account * ac2 = new Admin(0,
                              "f3332c00-5d2b-45c1-8cf4-3429bdf2845f",
                              "c0e095f0-575c-477c-bafd-2055d6958d4d",
                              cid_str, cid);

    m_server->addObject(ac2);

    ASSERT_EQUAL(m_server->getObject(cid_str), ac2);

    Atlas::Objects::Operation::Logout op;
    OpVector res;

    Anonymous arg;
    arg->setId(cid_str);
    op->setArgs1(arg);

    m_account->LogoutOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);

    ASSERT_NULL(Account_LogoutOperation_called);

    delete ac2;
}

void Admintest::test_GetOperation_no_args()
{
    Atlas::Objects::Operation::Get op;
    OpVector res;

    m_account->GetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_GetOperation_no_objtype()
{
    Atlas::Objects::Operation::Get op;
    OpVector res;

    Anonymous arg;
    op->setArgs1(arg);

    m_account->GetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_GetOperation_no_id()
{
    Atlas::Objects::Operation::Get op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("obj");
    op->setArgs1(arg);

    m_account->GetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_GetOperation_empty_id()
{
    Atlas::Objects::Operation::Get op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("obj");
    arg->setId("");
    op->setArgs1(arg);

    m_account->GetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_GetOperation_obj_unconnected()
{
    m_account->m_connection = 0;

    Atlas::Objects::Operation::Get op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("obj");
    arg->setId("9287");
    op->setArgs1(arg);

    m_account->GetOperation(op, res);

    ASSERT_EQUAL(res.size(), 0u);
}

void Admintest::test_GetOperation_obj_OOG()
{
    long cid = m_id_counter++;
    std::string cid_str = String::compose("%1", cid);
    Router * to = new TestObject(cid_str, cid);

    m_server->addObject(to);

    Atlas::Objects::Operation::Get op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("obj");
    arg->setId(cid_str);
    op->setArgs1(arg);

    m_account->GetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const Operation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);
    ASSERT_EQUAL(reply->getArgs().size(), 1u);

    const Root & reply_arg = reply->getArgs().front();

    ASSERT_TRUE(!reply_arg->isDefaultId());
    ASSERT_EQUAL(reply_arg->getId(), to->getId());

    delete to;
}

void Admintest::test_GetOperation_obj_IG()
{
    long cid = m_id_counter++;
    std::string cid_str = String::compose("%1", cid);
    Entity * to = new Entity(cid_str, cid);

    m_server->m_world.addEntity(to);

    Atlas::Objects::Operation::Get op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("obj");
    arg->setId(cid_str);
    op->setArgs1(arg);

    m_account->GetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const Operation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);
    ASSERT_EQUAL(reply->getArgs().size(), 1u);

    const Root & reply_arg = reply->getArgs().front();

    ASSERT_TRUE(!reply_arg->isDefaultId());
    ASSERT_EQUAL(reply_arg->getId(), to->getId());

    delete to;
}

void Admintest::test_GetOperation_obj_not_found()
{
    Atlas::Objects::Operation::Get op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("obj");
    arg->setId("1741");
    op->setArgs1(arg);

    m_account->GetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_GetOperation_rule_found()
{
    Atlas::Objects::Operation::Get op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("class");
    arg->setId("root");
    op->setArgs1(arg);

    m_account->GetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const Operation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);
    ASSERT_EQUAL(reply->getArgs().size(), 1u);

    const Root & reply_arg = reply->getArgs().front();

    ASSERT_TRUE(!reply_arg->isDefaultId());
    ASSERT_EQUAL(reply_arg->getId(), "root");
}

void Admintest::test_GetOperation_rule_not_found()
{
    Atlas::Objects::Operation::Get op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("class");
    arg->setId("1741");
    op->setArgs1(arg);

    m_account->GetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_GetOperation_unknown()
{
    Atlas::Objects::Operation::Get op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("unknownobjtype");
    arg->setId("1741");
    op->setArgs1(arg);

    m_account->GetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_SetOperation_no_args()
{
    Atlas::Objects::Operation::Set op;
    OpVector res;

    m_account->SetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_SetOperation_no_objtype()
{
    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_SetOperation_no_id()
{
    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("obj");
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_SetOperation_obj_IG()
{
    Account_SetOperation_called = 0;

    long cid = m_id_counter++;
    Entity * c = new Entity(compose("%1", cid), cid);

    m_account->m_charactersDict.insert(std::make_pair(cid, c));

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("obj");
    arg->setId(c->getId());
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_EQUAL(Account_SetOperation_called, m_account);

    // The operation returned would have come from Account::SetOperation
    // but that is stubbed out
    ASSERT_EQUAL(res.size(), 0u);

    delete c;
}

void Admintest::test_SetOperation_obj_not_found()
{
    Account_SetOperation_called = 0;

    long cid = m_id_counter++;

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("obj");
    arg->setId(compose("%1", cid));
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_NULL(Account_SetOperation_called);

    // FIXME No error? Is that right?
    ASSERT_EQUAL(res.size(), 0u);
}

void Admintest::test_SetOperation_rule_unknown()
{
    Ruleset_modifyRule_called = false;

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("class");
    arg->setId("unimportant_unmatched_string");
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
    ASSERT_TRUE(!Ruleset_modifyRule_called);
}

void Admintest::test_SetOperation_rule_fail()
{
    Ruleset_modifyRule_called = false;
    Ruleset_modifyRule_retval = -1;

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("class");
    arg->setId("root");
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
    ASSERT_TRUE(Ruleset_modifyRule_called);
}

void Admintest::test_SetOperation_rule_success()
{
    Ruleset_modifyRule_called = false;
    Ruleset_modifyRule_retval = 0;

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("class");
    arg->setId("root");
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);
    ASSERT_TRUE(Ruleset_modifyRule_called);
}

void Admintest::test_SetOperation_unknown()
{
    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setObjtype("unimportant_unknown_string");
    arg->setId("root");
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_OtherOperation_known()
{
    Operation op;
    OpVector res;

    m_account->OtherOperation(op, res);

    // Nothing should have happened.
}

void Admintest::test_OtherOperation_monitor()
{
    Atlas::Objects::Operation::Monitor op;
    OpVector res;

    Root arg;
    op->setArgs1(arg);

    m_account->OtherOperation(op, res);

    // Quick check to ensure op passed through to customMonitorOperaion
    // That function is fully tested below
    ASSERT_TRUE(m_account->m_monitorConnection.connected());
}

void Admintest::test_customMonitorOperation_succeed()
{
    // Check that Dispatching in not yet connected
    assert(m_server->m_world.Dispatching.slots().begin() ==
                 m_server->m_world.Dispatching.slots().end());

    Atlas::Objects::Operation::Monitor op;
    OpVector res;

    Root arg;
    op->setArgs1(arg);

    m_account->customMonitorOperation(op, res);

    ASSERT_TRUE(m_account->m_monitorConnection.connected());

    // Check that Dispatching has been connected
    assert(m_server->m_world.Dispatching.slots().begin() !=
                 m_server->m_world.Dispatching.slots().end());

}

void Admintest::test_customMonitorOperation_monitorin()
{
    // Check that Dispatching in not yet connected
    assert(m_server->m_world.Dispatching.slots().begin() ==
                 m_server->m_world.Dispatching.slots().end());

    // Set it up so it is already monitoring
    m_account->m_monitorConnection =
          null_signal.connect(sigc::mem_fun(this, &Admintest::null_method));
    ASSERT_TRUE(m_account->m_monitorConnection.connected());

    Atlas::Objects::Operation::Monitor op;
    OpVector res;

    Root arg;
    op->setArgs1(arg);

    m_account->customMonitorOperation(op, res);

    ASSERT_TRUE(m_account->m_monitorConnection.connected());

    // Check that Dispatching in not been connected
    assert(m_server->m_world.Dispatching.slots().begin() ==
                 m_server->m_world.Dispatching.slots().end());

}

void Admintest::test_customMonitorOperation_unconnected()
{
    m_account->m_connection = 0;

    // Check that Dispatching in not yet connected
    assert(m_server->m_world.Dispatching.slots().begin() ==
                 m_server->m_world.Dispatching.slots().end());

    Atlas::Objects::Operation::Monitor op;
    OpVector res;

    Root arg;
    op->setArgs1(arg);

    m_account->customMonitorOperation(op, res);

    ASSERT_TRUE(!m_account->m_monitorConnection.connected());

    // Check that Dispatching has not been connected
    assert(m_server->m_world.Dispatching.slots().begin() ==
                 m_server->m_world.Dispatching.slots().end());
}

void Admintest::test_customMonitorOperation_no_args()
{
    // Set it up so it is already monitoring
    m_account->m_monitorConnection =
          null_signal.connect(sigc::mem_fun(this, &Admintest::null_method));
    ASSERT_TRUE(m_account->m_monitorConnection.connected());

    // Check that Dispatching in not yet connected
    assert(m_server->m_world.Dispatching.slots().begin() ==
                 m_server->m_world.Dispatching.slots().end());

    Atlas::Objects::Operation::Monitor op;
    OpVector res;

    m_account->customMonitorOperation(op, res);

    ASSERT_TRUE(!m_account->m_monitorConnection.connected());

    // Check that Dispatching has not been connected
    assert(m_server->m_world.Dispatching.slots().begin() ==
                 m_server->m_world.Dispatching.slots().end());
}

void Admintest::test_createObject_class_no_id()
{
    Ruleset_installRule_called = false;
    Ruleset_installRule_retval = -1;

    std::string parent("root");
    Root arg;
    Atlas::Objects::Operation::Create op;
    OpVector res;

    arg->setObjtype("class");

    m_account->createObject(parent, arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);

    ASSERT_TRUE(!Ruleset_installRule_called);
}

void Admintest::test_createObject_class_exists()
{
    Ruleset_installRule_called = false;
    Ruleset_installRule_retval = -1;

    Inheritance::instance().addChild(atlasClass("character", "root"));

    std::string parent("root");
    Root arg;
    Atlas::Objects::Operation::Create op;
    OpVector res;

    arg->setObjtype("class");
    arg->setId("character");

    m_account->createObject(parent, arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);

    ASSERT_TRUE(!Ruleset_installRule_called);
}

void Admintest::test_createObject_class_parent_absent()
{
    Ruleset_installRule_called = false;
    Ruleset_installRule_retval = -1;

    std::string parent("root_entity");
    Root arg;
    Atlas::Objects::Operation::Create op;
    OpVector res;

    arg->setObjtype("class");
    arg->setId("character");

    m_account->createObject(parent, arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);

    ASSERT_TRUE(!Ruleset_installRule_called);
}

void Admintest::test_createObject_class_fail()
{
    Ruleset_installRule_called = false;
    Ruleset_installRule_retval = -1;

    std::string parent("root");
    Root arg;
    Atlas::Objects::Operation::Create op;
    OpVector res;

    arg->setObjtype("class");
    arg->setId("character");

    m_account->createObject(parent, arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);

    ASSERT_TRUE(Ruleset_installRule_called);
}

void Admintest::test_createObject_class_succeed()
{
    Ruleset_installRule_called = false;
    Ruleset_installRule_retval = 0;

    std::string parent("root");
    Root arg;
    Atlas::Objects::Operation::Create op;
    OpVector res;

    arg->setObjtype("class");
    arg->setId("character");

    m_account->createObject(parent, arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);

    ASSERT_TRUE(Ruleset_installRule_called);
}

void Admintest::test_createObject_juncture_id_fail()
{
    newId_fail = true;

    std::string parent("juncture");
    Root arg;
    Atlas::Objects::Operation::Create op;
    OpVector res;

    arg->setObjtype("obj");

    m_account->createObject(parent, arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Admintest::test_createObject_juncture()
{
    newId_fail = false;

    std::string parent("juncture");
    Root arg;
    Atlas::Objects::Operation::Create op;
    OpVector res;

    arg->setObjtype("obj");

    m_account->createObject(parent, arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);
    ASSERT_TRUE(res.front()->isDefaultRefno());
}

void Admintest::test_createObject_juncture_serialno()
{
    newId_fail = false;

    std::string parent("juncture");
    Root arg;
    Atlas::Objects::Operation::Create op;
    op->setSerialno(m_id_counter++);
    OpVector res;

    arg->setObjtype("obj");

    m_account->createObject(parent, arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::INFO_NO);
    ASSERT_TRUE(!res.front()->isDefaultRefno());
}

void Admintest::test_createObject_fallthrough()
{
    Account_createObject_called = 0;

    std::string parent("unimportant_string");
    Root arg;
    Atlas::Objects::Operation::Create op;
    op->setSerialno(m_id_counter++);
    OpVector res;

    arg->setObjtype("obj");

    m_account->createObject(parent, arg, op, res);

    ASSERT_EQUAL(res.size(), 0u);

    ASSERT_EQUAL(Account_createObject_called,
                 m_account);
}

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
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
         ConnectableRouter(id, intId, conn),
         m_username(uname), m_password(passwd)
{
}

Account::~Account()
{
}

LocatedEntity * Account::addNewCharacter(const std::string & typestr,
                                         const RootEntity & ent,
                                         const Root & arg)
{
    return 0;
}

int Account::connectCharacter(LocatedEntity *chr)
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
    Admintest::set_Account_createObject_called(this);
}

void Account::addToMessage(MapType & omap) const
{
}

void Account::addToEntity(const RootEntity & ent) const
{
}

void Account::externalOperation(const Operation & op, Link &)
{
}

void Account::operation(const Operation & op, OpVector & res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::LOGOUT_NO) {
        this->LogoutOperation(op, res);
    }
}

void Account::LogoutOperation(const Operation &, OpVector &)
{
    Admintest::set_Account_LogoutOperation_called(this);
}

void Account::CreateOperation(const Operation &, OpVector &)
{
}

void Account::SetOperation(const Operation &, OpVector &)
{
    Admintest::set_Account_SetOperation_called(this);
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

void Connection::externalOperation(const Operation & op, Link &)
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

void Connection::addEntity(LocatedEntity * ent)
{
}

ConnectableRouter::ConnectableRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}

ConnectableRouter::~ConnectableRouter()
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
    Admintest::set_Ruleset_modifyRule_called();
    return Admintest::get_Ruleset_modifyRule_retval();
}

int Ruleset::installRule(const std::string & class_name,
                         const std::string & section,
                         const Root & class_desc)
{
    Admintest::set_Ruleset_installRule_called();
    return Admintest::get_Ruleset_installRule_retval();
}

Juncture::Juncture(Connection * c, const std::string & id, long iid) :
          ConnectableRouter(id, iid, c),
          m_address(0),
          m_socket(0),
          m_peer(0),
          m_connectRef(0)
{
}

Juncture::~Juncture()
{
}

void Juncture::externalOperation(const Operation & op, Link &)
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
    m_objects[obj->getIntId()] = obj;
}

Router * ServerRouting::getObject(const std::string & id) const
{
    RouterMap::const_iterator I = m_objects.find(integerId(id));
    if (I == m_objects.end()) {
        return 0;
    } else {
        return I->second;
    }
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

void ServerRouting::externalOperation(const Operation & op, Link &)
{
}

void ServerRouting::operation(const Operation &, OpVector &)
{
}

TeleportAuthenticator * TeleportAuthenticator::m_instance = NULL;

int TeleportAuthenticator::removeTeleport(const std::string &entity_id)
{
    return 0;
}

LocatedEntity *TeleportAuthenticator::authenticateTeleport(const std::string &entity_id,
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
           Thing(id, intId),
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

void Character::externalOperation(const Operation & op, Link &)
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
       Entity(id, intId)
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
        LocatedEntity(id, intId), m_motion(0)
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

void Entity::externalOperation(const Operation & op, Link &)
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
    ent->setId(getId());
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

void Entity::callOperation(const Operation & op, OpVector & res)
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

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
    delete &m_gameWorld;
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    return 0;
}

LocatedEntity * BaseWorld::getEntity(long id) const
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
    ent->setId(getId());
}

void Router::clientError(const Operation & op,
                         const std::string & errstring,
                         OpVector & res,
                         const std::string & to) const
{
    res.push_back(Atlas::Objects::Operation::Error());
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
    if (Admintest::get_newId_fail()) {
        return -1;
    }
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

#include <common/Shaker.h>

Shaker::Shaker()
{
}
std::string Shaker::generateSalt(size_t length)
{
    return "";
}
