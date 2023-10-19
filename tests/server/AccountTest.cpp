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

#include "../TestBase.h"
#include "../TestWorld.h"

#include "server/Account.h"

#include "server/Connection.h"
#include "server/Lobby.h"
#include "server/ServerRouting.h"
#include "rules/simulation/Entity.h"

#include "common/CommSocket.h"
#include "common/compose.hpp"
#include "common/debug.h"
#include "common/id.h"
#include "../DatabaseNull.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
#include <server/Persistence.h>
#include <server/PossessionAuthenticator.h>
#include "rules/AtlasProperties.h"
#include "rules/PhysicalProperties.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::RootOperation;

using String::compose;


std::ostream& operator<<(std::ostream& os,
                         const std::map<std::string, Ref<LocatedEntity>>::const_iterator&)
{
    os << "[iterator]";
    return os;
}

std::ostream& operator<<(std::ostream& os,
                         const std::map<long, Ref<LocatedEntity>>::const_iterator&)
{
    os << "[iterator]";
    return os;
}

std::ostream& operator<<(std::ostream& os,
                         const std::map<const long, const LocatedEntity*>::const_iterator&)
{
    os << "[iterator]";
    return os;
}

std::ostream& operator<<(std::ostream& os,
                         const std::map<const long, LocatedEntity*>::const_iterator&)
{
    os << "[iterator]";
    return os;
}

std::ostream& operator<<(std::ostream& os,
                         const Ref<LocatedEntity>& e)
{
    os << e->describeEntity();
    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os,
                         const std::list<T>& sl)
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

class TestAccount;

PossessionAuthenticator* possessionAuthenticator = new PossessionAuthenticator();

class Accounttest : public Cyphesis::TestBase
{
    protected:
        long m_id_counter;

        DatabaseNull m_database;
        Ref<Entity> m_gw;
        Persistence* m_persistence;
        TestWorld* m_world;
        ServerRouting* m_server;
        Connection* m_connection;
        TestAccount* m_account;

        static Ref<Entity> TestWorld_addNewEntity_ret_value;
        static Ref<Entity> TeleportAuthenticator_ret_value;
        static OpVector Link_send_sent;
        static int characterError_ret_value;
        static int Lobby_operation_called;
        static std::list<std::pair<RootOperation, LocatedEntity*> > TestWorld_message_called;
    public:
        Accounttest();

        void setup();

        void teardown();

        void test_null();

        void test_characterDestroyed();

        void test_characterDestroyed_invalid();

        void test_connectCharacter_raw_Entity();

        void test_addCharacter_raw_Entity();

        void test_LogoutOperation_unknown();

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

        void test_CreateOperation_no_parent();

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


        static Ref<Entity> get_TestWorld_addNewEntity_ret_value();

        static Ref<Entity> get_TeleportAuthenticator_ret_value();

        static void append_Link_send_sent(const RootOperation&);

        static int get_characterError_ret_value();

        static void set_Lobby_operation_called(int class_no);

        static void set_TestWorld_message_called(const RootOperation&,
                                                 LocatedEntity&);
};

Ref<Entity> Accounttest::TestWorld_addNewEntity_ret_value;
Ref<Entity> Accounttest::TeleportAuthenticator_ret_value;
OpVector Accounttest::Link_send_sent;
int Accounttest::characterError_ret_value;
int Accounttest::Lobby_operation_called;
std::list<std::pair<RootOperation, LocatedEntity*> >
        Accounttest::TestWorld_message_called;

Ref<Entity> Accounttest::get_TestWorld_addNewEntity_ret_value()
{
    return TestWorld_addNewEntity_ret_value;
}

Ref<Entity> Accounttest::get_TeleportAuthenticator_ret_value()
{
    return TeleportAuthenticator_ret_value;
}

void Accounttest::append_Link_send_sent(const RootOperation& op)
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

void Accounttest::set_TestWorld_message_called(const RootOperation& op,
                                               LocatedEntity& entity)
{
    TestWorld_message_called.push_back(std::make_pair(op, &entity));
}

class TestAccount : public Account
{
    public:
        TestAccount(Connection* conn, const std::string& username,
                    const std::string& passwd,
                    RouterId id);

        ~TestAccount();

        virtual int characterError(const Operation& op,
                                   const Atlas::Objects::Root& ent,
                                   OpVector& res) const;


        void test_processExternalOperation(const Operation& op, OpVector& res)
        {
            processExternalOperation(op, res);
        }
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
    ADD_TEST(Accounttest::test_addCharacter_raw_Entity);
    ADD_TEST(Accounttest::test_LogoutOperation_unknown);
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
    ADD_TEST(Accounttest::test_CreateOperation_no_parent);
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
    ADD_TEST(Accounttest::test_TalkOperation_no_args);
    ADD_TEST(Accounttest::test_TalkOperation_non_entity);
    ADD_TEST(Accounttest::test_TalkOperation_self);
    ADD_TEST(Accounttest::test_TalkOperation_loc);
    ADD_TEST(Accounttest::test_TalkOperation_unconnected);
    ADD_TEST(Accounttest::test_OtherOperation);
}

void Accounttest::setup()
{


    m_persistence = new Persistence(m_database);
    m_gw = new Entity(m_id_counter++);
    TestWorld::extension.messageFn = &Accounttest::set_TestWorld_message_called;
    TestWorld::extension.addNewEntityFn = [&](const std::string&,
                                              const Atlas::Objects::Entity::RootEntity&) {
        Ref<Entity> ne = Accounttest::get_TestWorld_addNewEntity_ret_value();
        if (ne != nullptr) {
            ne->m_parent = m_gw.get();
            ne->requirePropertyClassFixed<PositionProperty>().data() = Point3D(0, 0, 0);
        }
        return ne;
    };
    m_world = new TestWorld(m_gw);

    m_server = new ServerRouting(*m_world,
                                 *m_persistence,
                                 "5529d7a4-0158-4dc1-b4a5-b5f260cac635",
                                 "bad621d4-616d-4faf-b9e6-471d12b139a9",
                                 m_id_counter++,
                                 AssetsHandler({}));
    m_connection = new Connection(*(CommSocket*) 0, *m_server,
                                  "8d18a4e8-f14f-4a46-997e-ada120d5438f",
                                  m_id_counter++);
    m_account = new TestAccount(m_connection,
                                "6c9f3236-5de7-4ba4-8b7a-b0222df0af38",
                                "fa1a03a2-a745-4033-85cb-bb694e921e62",
                                m_id_counter++);
    TestWorld_addNewEntity_ret_value = nullptr;
    TeleportAuthenticator_ret_value = nullptr;
}

void Accounttest::teardown()
{
    delete m_world;
    delete m_server;
    delete m_account;
    delete m_persistence;
}

void Accounttest::test_null()
{
    ASSERT_NOT_NULL(m_account);
}

void Accounttest::test_characterDestroyed()
{
    long cid = m_id_counter++;
    Ref<LocatedEntity> c = new Entity(cid);

    ASSERT_TRUE(m_account->m_charactersDict.empty());

    m_account->m_charactersDict.emplace(cid, c);

    ASSERT_NOT_EQUAL(m_account->m_charactersDict.find(cid),
                     m_account->m_charactersDict.end());
    ASSERT_EQUAL(m_account->m_charactersDict.find(cid)->second, c);

    m_account->characterDestroyed(cid);

    ASSERT_TRUE(m_account->m_charactersDict.empty());


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

    OpVector res;
    long cid = m_id_counter++;
    Ref<Entity> c = new Entity(cid);

    m_account->connectCharacter(c.get(), res);

    //Connecting a character doesn't mean that it's added to the account.
    ASSERT_TRUE(m_account->m_charactersDict.empty());


}

void Accounttest::test_addCharacter_raw_Entity()
{
    long cid = m_id_counter++;
    Ref<Entity> c = new Entity(cid);

    ASSERT_TRUE(m_account->m_charactersDict.empty());

    m_account->addCharacter(c.get());

    ASSERT_NOT_EQUAL(m_account->m_charactersDict.find(cid),
                     m_account->m_charactersDict.end());
    ASSERT_EQUAL(m_account->m_charactersDict.find(cid)->second, c.get());


}


void Accounttest::test_LogoutOperation_unknown()
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

void Accounttest::test_LogoutOperation_no_serialno()
{
    Link_send_sent.clear();

    RootOperation op;
    OpVector res;

    m_account->LogoutOperation(op, res);

    ASSERT_EQUAL(Link_send_sent.size(), 1u);

    const RootOperation& reply = Link_send_sent.front();

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

    const RootOperation& reply = Link_send_sent.front();

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
    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Accounttest::test_getType()
{
    const char* account_type = m_account->getType();

    ASSERT_EQUAL(std::string("account"), account_type);
}

void Accounttest::test_store()
{
    m_account->store();
}

void Accounttest::test_addToMessage()
{
    long cid = m_id_counter++;
    Ref<Entity> c = new Entity(cid);
    m_account->m_charactersDict.emplace(c->getIntId(), c);

    MapType data;

    m_account->m_username = "2fe6afa4-747f-490b-a292-783bf3f4520b";
    m_account->m_password = "09a6ec9f-493c-4cca-8e4a-241d15877ab4";

    m_account->addToMessage(data);

    ASSERT_EQUAL(data["username"], m_account->m_username);
    ASSERT_EQUAL(data["name"], m_account->m_username);
    ASSERT_EQUAL(data["password"], m_account->m_password);
    ASSERT_EQUAL(data["parent"], "account");
    ASSERT_EQUAL(data["characters"], ListType(1, c->getId()));
    ASSERT_EQUAL(data["objtype"], "obj");
    ASSERT_EQUAL(data["id"], m_account->getId());

    m_account->m_charactersDict.erase(c->getIntId());

}

void Accounttest::test_addToEntity()
{
    long cid = m_id_counter++;
    Ref<Entity> c = new Entity(cid);
    m_account->m_charactersDict.emplace(c->getIntId(), c);

    Anonymous data;

    m_account->m_username = "36b0931c-19db-4f87-8b83-591d465af9a0";
    m_account->m_password = "980fc361-2a77-4246-8877-d57895435d6d";

    m_account->addToEntity(data);

    ASSERT_EQUAL(data->getAttr("username"), m_account->m_username);
    ASSERT_TRUE(!data->isDefaultName());
    ASSERT_EQUAL(data->getName(), m_account->m_username);
    ASSERT_EQUAL(data->getAttr("password"), m_account->m_password);
    ASSERT_TRUE(!data->isDefaultParent());
    ASSERT_EQUAL(data->getParent(), "account");
    ASSERT_EQUAL(data->getAttr("characters"), ListType(1, c->getId()));
    ASSERT_TRUE(!data->isDefaultParent());
    ASSERT_EQUAL(data->getObjtype(), "obj");
    ASSERT_EQUAL(data->getAttr("id"), m_account->getId());

    m_account->m_charactersDict.erase(c->getIntId());

}

void Accounttest::test_operation_Create()
{
    Atlas::Objects::Operation::Create op;
    OpVector res;

    m_account->test_processExternalOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);
    ASSERT_EQUAL(res.front()->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Accounttest::test_operation_Get()
{
    Atlas::Objects::Operation::Get op;
    OpVector res;

    m_account->test_processExternalOperation(op, res);
}

void Accounttest::test_operation_Imaginary()
{
    Atlas::Objects::Operation::Imaginary op;
    OpVector res;

    m_account->test_processExternalOperation(op, res);
}

void Accounttest::test_operation_Logout()
{
    Atlas::Objects::Operation::Logout op;
    OpVector res;

    m_account->test_processExternalOperation(op, res);
}

void Accounttest::test_operation_Look()
{
    Atlas::Objects::Operation::Look op;
    OpVector res;

    m_account->test_processExternalOperation(op, res);
}

void Accounttest::test_operation_Set()
{
    Atlas::Objects::Operation::Set op;
    OpVector res;

    m_account->test_processExternalOperation(op, res);
}

void Accounttest::test_operation_Talk()
{
    Atlas::Objects::Operation::Talk op;
    OpVector res;

    m_account->test_processExternalOperation(op, res);
}

void Accounttest::test_operation_INVALID()
{
    Atlas::Objects::Operation::Generic op;
    op->setType("3d6b8a1e-137c-40a6-9ec9-1b21591e4937", OP_INVALID);
    OpVector res;

    m_account->test_processExternalOperation(op, res);
}

void Accounttest::test_operation_Other()
{
    Atlas::Objects::Operation::RootOperation op;
    OpVector res;

    m_account->test_processExternalOperation(op, res);
}

void Accounttest::test_CreateOperation_no_args()
{
    Atlas::Objects::Operation::Create op;
    OpVector res;

    m_account->CreateOperation(op, res);
}

void Accounttest::test_CreateOperation_no_parent()
{
    Atlas::Objects::Operation::Create op;
    OpVector res;

    Anonymous create_arg;
    op->setArgs1(create_arg);

    m_account->test_processExternalOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation& reply = res.front();
    ASSERT_EQUAL(reply->getClassNo(), Atlas::Objects::Operation::ERROR_NO);
}

void Accounttest::test_CreateOperation_good()
{
//    long cid = m_id_counter++;
//
//    // Set up the creation so it succeeds
//    characterError_ret_value = 0;
//    TestWorld_addNewEntity_ret_value = new Entity(cid);
//
//    Atlas::Objects::Operation::Create op;
//    OpVector res;
//
//    Anonymous create_arg;
//    create_arg->setParent("foo");
//    create_arg->setAttr("spawn_name", "foo");
//    op->setArgs1(create_arg);
//
//    m_account->test_processExternalOperation(op, res);
//
//    ASSERT_EQUAL(res.size(), 1u);
//    ASSERT_EQUAL(Atlas::Objects::Operation::SIGHT_NO, res.front()->getClassNo());
//
//
//    TestWorld_addNewEntity_ret_value = 0;
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

    const RootOperation& result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::SIGHT_NO);
    ASSERT_EQUAL(result->getArgs().size(),
                 1u);

    const Root& result_arg = result->getArgs().front();

    ASSERT_TRUE(!result_arg->isDefaultId());
    ASSERT_EQUAL(result_arg->getId(),
                 m_server->getLobby().getId());
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

    const RootOperation& result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
}

void Accounttest::test_LookOperation_known_character()
{
    long cid = m_id_counter++;
    Ref<Entity> c = new Entity(cid);
    m_account->m_charactersDict.emplace(c->getIntId(), c);

    Atlas::Objects::Operation::Look op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    op->setArgs1(arg);

    m_account->LookOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation& result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::SIGHT_NO);
    ASSERT_EQUAL(result->getArgs().size(),
                 1u);

    const Root& result_arg = result->getArgs().front();

    ASSERT_TRUE(!result_arg->isDefaultId());
    ASSERT_EQUAL(result_arg->getId(),
                 c->getId());

    m_account->m_charactersDict.erase(c->getIntId());

}

void Accounttest::test_LookOperation_known_account()
{
    long cid = m_id_counter++;
    Account* ac = new TestAccount(0, "", "", cid);
    m_server->getLobby().addAccount(ac);

    Atlas::Objects::Operation::Look op;
    OpVector res;

    Anonymous arg;
    arg->setId(ac->getId());
    op->setArgs1(arg);

    m_account->LookOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation& result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::SIGHT_NO);
    ASSERT_EQUAL(result->getArgs().size(),
                 1u);

    const Root& result_arg = result->getArgs().front();

    ASSERT_TRUE(!result_arg->isDefaultId());
    ASSERT_EQUAL(result_arg->getId(),
                 ac->getId());

    m_server->getLobby().removeAccount(ac);
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

    const RootOperation& result = res.front();

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

    const RootOperation& result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);

}

void Accounttest::test_LookOperation_possess_Entity()
{
    long cid = m_id_counter++;
    Ref<Entity> c = new Entity(cid);
    TeleportAuthenticator_ret_value = c;

    Atlas::Objects::Operation::Look op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    arg->setAttr("possess_key", "3efc5e84-6fc6-4c66-bd68-1eec24ba09b6");
    op->setArgs1(arg);

    m_account->LookOperation(op, res);

    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation& result = res.front();

    ASSERT_EQUAL(result->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);

    m_account->m_charactersDict.erase(c->getIntId());

}

void Accounttest::test_LookOperation_possess_Character()
{
//    long cid = m_id_counter++;
//    Ref<Entity>  c = new Entity(cid);
//    TeleportAuthenticator_ret_value = c;
//
//    Atlas::Objects::Operation::Look op;
//    OpVector res;
//
//    Anonymous arg;
//    arg->setId(c->getId());
//    arg->setAttrValue("possess_key", "3efc5e84-6fc6-4c66-bd68-1eec24ba09b6");
//    op->setArgs1(arg);
//
//    m_account->LookOperation(op, res);
//
//    ASSERT_EQUAL(res.size(), 1u);
//
//    const RootOperation & result = res.front();
//
//    ASSERT_EQUAL(result->getClassNo(),
//                 Atlas::Objects::Operation::SIGHT_NO);
//    ASSERT_EQUAL(result->getArgs().size(),
//                 1u);
//
//    const Root & result_arg = result->getArgs().front();
//
//    ASSERT_TRUE(!result_arg->isDefaultId());
//    ASSERT_EQUAL(result_arg->getId(),
//                 c->getId());
//
//    m_account->m_charactersDict.erase(c->getIntId());

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

    ASSERT_EQUAL(res.size(), 0u);
    ASSERT_TRUE(Accounttest::TestWorld_message_called.empty());

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

    ASSERT_EQUAL(res.size(), 0u);
    ASSERT_TRUE(Accounttest::TestWorld_message_called.empty());
}

void Accounttest::test_SetOperation_empty()
{
    Accounttest::TestWorld_message_called.clear();

    long cid = m_id_counter++;

    Ref<Entity> c = new Entity(cid);
    m_account->m_charactersDict.emplace(c->getIntId(), c);

    Atlas::Objects::Operation::Set op;
    OpVector res;

    Anonymous arg;
    arg->setId(c->getId());
    op->setArgs1(arg);

    m_account->SetOperation(op, res);

    ASSERT_TRUE(res.empty());
    ASSERT_TRUE(Accounttest::TestWorld_message_called.empty());

    m_account->m_charactersDict.clear();

}


void Accounttest::test_TalkOperation_no_args()
{
    Atlas::Objects::Operation::Talk op;
    OpVector res;

    m_account->TalkOperation(op, res);


    ASSERT_EQUAL(res.size(), 1u);

    const RootOperation& reply = res.front();
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

    const RootOperation& reply = res.front();
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


TestAccount::TestAccount(Connection* conn, const std::string& username,
                         const std::string& passwd,
                         RouterId id) :
        Account(conn, username, passwd, std::move(id))
{
}

TestAccount::~TestAccount()
{
    delete m_connection;
}

int TestAccount::characterError(const Operation& op,
                                const Atlas::Objects::Root& ent,
                                OpVector& res) const
{
    return Accounttest::get_characterError_ret_value();
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
#include "server/PossessionAuthenticator.h"

#include "common/globals.h"
#include "common/id.h"
#include "common/log.h"

#include <cstdlib>

#include "../stubs/server/stubConnection.h"
#include "../stubs/server/stubConnectableRouter.h"
#include "../stubs/common/stubAssetsHandler.h"

#define STUB_ServerRouting_ServerRouting

ServerRouting::ServerRouting(BaseWorld& wrld,
                             Persistence& persistence,
                             std::string ruleset,
                             std::string name,
                             RouterId lobbyId,
                             AssetsHandler assetsHandler) :
        m_svrRuleset(ruleset), m_svrName(name),
        m_lobby(new Lobby(*this, lobbyId)),
        m_numClients(0),
        m_world(wrld),
        m_persistence(persistence),
        m_assetsHandler(assetsHandler)
{
}

#include "../stubs/server/stubServerRouting.h"

#define STUB_PossessionAuthenticator_authenticatePossession

Ref<LocatedEntity> PossessionAuthenticator::authenticatePossession(const std::string& entity_id,
                                                                   const std::string& possess_key)
{
    Ref<Entity> ne = Accounttest::get_TeleportAuthenticator_ret_value();
    return ne;
}

#include "../stubs/server/stubPossessionAuthenticator.h"
#include "../stubs/server/stubPersistence.h"


Lobby::Lobby(ServerRouting& s, RouterId id) :
        Router(id),
        m_server(s)
{
}

Lobby::~Lobby()
{
}

void Lobby::removeAccount(ConnectableRouter* ac)
{
    m_accounts.erase(ac->getId());
}

void Lobby::addToMessage(MapType& omap) const
{
}

void Lobby::addToEntity(const Atlas::Objects::Entity::RootEntity& ent) const
{
    ent->setId(getId());
}

void Lobby::addAccount(ConnectableRouter* ac)
{
    m_accounts[ac->getId()] = ac;
}

void Lobby::externalOperation(const Operation& op, Link&)
{
}

void Lobby::operation(const Operation& op, OpVector& res)
{
    Accounttest::set_Lobby_operation_called(op->getClassNo());
}

#include "../stubs/rules/simulation/stubThing.h"
#include "../stubs/common/stubVariable.h"
#include "../stubs/common/stubMonitors.h"
#include "../stubs/common/stubid.h"


#define STUB_Entity_addToEntity

void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity& ent) const
{
    ent->setId(getId());
}

#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stubDatabase.h"

#define STUB_Link_send

void Link::send(const Operation& op) const
{
    Accounttest::append_Link_send_sent(op);
}

void Link::send(const OpVector& ops) const
{
}

#include "../stubs/common/stubLink.h"
#include "../stubs/common/stubcustom.h"
#include "../stubs/common/stubProperty.h"
#include "../stubs/rules/simulation/stubBaseWorld.h"
#include "../stubs/rules/simulation/stubExternalMind.h"
#include "../stubs/rules/simulation/stubMindsProperty.h"


#define STUB_Router_error

void Router::error(const Operation& op,
                   const std::string& errstring,
                   OpVector& res,
                   const std::string& to) const
{
    res.push_back(Atlas::Objects::Operation::Error());
}

#define STUB_Router_clientError

void Router::clientError(const Operation& op,
                         const std::string& errstring,
                         OpVector& res,
                         const std::string& to) const
{
    res.push_back(Atlas::Objects::Operation::Error());
}

#include "../stubs/common/stubRouter.h"

#include "../stubs/rules/stubLocation.h"
#include "../stubs/common/stublog.h"


bool database_flag = false;

#include <common/Shaker.h>

Shaker::Shaker()
{
}

std::string Shaker::generateSalt(size_t length)
{
    return "";
}
#include "../stubs/rules/stubPhysicalProperties.h"

