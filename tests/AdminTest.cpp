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
#include "TestWorld.h"

#include "server/Admin.h"

#include "server/Connection.h"
#include "server/Ruleset.h"
#include "server/ServerRouting.h"

#include "rules/simulation/Entity.h"

#include "common/CommSocket.h"
#include "common/compose.hpp"
#include "common/debug.h"
#include "common/Inheritance.h"
#include "common/operations/Monitor.h"

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
                          const std::map<long, LocatedEntity *>::const_iterator &)
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

class TestObject : public ConnectableRouter
{
  public:
    explicit TestObject(const std::string & id, long intId);

        void externalOperation(const Operation &, Link &) override;

        void operation(const Operation &, OpVector &) override;
        void setConnection(Connection* connection) override {

        }

        Connection* getConnection() const override
        {
            return nullptr;
        }
};

TestObject::TestObject(const std::string & id, long intId) : ConnectableRouter(id, intId)
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
    TestWorld* m_world;

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
    void test_characterError_default_parent();
    void test_characterError_empty_parent();
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

        Inheritance* inheritance;
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
    ADD_TEST(Admintest::test_opDispatched);
    ADD_TEST(Admintest::test_opDispatched_unconnected);
    ADD_TEST(Admintest::test_opDispatched_unconnected_monitored);
    ADD_TEST(Admintest::test_characterError_default_parent);
    ADD_TEST(Admintest::test_characterError_empty_parent);
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
    inheritance = new Inheritance();
    Atlas::Objects::Operation::MONITOR_NO = m_id_counter++;

    Ref<Entity> gw = new Entity(compose("%1", m_id_counter),
                             m_id_counter++);
    m_world = new TestWorld(gw);

    m_server = new ServerRouting(*m_world,
                                 "5529d7a4-0158-4dc1-b4a5-b5f260cac635",
                                 "bad621d4-616d-4faf-b9e6-471d12b139a9",
                                 compose("%1", m_id_counter), m_id_counter++,
                                 compose("%1", m_id_counter), m_id_counter++);
    m_connection = new Connection(*(CommSocket*) nullptr, *m_server,
                                  "8d18a4e8-f14f-4a46-997e-ada120d5438f",
                                  compose("%1", m_id_counter), m_id_counter++);
    m_account = new Admin(m_connection,
                          "6c9f3236-5de7-4ba4-8b7a-b0222df0af38",
                          "fa1a03a2-a745-4033-85cb-bb694e921e62",
                          compose("%1", m_id_counter), m_id_counter++);
}

void Admintest::teardown()
{
    delete m_world;
    delete inheritance;

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

void Admintest::test_characterError_default_parent()
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

void Admintest::test_characterError_empty_parent()
{
    Operation op;
    Root ent;
    OpVector res;

    ent->setParent("");

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

    ent->setParent("settler");

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
    auto to = new TestObject(cid_str, cid);

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

    
}

void Admintest::test_GetOperation_obj_IG()
{
    long cid = m_id_counter++;
    std::string cid_str = String::compose("%1", cid);
    Ref<Entity>  to = new Entity(cid_str, cid);

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
    Ref<Entity>  c = new Entity(compose("%1", cid), cid);

    m_account->m_charactersDict.insert(std::make_pair(cid, c.get()));

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
    arg->setParent(parent);

    m_account->createObject(arg, op, res);

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
    arg->setParent(parent);

    m_account->createObject(arg, op, res);

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
    arg->setParent(parent);

    m_account->createObject(arg, op, res);

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
    arg->setParent(parent);

    m_account->createObject(arg, op, res);

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
    arg->setParent(parent);

    m_account->createObject(arg, op, res);

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
    arg->setParent(parent);

    m_account->createObject(arg, op, res);

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
    arg->setParent(parent);

    m_account->createObject(arg, op, res);

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
    arg->setParent(parent);

    m_account->createObject(arg, op, res);

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
    arg->setParent(parent);

    m_account->createObject(arg, op, res);

    ASSERT_EQUAL(res.size(), 0u);

    ASSERT_EQUAL(Account_createObject_called,
                 m_account);
}



int main()
{
    boost::asio::io_service io_service;
    Ruleset ruleset(nullptr, io_service);

    Admintest t;

    return t.run();
}

// stubs

#include "server/Connection.h"
#include "server/Juncture.h"
#include "server/Persistence.h"
#include "server/PossessionAuthenticator.h"

#include "common/globals.h"
#include "common/id.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <cstdlib>
#include <cstdio>

#define STUB_Account_createObject
void Account::createObject(const Root & arg,
                           const Operation & op,
                           OpVector & res)
{
    Admintest::set_Account_createObject_called(this);
}

#define STUB_Account_operation
void Account::operation(const Operation & op, OpVector & res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::LOGOUT_NO) {
        this->LogoutOperation(op, res);
    }
}

#define STUB_Account_LogoutOperation
void Account::LogoutOperation(const Operation &, OpVector &)
{
    Admintest::set_Account_LogoutOperation_called(this);
}

#define STUB_Account_SetOperation
void Account::SetOperation(const Operation &, OpVector &)
{
    Admintest::set_Account_SetOperation_called(this);
}

#include "stubs/server/stubAccount.h"


#include "stubs/server/stubConnection.h"
#include "stubs/rulesets/stubLocation.h"

#include "stubs/server/stubRuleHandler.h"

#include "stubs/server/stubEntityRuleHandler.h"
#include "stubs/server/stubArchetypeRuleHandler.h"
#include "stubs/server/stubOpRuleHandler.h"
#include "stubs/server/stubPropertyRuleHandler.h"
#include "stubs/server/stubConnectableRouter.h"

#define STUB_Ruleset_modifyRule
int Ruleset::modifyRule(const std::string & class_name,
                        const Root & class_desc)
{
    Admintest::set_Ruleset_modifyRule_called();
    return Admintest::get_Ruleset_modifyRule_retval();
}

#define STUB_Ruleset_installRule
int Ruleset::installRule(const std::string & class_name,
                         const std::string & section,
                         const Root & class_desc)
{
    Admintest::set_Ruleset_installRule_called();
    return Admintest::get_Ruleset_installRule_retval();
}

#include "stubs/server/stubRuleset.h"
#include "stubs/server/stubJuncture.h"

#define STUB_ServerRouting_addObject
void ServerRouting::addObject(ConnectableRouter * obj)
{
    m_objects[obj->getIntId()] = obj;
}

#define STUB_ServerRouting_getObject
ConnectableRouter * ServerRouting::getObject(const std::string & id) const
{
    auto I = m_objects.find(integerId(id));
    if (I == m_objects.end()) {
        return 0;
    } else {
        return I->second;
    }
}
#include "stubs/server/stubServerRouting.h"
#include "stubs/server/stubPossessionAuthenticator.h"


#include "stubs/server/stubPersistence.h"
#include "stubs/rulesets/stubThing.h"

#define STUB_Entity_addToEntity
void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
    ent->setId(getId());
}
#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"

#define STUB_Link_send
void Link::send(const Operation & op) const
{
    Admintest::set_Link_sent_called();
}
#include "stubs/common/stubLink.h"



#define STUB_Inheritance_Inheritance
Inheritance::Inheritance()
    : Singleton(), noClass(0)
{
    Atlas::Objects::Entity::Anonymous root_desc;

    root_desc->setObjtype("meta");
    root_desc->setId("root");

    TypeNode * root = new TypeNode("root", root_desc);

    atlasObjects["root"] = root;
}

#define STUB_Inheritance_getType
const TypeNode* Inheritance::getType(const std::string & parent) const
{
    auto I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second;
}

#define STUB_Inheritance_getClass
const Atlas::Objects::Root& Inheritance::getClass(const std::string & parent, Visibility v)
{
    auto I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return noClass;
    }
    return I->second->description(v);
}


#define STUB_Inheritance_hasClass
bool Inheritance::hasClass(const std::string & parent)
{
    auto I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return false;
    }
    return true;
}

#define STUB_Inheritance_addChild
TypeNode* Inheritance::addChild(const Atlas::Objects::Root & obj)
{
    const std::string & child = obj->getId();
    const std::string & parent = obj->getParent();
    assert(atlasObjects.find(child) == atlasObjects.end());

    auto I = atlasObjects.find(parent);
    assert(I != atlasObjects.end());

    Element children(ListType(1, child));
    if (I->second->description(Visibility::PRIVATE)->copyAttr("children", children) == 0) {
        assert(children.isList());
        children.asList().push_back(child);
    }
    I->second->description(Visibility::PRIVATE)->setAttr("children", children);

    TypeNode * type = new TypeNode(child, obj);
    type->setParent(I->second);

    atlasObjects[child] = type;

    return type;
}
#include "stubs/common/stubInheritance.h"

Root atlasClass(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;

    r->setParent(parent);
    r->setObjtype("class");
    r->setId(name);

    return r;
}

#include "stubs/common/stubTypeNode.h"
#include "stubs/rulesets/stubBaseWorld.h"
#include "stubs/rulesets/stubAdminMind.h"
#include "stubs/rulesets/stubExternalMind.h"


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
int THOUGHT_NO = -1;
int GOAL_INFO_NO=-1;
} } }

#include <common/Shaker.h>

Shaker::Shaker()
{
}
std::string Shaker::generateSalt(size_t length)
{
    return "";
}
