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

#include "../TestBase.h"

#include "server/Lobby.h"

#include "server/Connection.h"
#include "server/Account.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

class TestAccount : public Account
{
    public:
        TestAccount() : Account(0, "bob", "foo", 2)
        {}

        virtual int characterError(const Operation&,
                                   const Atlas::Objects::Root& ent,
                                   OpVector& res) const
        {
            return false;
        }
};

class Lobbytest : public Cyphesis::TestBase
{
    private:
        Lobby* m_lobby;
    public:
        Lobbytest();

        void setup();

        void teardown();

        void test_constructor();

        void test_addAccount();

        void test_delAccount();

        void test_delAccount_empty();

        void test_operation();

        void test_addAccount_connected();

        void test_operation_connected();

        void test_operation_connected_other();

        void test_operation_account();

        void test_addToMessage();

        void test_addToEntity();
};

Lobbytest::Lobbytest()
{
    ADD_TEST(Lobbytest::test_constructor);
    ADD_TEST(Lobbytest::test_addAccount);
    ADD_TEST(Lobbytest::test_delAccount);
    ADD_TEST(Lobbytest::test_delAccount_empty);
    ADD_TEST(Lobbytest::test_operation);
    ADD_TEST(Lobbytest::test_addAccount_connected);
    ADD_TEST(Lobbytest::test_operation_connected);
    ADD_TEST(Lobbytest::test_operation_connected_other);
    ADD_TEST(Lobbytest::test_operation_account);
    ADD_TEST(Lobbytest::test_addToMessage);
    ADD_TEST(Lobbytest::test_addToEntity);
}

void Lobbytest::setup()
{
    m_lobby = new Lobby(*(ServerRouting*) 0, 1);
}

void Lobbytest::teardown()
{
    delete m_lobby;
}

void Lobbytest::test_constructor()
{
}

void Lobbytest::test_addAccount()
{
    assert(m_lobby->getAccounts().size() == 0);

    TestAccount tac;
    m_lobby->addAccount(&tac);

    assert(m_lobby->getAccounts().size() == 1);
}

void Lobbytest::test_delAccount()
{
    assert(m_lobby->getAccounts().size() == 0);

    TestAccount tac;

    m_lobby->addAccount(&tac);

    assert(m_lobby->getAccounts().size() == 1);

    m_lobby->removeAccount(&tac);

    assert(m_lobby->getAccounts().size() == 0);
}

void Lobbytest::test_delAccount_empty()
{
    assert(m_lobby->getAccounts().size() == 0);
    TestAccount tac;
    m_lobby->removeAccount(&tac);

    assert(m_lobby->getAccounts().size() == 0);
}

void Lobbytest::test_operation()
{
    Atlas::Objects::Operation::RootOperation op;
    OpVector res;
    m_lobby->operation(op, res);
}

void Lobbytest::test_addAccount_connected()
{
    Connection conn(*(CommSocket*) 0,
                    *(ServerRouting*) 0,
                    "foo", 3);
    TestAccount tac;

    tac.setConnection(&conn);

    m_lobby->addAccount(&tac);

    Atlas::Objects::Operation::RootOperation op;
    OpVector res;
    m_lobby->operation(op, res);
}

void Lobbytest::test_operation_connected()
{
    Connection conn(*(CommSocket*) 0,
                    *(ServerRouting*) 0,
                    "foo", 3);
    TestAccount tac;

    tac.setConnection(&conn);

    m_lobby->addAccount(&tac);

    Atlas::Objects::Operation::RootOperation op;
    OpVector res;
    op->setTo("3");
    m_lobby->operation(op, res);
}

void Lobbytest::test_operation_connected_other()
{
    Connection conn(*(CommSocket*) 0,
                    *(ServerRouting*) 0,
                    "foo", 3);
    TestAccount tac;

    tac.setConnection(&conn);

    m_lobby->addAccount(&tac);

    Atlas::Objects::Operation::RootOperation op;
    OpVector res;
    op->setTo("2");
    m_lobby->operation(op, res);
}

void Lobbytest::test_operation_account()
{
    TestAccount tac;

    m_lobby->addAccount(&tac);

    Atlas::Objects::Operation::RootOperation op;
    OpVector res;
    op->setTo("2");
    m_lobby->operation(op, res);
}

void Lobbytest::test_addToMessage()
{
    TestAccount testAccount;
    m_lobby->addAccount(&testAccount);

    Atlas::Message::MapType e;
    m_lobby->addToMessage(e);
}

void Lobbytest::test_addToEntity()
{
    TestAccount testAccount;
    m_lobby->addAccount(&testAccount);

    Atlas::Objects::Entity::RootEntity e;
    m_lobby->addToEntity(e);
}


int main()
{
    Lobbytest t;

    return t.run();
}

// Stub functions

#include "common/log.h"
#include "../stubs/server/stubAccount.h"
#include "../stubs/server/stubConnectableRouter.h"
#include "../stubs/server/stubConnection.h"
#include "../stubs/common/stubLink.h"
#include "../stubs/common/stubRouter.h"
#include "rules/simulation/ExternalMind.h"
#include "../stubs/common/stublog.h"
