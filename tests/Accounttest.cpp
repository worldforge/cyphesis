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

#include "server/Account.h"

#include "server/Connection.h"
#include "server/Lobby.h"
#include "server/ServerRouting.h"

#include "rulesets/Character.h"

#include "common/CommSocket.h"
#include "common/compose.hpp"
#include "common/debug.h"
#include "common/id.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::RootOperation;

using String::compose;

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

template <typename T>
std::ostream & operator<<(std::ostream & os,
                          const std::list<T> & sl)
{
    typename std::list<T>::const_iterator I = sl.begin();
    typename std::list<T>::const_iterator Iend = sl.end();
    os << "[";
    for (; I != Iend; ++I) {
        if (I != sl.begin()) {
            os << ", ";
        }
        os << *I;
    }
    os << "]";
    return os;
}

class Accounttest : public Cyphesis::TestBase
{
  protected:
    long m_id_counter;

    ServerRouting * m_server;
    Connection * m_connection;
    Account * m_account;

    static Entity * TestWorld_addNewEntity_ret_value;
    static Entity * TeleportAuthenticator_ret_value;
    static OpVector Link_send_sent;
    static int characterError_ret_value;
    static int Lobby_operation_called;
    static std::list<std::pair<RootOperation, LocatedEntity *> > TestWorld_message_called;
  public:
    Accounttest();

    void setup();
    void teardown();

    void test_null();
    void test_characterDestroyed();
    void test_characterDestroyed_invalid();
    void test_connectCharacter_raw_Entity();
    void test_connectCharacter_Character();
    void test_addCharacter_raw_Entity();
    void test_addCharacter_Character();
    void test_addNewCharacter_fail();
    void test_addNewCharacter_raw_Entity();
    void test_addNewCharacter_Character();
    void test_addNewCharacter_unconnected();
    void test_LogoutOperation_no_serialno();
    void test_LogoutOperation_serialno();
    void test_LogoutOperation_unconnected();
    void test_getType();
    void test_store();
    void test_addToMessage();
    void test_addToEntity();
    void test_operation_Create();
    void test_operation_Get();
    void test_operation_Imaginary();
    void test_operation_Logout();
    void test_operation_Look();
    void test_operation_Set();
    void test_operation_Talk();
    void test_operation_INVALID();
    void test_operation_Other();
    void test_CreateOperation_no_args();
    void test_CreateOperation_no_parents();
    void test_CreateOperation_good();
    void test_GetOperation();
    void test_ImaginaryOperation_no_args();
    void test_ImaginaryOperation_Root_args();
    void test_ImaginaryOperation_no_loc();
    void test_ImaginaryOperation_loc();
    void test_ImaginaryOperation_unconnected();
    void test_LookOperation_no_args();
    void test_LookOperation_unconnected();
    void test_LookOperation_no_id();
    void test_LookOperation_known_character();
    void test_LookOperation_known_account();
    void test_LookOperation_unknown();
    void test_LookOperation_possess_invalid();
    void test_LookOperation_possess_Entity();
    void test_LookOperation_possess_Character();
    void test_SetOperation_no_args();
    void test_SetOperation_no_id();
    void test_SetOperation_unowned_character();
    void test_SetOperation_empty();
    void test_SetOperation_guise();
    void test_SetOperation_height();
    void test_SetOperation_height_non_float();
    void test_SetOperation_height_no_bbox();
    void test_SetOperation_tasks_empty();
    void test_SetOperation_tasks_good();
    void test_TalkOperation_no_args();
    void test_TalkOperation_non_entity();
    void test_TalkOperation_self();
    void test_TalkOperation_loc();
    void test_TalkOperation_unconnected();
    void test_OtherOperation();
    void test_createObject_permission_error();
    void test_createObject_add_failed();
    void test_createObject_raw_Entity();
    void test_createObject_Character();
    void test_filterTasks_empty();
    void test_filterTasks_malformed_not_map();
    void test_filterTasks_malformed_no_name();
    void test_filterTasks_good();

    static Entity * get_TestWorld_addNewEntity_ret_value();
    static Entity * get_TeleportAuthenticator_ret_value();
    static void append_Link_send_sent(const RootOperation &);
    static int get_characterError_ret_value();
    static void set_Lobby_operation_called(int class_no);
    static void set_TestWorld_message_called(const RootOperation &,
                                              LocatedEntity &);
};

Entity * Accounttest::TestWorld_addNewEntity_ret_value;
Entity * Accounttest::TeleportAuthenticator_ret_value;
OpVector Accounttest::Link_send_sent;
int Accounttest::characterError_ret_value;
int Accounttest::Lobby_operation_called;
std::list<std::pair<RootOperation, LocatedEntity *> >
      Accounttest::TestWorld_message_called;

Entity * Accounttest::get_TestWorld_addNewEntity_ret_value()
{
    return TestWorld_addNewEntity_ret_value;
}

Entity * Accounttest::get_TeleportAuthenticator_ret_value()
{
    return TeleportAuthenticator_ret_value;
}

void Accounttest::append_Link_send_sent(const RootOperation & op)
{
    Link_send_sent.push_back(op);
}

int Accounttest::get_characterError_ret_value()
{
    return characterError_ret_value;
}

void Accounttest::set_Lobby_operation_called(int class_no)
{
    Lobby_operation_called = class_no;
}

void Accounttest::set_TestWorld_message_called(const RootOperation & op,
                                                LocatedEntity & entity)
{
    TestWorld_message_called.push_back(std::make_pair(op, &entity));
}

class TestAccount : public Account {
  public:
    TestAccount(Connection * conn, const std::string & username,
                                   const std::string & passwd,
                                   const std::string & id, long intId);

    ~TestAccount();

    virtual int characterError(const Operation & op,
                               const Atlas::Objects::Root & ent,
                               OpVector & res) const;

    LocatedEntity * testAddNewCharacter(const std::string & typestr,
                                        const RootEntity & ent,
                                        const RootEntity & arg);
};

Accounttest::Accounttest() : m_id_counter(0L),
                             m_server(0),
                             m_connection(0),
                             m_account(0)
{
    ADD_TEST(Accounttest::test_null);
    ADD_TEST(Accounttest::test_characterDestroyed);
    ADD_TEST(Accounttest::test_characterDestroyed_invalid);
    ADD_TEST(Accounttest::test_connectCharacter_raw_Entity);
    ADD_TEST(Accounttest::test_connectCharacter_Character);
    ADD_TEST(Accounttest::test_addCharacter_raw_Entity);
    ADD_TEST(Accounttest::test_addCharacter_Character);
    ADD_TEST(Accounttest::test_addNewCharacter_fail);
    ADD_TEST(Accounttest::test_addNewCharacter_raw_Entity);
    ADD_TEST(Accounttest::test_addNewCharacter_Character);
    ADD_TEST(Accounttest::test_addNewCharacter_unconnected);
    ADD_TEST(Accounttest::test_LogoutOperation_no_serialno);
    ADD_TEST(Accounttest::test_LogoutOperation_serialno);
    ADD_TEST(Accounttest::test_LogoutOperation_unconnected);
    ADD_TEST(Accounttest::test_getType);
    ADD_TEST(Accounttest::test_store);
    ADD_TEST(Accounttest::test_addToMessage);
    ADD_TEST(Accounttest::test_addToEntity);
    ADD_TEST(Accounttest::test_operation_Create);
    ADD_TEST(Accounttest::test_operation_Get);
    ADD_TEST(Accounttest::test_operation_Imaginary);
    ADD_TEST(Accounttest::test_operation_Logout);
    ADD_TEST(Accounttest::test_operation_Look);
    ADD_TEST(Accounttest::test_operation_Set);
    ADD_TEST(Accounttest::test_operation_Talk);
    ADD_TEST(Accounttest::test_operation_INVALID);
    ADD_TEST(Accounttest::test_operation_Other);
    ADD_TEST(Accounttest::test_CreateOperation_no_args);
    ADD_TEST(Accounttest::test_CreateOperation_no_parents);
    ADD_TEST(Accounttest::test_CreateOperation_good);
    ADD_TEST(Accounttest::test_GetOperation);
    ADD_TEST(Accounttest::test_ImaginaryOperation_no_args);
    ADD_TEST(Accounttest::test_ImaginaryOperation_Root_args);
    ADD_TEST(Accounttest::test_ImaginaryOperation_no_loc);
    ADD_TEST(Accounttest::test_ImaginaryOperation_loc);
    ADD_TEST(Accounttest::test_ImaginaryOperation_unconnected);
    ADD_TEST(Accounttest::test_LookOperation_no_args);
    ADD_TEST(Accounttest::test_LookOperation_unconnected);
    ADD_TEST(Accounttest::test_LookOperation_no_id);
    ADD_TEST(Accounttest::test_LookOperation_known_character);
    ADD_TEST(Accounttest::test_LookOperation_known_account);
    ADD_TEST(Accounttest::test_LookOperation_unknown);
    ADD_TEST(Accounttest::test_LookOperation_possess_invalid);
    ADD_TEST(Accounttest::test_LookOperation_possess_Entity);
    ADD_TEST(Accounttest::test_LookOperation_possess_Character);
    ADD_TEST(Accounttest::test_SetOperation_no_args);
    ADD_TEST(Accounttest::test_SetOperation_no_id);
    ADD_TEST(Accounttest::test_SetOperation_unowned_character);
    ADD_TEST(Accounttest::test_SetOperation_empty);
    ADD_TEST(Accounttest::test_SetOperation_guise);
    ADD_TEST(Accounttest::test_SetOperation_height);
    ADD_TEST(Accounttest::test_SetOperation_height_non_float);
    ADD_TEST(Accounttest::test_SetOperation_height_no_bbox);
    ADD_TEST(Accounttest::test_SetOperation_tasks_empty);
    ADD_TEST(Accounttest::test_SetOperation_tasks_good);
    ADD_TEST(Accounttest::test_TalkOperation_no_args);
    ADD_TEST(Accounttest::test_TalkOperation_non_entity);
    ADD_TEST(Accounttest::test_TalkOperation_self);
    ADD_TEST(Accounttest::test_TalkOperation_loc);
    ADD_TEST(Accounttest::test_TalkOperation_unconnected);
    ADD_TEST(Accounttest::test_OtherOperation);
    ADD_TEST(Accounttest::test_createObject_permission_error);
    ADD_TEST(Accounttest::test_createObject_add_failed);
    ADD_TEST(Accounttest::test_createObject_raw_Entity);
    ADD_TEST(Accounttest::test_createObject_Character);
    ADD_TEST(Accounttest::test_filterTasks_empty);
    ADD_TEST(Accounttest::test_filterTasks_malformed_not_map);
    ADD_TEST(Accounttest::test_filterTasks_malformed_no_name);
    ADD_TEST(Accounttest::test_filterTasks_good);
}

void Accounttest::setup()
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
    m_account = new TestAccount(m_connection,
                                "6c9f3236-5de7-4ba4-8b7a-b0222df0af38",
                                "fa1a03a2-a745-4033-85cb-bb694e921e62",
                                compose("%1", m_id_counter), m_id_counter++);
}

void Accounttest::teardown()
{
    delete m_server;
    delete m_account;
}

void Accounttest::test_null()
{
    ASSERT_NOT_NULL(m_account);
}

void Accounttest::test_characterDestroyed()
{
    long cid = m_id_counter++;
    Entity * c = new Entity(compose("%1", cid), cid);

    ASSERT_TRUE(m_account->m_charactersDict.empty());

    m_account->m_charactersDict.insert(std::make_pair(cid, c));

    ASSERT_NOT_EQUAL(m_account->m_charactersDict.find(cid),
                     m_account->m_charactersDict.end());
    ASSERT_EQUAL(m_account->m_charactersDict.find(cid)->second, c);

    m_account->characterDestroyed(cid);

    ASSERT_TRUE(m_account->m_charactersDict.empty());

    delete c;
}

void Accounttest::test_characterDestroyed_invalid()
{
    long cid = m_id_counter++;

    ASSERT_TRUE(m_account->m_charactersDict.empty());

    m_account->characterDestroyed(cid);

    ASSERT_TRUE(m_account->m_charactersDict.empty());

    ASSERT_EQUAL(m_account->m_charactersDict.find(cid),
                 m_account->m_charactersDict.end());
}

void Accounttest::test_connectCharacter_raw_Entity()
{
    ASSERT_TRUE(m_account->m_charactersDict.empty());

    long cid = m_id_counter++;
    Entity * c = new Entity(compose("%1", cid), cid);
    
    m_account->connectCharacter(c);

    ASSERT_TRUE(m_account->m_charactersDict.empty());

    delete c;
}

void Accounttest::test_connectCharacter_Character()
{
    ASSERT_TRUE(m_account->m_charactersDict.empty());

    long cid = m_id_counter++;
    Entity * c = new Character(compose("%1", cid), cid);
    
    m_account->connectCharacter(c);

    ASSERT_NOT_EQUAL(m_account->m_charactersDict.find(cid),
                     m_account->m_charactersDict.end());
    ASSERT_EQUAL(m_account->m_charactersDict.find(cid)->second, c);

    m_account->m_charactersDict.erase(cid);

    delete c;
}

void Accounttest::test_addCharacter_raw_Entity()
{
    long cid = m_id_counter++;
    Entity * c = new Entity(compose("%1", cid), cid);

    ASSERT_TRUE(m_account->m_charactersDict.empty());

    m_account->addCharacter(c);

    // Only objects that inherit from Character are added
    ASSERT_TRUE(m_account->m_charactersDict.empty());

    delete c;
}

void Accounttest::test_addCharacter_Character()
{
    long cid = m_id_counter++;
    Entity * c = new Character(compose("%1", cid), cid);

    ASSERT_TRUE(m_account->m_charactersDict.empty());

    m_account->addCharacter(c);

    ASSERT_NOT_EQUAL(m_account->m_charactersDict.find(cid),
                     m_account->m_charactersDict.end());
    ASSERT_EQUAL(m_account->m_charactersDict.find(cid)->second, c);

    m_account->m_charactersDict.erase(cid);

    delete c;
}

void Accounttest::test_addNewCharacter_fail()
{
    TestWorld_addNewEntity_ret_value = 0;

    ASSERT_TRUE(m_account->m_charactersDict.empty());
    ASSERT_NOT_NULL(m_account->m_connection);

    LocatedEntity * te = m_account->addNewCharacter(
          "0e657318-2424-45c9-8a3c-a61ee1303342",
          RootEntity(),
          Root());

    ASSERT_NULL(te);
}

void Accounttest::test_addNewCharacter_raw_Entity()
{
    long cid = m_id_counter++;
    TestWorld_addNewEntity_ret_value = new Entity(compose("%1", cid), cid);

    ASSERT_TRUE(m_account->m_charactersDict.empty());
    ASSERT_NOT_NULL(m_account->m_connection);

    LocatedEntity * te = m_account->addNewCharacter(
          "9e0ff22a-3b57-4703-b3fd-6ed0b8a89edc",
          RootEntity(),
          Root());

    ASSERT_NOT_NULL(te);

    // It hasn't been connected, because it is not a character
    ASSERT_TRUE(m_account->m_charactersDict.empty());

    delete TestWorld_addNewEntity_ret_value;
}

void Accounttest::test_addNewCharacter_Character()
{
    long cid = m_id_counter++;
    TestWorld_addNewEntity_ret_value = new Character(compose("%1", cid), cid);

    ASSERT_TRUE(m_account->m_charactersDict.empty());
    ASSERT_NOT_NULL(m_account->m_connection);

    LocatedEntity * te = m_account->addNewCharacter(
          "aa119eb0-ad7d-46d5-a8c3-5797ca541b6c",
          RootEntity(),
          Root());

    ASSERT_NOT_NULL(te);

    ASSERT_NOT_EQUAL(m_account->m_charactersDict.find(cid),
                     m_account->m_charactersDict.end());
    ASSERT_EQUAL(m_account->m_charactersDict.find(cid)->second, 
                 TestWorld_addNewEntity_ret_value);

    m_account->m_charactersDict.erase(cid);

    delete TestWorld_addNewEntity_ret_value;
}

void Accounttest::test_addNewCharacter_unconnected()
{
    // Make the account disconnected
    delete m_account->m_connection;
    m_account->m_connection = 0;

    long cid = m_id_counter++;
    TestWorld_addNewEntity_ret_value = new Character(compose("%1", cid), cid);

    ASSERT_TRUE(m_account->m_charactersDict.empty());
    ASSERT_NULL(m_account->m_connection);

    LocatedEntity * te = m_account->addNewCharacter(
          "3b657231-f87b-407c-99ee-9e0195475a3f",
          RootEntity(),
          Root());

    ASSERT_NULL(te);

    delete TestWorld_addNewEntity_ret_value;
    TestWorld_addNewEntity_ret_value = 0;
}

void Accounttest::test_LogoutOperation_no_serialno()
{
    Link_send_sent.clear();

    RootOperation op;
    OpVector res;

    m_account->LogoutOperation(op, res);

    ASSERT_EQUAL(Link_send_sent.size(), 1u);

    const RootOperation & reply = Link_send_sent.front();

    ASSERT_EQUAL(reply->getClassNo(), Atlas::Objects::Operation::INFO_NO);
    ASSERT_TRUE(reply->isDefaultRefno());
}

void Accounttest::test_LogoutOperation_serialno()
{
    Link_send_sent.clear();

    const long serno = 0x1fae73;

    RootOperation op;
    op->setSerialno(serno);
    OpVector res;

    m_account->LogoutOperation(op, res);

    ASSERT_EQUAL(Link_send_sent.size(), 1u);

    const RootOperation & reply = Link_send_sent.front();

    ASSERT_EQUAL(reply->getClassNo(), Atlas::Objects::Operation::INFO_NO);
    ASSERT_TRUE(!reply->isDefaultRefno());
    ASSERT_EQUAL(reply->getRefno(), serno);
}

void Accounttest::test_LogoutOperation_unconnected()
{
    Link_send_sent.clear();

    // Make the account unconnected
    delete m_account->m_connection;
    m_account->m_connection = 0;

    RootOperation op;
    OpVector res;

    m_account->LogoutOperation(op, res);

    ASSERT_EQUAL(Link_send_sent.size(), 0u);
}

void Accounttest::test_getType()
{
    const char * account_type = m_account->getType();

    ASSERT_EQUAL(std::string("account"), account_type);
}

void Accounttest::test_store()
{
    m_account->store();
}

void Accounttest::test_addToMessage()
{
    long cid = m_id_counter++;
    Entity * c = new Character(compose("%1", cid), cid);
    m_account->m_charactersDict.insert(std::make_pair(c->getIntId(), c));

    MapType data;

    m_account->m_username = "2fe6afa4-747f-490b-a292-783bf3f4520b";
    m_account->m_password = "09a6ec9f-493c-4cca-8e4a-241d15877ab4";

    m_account->addToMessage(data);

    ASSERT_EQUAL(data["username"], m_account->m_username);
    ASSERT_EQUAL(data["name"], m_account->m_username);
    ASSERT_EQUAL(data["password"], m_account->m_password);
    ASSERT_EQUAL(data["parents"], ListType(1, "account"));
    ASSERT_EQUAL(data["characters"], ListType(1, c->getId()));
    ASSERT_EQUAL(data["objtype"], "obj");
    ASSERT_EQUAL(data["id"], m_account->getId());

    m_account->m_charactersDict.erase(c->getIntId());
    delete c;
}

void Accounttest::test_addToEntity()
{
    long cid = m_id_counter++;
    Entity * c = new Character(compose("%1", cid), cid);
    m_account->m_charactersDict.insert(std::make_pair(c->getIntId(), c));

    Anonymous data;

    m_account->m_username = "36b0931c-19db-4f87-8b83-591d465af9a0";
    m_account->m_password = "980fc361-2a77-4246-8877-d57895435d6d";

    m_account->addToEntity(data);

    ASSERT_EQUAL(data->getAttr("username"), m_account->m_username);
    ASSERT_TRUE(!data->isDefaultName());
    ASSERT_EQUAL(data->getName(), m_account->m_username);
    ASSERT_EQUAL(data->getAttr("password"), m_account->m_password);
    ASSERT_TRUE(!data->isDefaultParents());
    ASSERT_EQUAL(data->getParents(), std::list<std::string>(1, "account"));
    ASSERT_EQUAL(data->getAttr("characters"), ListType(1, c->getId()));
    ASSERT_TRUE(!data->isDefaultParents());
    ASSERT_EQUAL(data->getObjtype(), "obj");
    ASSERT_EQUAL(data->getAttr("id"), m_account->getId());

    m_account->m_charactersDict.erase(c->getIntId());
    delete c;
}

void Accounttest::test_operation_Create()
{
    Atlas::Objects::Operation::Create op;
    OpVector res;

    m_account->operation(op, res);

    ASSERT_TRUE(res.empty());
}

void Accounttest::test_operation_Get()
{
    Atlas::Objects::Operation::Get op;
    OpVector res;

    m_account->operation(op, res);
}

void Accounttest::test_operation_Imaginary()
{
    Atlas::Objects::Operation::Imaginary op;
    OpVector res;

    m_account->operation(op, res);
}

void Accounttest::test_operation_Logout()
{
    Atlas::Objects::Operation::Logout op;
    OpVector res;

    m_account->operation(op, res);
}

void Accounttest::test_operation_Look()
{
    Atlas::Objects::Operation::Look op;
    OpVector res;

    m_account->operation(op, res);
}

void Accounttest::test_operation_Set()
{
    Atlas::Objects::Operation::Set op;
    OpVector res;

    m_account->operation(op, res);
}

void Accounttest::test_operation_Talk()
{
    Atlas::Objects::Operation::Talk op;
    OpVector res;

    m_account->operation(op, res);
}

void Accounttest::test_operation_INVALID()
{
    Atlas::Objects::Operation::Generic op;
    op->setType("3d6b8a1e-137c-40a6-9ec9-1b21591e4937", OP_INVALID);
    OpVector res;

    m_account->operation(op, res);
}

void Accounttest::test_operation_Other()
{
    Atlas::Objects::Operation::RootOperation op;
    OpVector res;

    m_account->operation(op, res);
}

void Accounttest::test_CreateOperation_no_args()
{
    Atlas::Objects::Operation::Create op;
    OpVector res;

    m_account->CreateOperation(op, res);
}

void Accounttest::test_CreateOperation_no_parents()
{
    Atlas::Objects::Operation::Create op;
    OpVector res;

    Anonymous create_arg;
    op->setArgs1(create_arg);

    m_account->operation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & reply = res.front();
    ASSERT_EQUAL(reply->getClassNo(), Atlas::Objects::Operation::ERROR_NO);
}

void Accounttest::test_CreateOperation_good()
{
    long cid = m_id_counter++;

    // Set up the creation so it succeeds
    characterError_ret_value = 0;
    TestWorld_addNewEntity_ret_value = new Character(compose("%1", cid), cid);

    Atlas::Objects::Operation::Create op;
    OpVector res;

    Anonymous create_arg;
    create_arg->setParents(std::list<std::string>(1, "foo"));
    op->setArgs1(create_arg);

    m_account->operation(op, res);

    ASSERT_EQUAL(res.size(), 2u);

    delete TestWorld_addNewEntity_ret_value;
    TestWorld_addNewEntity_ret_value = 0;
}

void Accounttest::test_GetOperation()
{
    Atlas::Objects::Operation::Get op;
    OpVector res;

    m_account->GetOperation(op, res);

    ASSERT_TRUE(res.empty());
}

void Accounttest::test_ImaginaryOperation_no_args()
{
    Atlas::Objects::Operation::Imaginary op;
    OpVector res;

    m_account->ImaginaryOperation(op, res);

    ASSERT_TRUE(res.empty());
}

void Accounttest::test_ImaginaryOperation_Root_args()
{
    Atlas::Objects::Operation::Imaginary op;
    OpVector res;

    Root arg;
    op->setArgs1(arg);

    m_account->ImaginaryOperation(op, res);
}

void Accounttest::test_ImaginaryOperation_no_loc()
{
    Lobby_operation_called = -1;

    Atlas::Objects::Operation::Imaginary op;
    OpVector res;

    Anonymous arg;
    op->setArgs1(arg);

    m_account->ImaginaryOperation(op, res);

    ASSERT_EQUAL(Lobby_operation_called,
                 Atlas::Objects::Operation::SIGHT_NO);
}

void Accounttest::test_ImaginaryOperation_loc()
{
    Lobby_operation_called = -1;

    Atlas::Objects::Operation::Imaginary op;
    OpVector res;

    Anonymous arg;
    arg->setLoc("foo");
    op->setArgs1(arg);

    m_account->ImaginaryOperation(op, res);

    ASSERT_EQUAL(Lobby_operation_called,
                 Atlas::Objects::Operation::SIGHT_NO);
}

void Accounttest::test_ImaginaryOperation_unconnected()
{
    Lobby_operation_called = -1;
    delete m_account->m_connection;
    m_account->m_connection = 0;

    Atlas::Objects::Operation::Imaginary op;
    OpVector res;

    Anonymous arg;
    op->setArgs1(arg);

    m_account->ImaginaryOperation(op, res);

    ASSERT_EQUAL(Lobby_operation_called, -1);
}

void Accounttest::test_LookOperation_no_args()
{
    Atlas::Objects::Operation::Look op;
    OpVector res;

    m_account->LookOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::SIGHT_NO);
    ASSERT_EQUAL(result->getArgs().size(),
                 1u);

    const Root & result_arg = result->getArgs().front();

    ASSERT_TRUE(!result_arg->isDefaultId());
    ASSERT_EQUAL(result_arg->getId(),
                 m_server->m_lobby.getId());
}

void Accounttest::test_LookOperation_unconnected()
{
    delete m_account->m_connection;
    m_account->m_connection = 0;

    Atlas::Objects::Operation::Look op;
    OpVector res;

    m_account->LookOperation(op, res);

    ASSERT_TRUE(res.empty());
}

void Accounttest::test_LookOperation_no_id()
{
    Atlas::Objects::Operation::Look op;
    OpVector res;

    Anonymous arg;
    op->setArgs1(arg);

    m_account->LookOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Accounttest::test_LookOperation_known_character()
{
    long cid = m_id_counter++;
    Entity * c = new Character(compose("%1", cid), cid);
    m_account->m_charactersDict.insert(std::make_pair(c->getIntId(), c));

    Atlas::Objects::Operation::Look op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    op->setArgs1(arg);

    m_account->LookOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::SIGHT_NO);
    ASSERT_EQUAL(result->getArgs().size(),
                 1u);

    const Root & result_arg = result->getArgs().front();

    ASSERT_TRUE(!result_arg->isDefaultId());
    ASSERT_EQUAL(result_arg->getId(),
                 c->getId());

    m_account->m_charactersDict.erase(c->getIntId());
    delete c;
}

void Accounttest::test_LookOperation_known_account()
{
    long cid = m_id_counter++;
    Account * ac = new TestAccount(0, "","", compose("%1", cid), cid);
    m_server->m_lobby.addAccount(ac);

    Atlas::Objects::Operation::Look op;
    OpVector res;

    Anonymous arg;
    arg->setId(ac->getId());
    op->setArgs1(arg);

    m_account->LookOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::SIGHT_NO);
    ASSERT_EQUAL(result->getArgs().size(),
                 1u);

    const Root & result_arg = result->getArgs().front();

    ASSERT_TRUE(!result_arg->isDefaultId());
    ASSERT_EQUAL(result_arg->getId(),
                 ac->getId());

    m_server->m_lobby.delAccount(ac);
    delete ac;
}

void Accounttest::test_LookOperation_unknown()
{
    Atlas::Objects::Operation::Look op;
    OpVector res;

    Anonymous arg;
    arg->setId("8026");
    op->setArgs1(arg);

    m_account->LookOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Accounttest::test_LookOperation_possess_invalid()
{
    TeleportAuthenticator_ret_value = 0;

    Atlas::Objects::Operation::Look op;
    OpVector res;

    Anonymous arg;
    arg->setId("8026");
    arg->setAttr("possess_key", "3efc5e84-6fc6-4c66-bd68-1eec24ba09b6");
    op->setArgs1(arg);

    m_account->LookOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);

}

void Accounttest::test_LookOperation_possess_Entity()
{
    long cid = m_id_counter++;
    Entity * c = new Entity(compose("%1", cid), cid);
    TeleportAuthenticator_ret_value = c;

    Atlas::Objects::Operation::Look op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    arg->setAttr("possess_key", "3efc5e84-6fc6-4c66-bd68-1eec24ba09b6");
    op->setArgs1(arg);

    m_account->LookOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);

    m_account->m_charactersDict.erase(c->getIntId());
    delete c;
}

void Accounttest::test_LookOperation_possess_Character()
{
    long cid = m_id_counter++;
    Entity * c = new Character(compose("%1", cid), cid);
    TeleportAuthenticator_ret_value = c;

    Atlas::Objects::Operation::Look op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    arg->setAttr("possess_key", "3efc5e84-6fc6-4c66-bd68-1eec24ba09b6");
    op->setArgs1(arg);

    m_account->LookOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::SIGHT_NO);
    ASSERT_EQUAL(result->getArgs().size(),
                 1u);

    const Root & result_arg = result->getArgs().front();

    ASSERT_TRUE(!result_arg->isDefaultId());
    ASSERT_EQUAL(result_arg->getId(),
                 c->getId());

    m_account->m_charactersDict.erase(c->getIntId());
    delete c;
}

void Accounttest::test_SetOperation_no_args()
{
    Accounttest::TestWorld_message_called.clear();

    Atlas::Objects::Operation::Set op;
    OpVector res;

    m_account->SetOperation(op, res);

    ASSERT_TRUE(res.empty());
    ASSERT_TRUE(Accounttest::TestWorld_message_called.empty());
}

void Accounttest::test_SetOperation_no_id()
{
    Accounttest::TestWorld_message_called.clear();

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_TRUE(Accounttest::TestWorld_message_called.empty());

    const RootOperation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Accounttest::test_SetOperation_unowned_character()
{
    Accounttest::TestWorld_message_called.clear();

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setId("975");
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_TRUE(Accounttest::TestWorld_message_called.empty());

    const RootOperation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Accounttest::test_SetOperation_empty()
{
    Accounttest::TestWorld_message_called.clear();

    long cid = m_id_counter++;

    Character * c = new Character(compose("%1", cid), cid);
    m_account->m_charactersDict.insert(std::make_pair(c->getIntId(), c));

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_TRUE(res.empty());
    ASSERT_TRUE(Accounttest::TestWorld_message_called.empty());

    m_account->m_charactersDict.clear();
    delete c;
}

void Accounttest::test_SetOperation_guise()
{
    Accounttest::TestWorld_message_called.clear();

    long cid = m_id_counter++;

    Character * c = new Character(compose("%1", cid), cid);
    m_account->m_charactersDict.insert(std::make_pair(c->getIntId(), c));

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    arg->setAttr("guise", "unimportant_value");
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_TRUE(res.empty());
    ASSERT_TRUE(!Accounttest::TestWorld_message_called.empty());

    const RootOperation & result =
          Accounttest::TestWorld_message_called.front().first;

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::SET_NO);
    ASSERT_EQUAL(result->getArgs().size(), 1u);
    ASSERT_TRUE(result->getArgs().front()->hasAttr("guise"));
    ASSERT_TRUE(!result->getArgs().front()->hasAttr("bbox"));
    ASSERT_TRUE(!result->getArgs().front()->hasAttr("tasks"));

    LocatedEntity * result_entity =
          Accounttest::TestWorld_message_called.front().second;

    ASSERT_EQUAL(result_entity, c);

    m_account->m_charactersDict.clear();
    delete c;
}

void Accounttest::test_SetOperation_height()
{
    Accounttest::TestWorld_message_called.clear();

    long cid = m_id_counter++;

    Character * c = new Character(compose("%1", cid), cid);
    c->m_location.m_bBox = BBox(Point3D(0,0,0), Point3D(1,1,1));
    m_account->m_charactersDict.insert(std::make_pair(c->getIntId(), c));

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    arg->setAttr("height", 2.);
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_TRUE(res.empty());
    ASSERT_TRUE(!Accounttest::TestWorld_message_called.empty());

    const RootOperation & result =
          Accounttest::TestWorld_message_called.front().first;

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::SET_NO);
    ASSERT_EQUAL(result->getArgs().size(), 1u);
    ASSERT_TRUE(!result->getArgs().front()->hasAttr("guise"));
    ASSERT_TRUE(result->getArgs().front()->hasAttr("bbox"));
    ASSERT_TRUE(!result->getArgs().front()->hasAttr("tasks"));

    // Check the resulting bbox is the right height
    Element bbox = result->getArgs().front()->getAttr("bbox");
    ASSERT_TRUE(bbox.isList());
    ASSERT_EQUAL(bbox.List().size(), 3u);
    ASSERT_TRUE(bbox.List()[2].isNum());
    ASSERT_GREATER(bbox.List()[2].asNum(), 1.9);
    ASSERT_LESS(bbox.List()[2].asNum(), 2.1);

    LocatedEntity * result_entity =
          Accounttest::TestWorld_message_called.front().second;

    ASSERT_EQUAL(result_entity, c);

    m_account->m_charactersDict.clear();
    delete c;
}

void Accounttest::test_SetOperation_height_non_float()
{
    Accounttest::TestWorld_message_called.clear();

    long cid = m_id_counter++;

    Character * c = new Character(compose("%1", cid), cid);
    c->m_location.m_bBox = BBox(Point3D(0,0,0), Point3D(1,1,1));
    m_account->m_charactersDict.insert(std::make_pair(c->getIntId(), c));

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    arg->setAttr("height", "2");
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_TRUE(res.empty());
    ASSERT_TRUE(Accounttest::TestWorld_message_called.empty());

    m_account->m_charactersDict.clear();
    delete c;
}

void Accounttest::test_SetOperation_height_no_bbox()
{
    Accounttest::TestWorld_message_called.clear();

    long cid = m_id_counter++;

    Character * c = new Character(compose("%1", cid), cid);
    m_account->m_charactersDict.insert(std::make_pair(c->getIntId(), c));

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    arg->setAttr("height", 2.);
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_TRUE(res.empty());
    ASSERT_TRUE(Accounttest::TestWorld_message_called.empty());

    m_account->m_charactersDict.clear();
    delete c;
}

void Accounttest::test_SetOperation_tasks_empty()
{
    Accounttest::TestWorld_message_called.clear();

    long cid = m_id_counter++;

    Character * c = new Character(compose("%1", cid), cid);
    m_account->m_charactersDict.insert(std::make_pair(c->getIntId(), c));

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    arg->setAttr("tasks", "invalid");
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_TRUE(res.empty());
    ASSERT_TRUE(Accounttest::TestWorld_message_called.empty());

    m_account->m_charactersDict.clear();
    delete c;
}

void Accounttest::test_SetOperation_tasks_good()
{
    Accounttest::TestWorld_message_called.clear();

    long cid = m_id_counter++;

    Character * c = new Character(compose("%1", cid), cid);
    m_account->m_charactersDict.insert(std::make_pair(c->getIntId(), c));

    Atlas::Objects::Operation::Set op;
    OpVector res;

    MapType task;
    task["mim"] = "baz";
    task["name"] = "gootle";
    ListType tasks = ListType(1, task);

    Anonymous arg;
    arg->setId(c->getId());
    arg->setAttr("tasks", tasks);
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_TRUE(res.empty());
    ASSERT_TRUE(!Accounttest::TestWorld_message_called.empty());

    const RootOperation & result =
          Accounttest::TestWorld_message_called.front().first;

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::SET_NO);
    ASSERT_EQUAL(result->getArgs().size(), 1u);
    ASSERT_TRUE(!result->getArgs().front()->hasAttr("guise"));
    ASSERT_TRUE(!result->getArgs().front()->hasAttr("bbox"));
    ASSERT_TRUE(result->getArgs().front()->hasAttr("tasks"));

    LocatedEntity * result_entity =
          Accounttest::TestWorld_message_called.front().second;

    ASSERT_EQUAL(result_entity, c);

    m_account->m_charactersDict.clear();
    delete c;
}

void Accounttest::test_TalkOperation_no_args()
{
    Atlas::Objects::Operation::Talk op;
    OpVector res;

    m_account->TalkOperation(op, res);


    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & reply = res.front();
    ASSERT_EQUAL(reply->getClassNo(), Atlas::Objects::Operation::ERROR_NO);
}

void Accounttest::test_TalkOperation_non_entity()
{
    Atlas::Objects::Operation::Talk op;
    OpVector res;

    Root talk_arg;
    op->setArgs1(talk_arg);

    m_account->TalkOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & reply = res.front();
    ASSERT_EQUAL(reply->getClassNo(), Atlas::Objects::Operation::ERROR_NO);
}

void Accounttest::test_TalkOperation_self()
{
    Lobby_operation_called = -1;

    Atlas::Objects::Operation::Talk op;
    OpVector res;

    Anonymous talk_arg;
    op->setArgs1(talk_arg);

    m_account->TalkOperation(op, res);

    ASSERT_TRUE(res.empty());

    ASSERT_EQUAL(Lobby_operation_called, Atlas::Objects::Operation::SOUND_NO);
}

void Accounttest::test_TalkOperation_loc()
{
    Lobby_operation_called = -1;

    Atlas::Objects::Operation::Talk op;
    OpVector res;

    Anonymous talk_arg;
    talk_arg->setLoc("6273");
    op->setArgs1(talk_arg);

    m_account->TalkOperation(op, res);

    ASSERT_TRUE(res.empty());

    ASSERT_EQUAL(Lobby_operation_called, Atlas::Objects::Operation::SOUND_NO);
}

void Accounttest::test_TalkOperation_unconnected()
{
    delete m_account->m_connection;
    m_account->m_connection = 0;

    Lobby_operation_called = -1;

    Atlas::Objects::Operation::Talk op;
    OpVector res;

    Anonymous talk_arg;
    op->setArgs1(talk_arg);

    m_account->TalkOperation(op, res);

    ASSERT_TRUE(res.empty());

    ASSERT_EQUAL(Lobby_operation_called, -1);
}


void Accounttest::test_OtherOperation()
{
    Atlas::Objects::Operation::RootOperation op;
    OpVector res;

    m_account->OtherOperation(op, res);
}

void Accounttest::test_createObject_permission_error()
{
    characterError_ret_value = -1;

    Anonymous arg;
    Atlas::Objects::Operation::Create op;
    OpVector res;

    m_account->createObject("foO", arg, op, res);

    ASSERT_TRUE(res.empty());

    // addNewCharacter() would have put it here if it was created
    ASSERT_TRUE(m_account->m_charactersDict.empty());
}

void Accounttest::test_createObject_add_failed()
{
    characterError_ret_value = 0;
    TestWorld_addNewEntity_ret_value = 0;

    Anonymous arg;
    Atlas::Objects::Operation::Create op;
    OpVector res;

    m_account->createObject("foO", arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation & reply = res.front();
    ASSERT_EQUAL(reply->getClassNo(), Atlas::Objects::Operation::ERROR_NO);

    ASSERT_TRUE(m_account->m_charactersDict.empty());
}

void Accounttest::test_createObject_raw_Entity()
{
    long cid = m_id_counter++;

    characterError_ret_value = 0;
    TestWorld_addNewEntity_ret_value = new Entity(compose("%1", cid), cid);

    Anonymous arg;
    Atlas::Objects::Operation::Create op;
    OpVector res;

    m_account->createObject("foO", arg, op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_TRUE(m_account->m_charactersDict.empty());

    delete TestWorld_addNewEntity_ret_value;
    TestWorld_addNewEntity_ret_value = 0;
}

void Accounttest::test_createObject_Character()
{
    long cid = m_id_counter++;

    characterError_ret_value = 0;
    TestWorld_addNewEntity_ret_value = new Character(compose("%1", cid), cid);

    Anonymous arg;
    Atlas::Objects::Operation::Create op;
    OpVector res;

    m_account->createObject("foO", arg, op, res);

    ASSERT_EQUAL(res.size(), 2u);
    ASSERT_EQUAL(m_account->m_charactersDict.size(), 1u);

    const RootOperation & info = res.front();
    ASSERT_EQUAL(info->getArgs().size(), 1u);

    const Root & info_arg = info->getArgs().front();
    ASSERT_TRUE(!info_arg->isDefaultId());

    long info_id = integerId(info_arg->getId());
    ASSERT_NOT_EQUAL(m_account->m_charactersDict.find(info_id),
                     m_account->m_charactersDict.end());
}

void Accounttest::test_filterTasks_empty()
{
    ListType tasks;
    Anonymous result;

    int ret = m_account->filterTasks(tasks, result);

    ASSERT_EQUAL(ret, 0);

    ASSERT_EQUAL(result->getAttr("tasks"), ListType());
}

void Accounttest::test_filterTasks_malformed_not_map()
{
    ListType tasks(1, std::string("bar"));
    Anonymous result;

    int ret = m_account->filterTasks(tasks, result);

    ASSERT_EQUAL(ret, -1);

    ASSERT_EQUAL(result->hasAttr("tasks"), false);
}

void Accounttest::test_filterTasks_malformed_no_name()
{
    MapType task;
    task["mim"] = "baz";
    ListType tasks = ListType(1, task);
    Anonymous result;

    int ret = m_account->filterTasks(tasks, result);

    ASSERT_EQUAL(ret, -1);

    ASSERT_EQUAL(result->hasAttr("tasks"), false);
}

void Accounttest::test_filterTasks_good()
{
    MapType task;
    task["mim"] = "baz";
    task["name"] = "gootle";
    ListType tasks = ListType(1, task);
    Anonymous result;

    int ret = m_account->filterTasks(tasks, result);

    ASSERT_EQUAL(ret, 0);

    ASSERT_EQUAL(result->getAttr("tasks"), tasks);
}

TestAccount::TestAccount(Connection * conn, const std::string & username,
                         const std::string & passwd,
                         const std::string & id, long intId) :
      Account(conn, username, passwd, id, intId)
{
}

TestAccount::~TestAccount()
{
    delete m_connection;
}

int TestAccount::characterError(const Operation & op,
                                const Atlas::Objects::Root & ent,
                                OpVector & res) const
{
    return Accounttest::get_characterError_ret_value();
}

LocatedEntity * TestAccount::testAddNewCharacter(const std::string & typestr,
                                                 const RootEntity & ent,
                                                 const RootEntity & arg)
{
    return addNewCharacter(typestr, ent, arg);
}

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
    Accounttest::set_TestWorld_message_called(op, ent);
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    Entity * ne = Accounttest::get_TestWorld_addNewEntity_ret_value();
    if (ne != 0) {
        ne->m_location.m_loc = &m_gameWorld;
        ne->m_location.m_pos = Point3D(0,0,0);
        assert(ne->m_location.isValid());
    }
    return ne;
}

int main()
{
    Accounttest t;

    return t.run();
}

// stubs

#include "server/Connection.h"
#include "server/Lobby.h"
#include "server/Persistence.h"
#include "server/TeleportAuthenticator.h"

#include "common/globals.h"
#include "common/id.h"
#include "common/log.h"

#include <cstdlib>

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

ServerRouting::ServerRouting(BaseWorld & wrld,
                             const std::string & ruleset,
                             const std::string & name,
                             const std::string & id, long intId,
                             const std::string & lId, long lIntId) :
        Router(id, intId),
        m_svrRuleset(ruleset), m_svrName(name),
        m_numClients(0), m_world(wrld),
        m_lobby(*new Lobby(*this, lId, lIntId))
{
}

ServerRouting::~ServerRouting()
{
    delete &m_world;
    delete &m_lobby;
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
    Entity * ne = Accounttest::get_TeleportAuthenticator_ret_value();
    return ne;
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

Lobby::Lobby(ServerRouting & s, const std::string & id, long intId) :
       Router(id, intId),
       m_server(s)
{
}

Lobby::~Lobby()
{
}

void Lobby::delAccount(Account * ac)
{
    m_accounts.erase(ac->getId());
}

void Lobby::addToMessage(MapType & omap) const
{
}

void Lobby::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
    ent->setId(getId());
}

void Lobby::addAccount(Account * ac)
{
    m_accounts[ac->getId()] = ac;
}

void Lobby::externalOperation(const Operation & op, Link &)
{
}

void Lobby::operation(const Operation & op, OpVector & res)
{
    Accounttest::set_Lobby_operation_called(op->getClassNo());
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

int Character::linkExternal(Link * link)
{
    return 0;
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
    Accounttest::append_Link_send_sent(op);
}

void Link::disconnect()
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

#include <common/Shaker.h>

Shaker::Shaker()
{
}

std::string Shaker::generateSalt(size_t length)
{
    return "";
}
