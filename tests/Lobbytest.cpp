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

#include "server/Lobby.h"

#include "server/CommClient.h"
#include "server/Connection.h"
#include "server/Account.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

class TestAccount : public Account
{
  public:
    TestAccount() : Account(0, "bob", "foo", "2", 2) { }

    virtual int characterError(const Operation &,
                               const Atlas::Objects::Entity::RootEntity & ent,
                               OpVector & res) const
    {
        return false;
    }
};

int main()
{
    {
        Lobby * l = new Lobby(*(ServerRouting*)0, "1", 1);
        delete l;
    }

    {
        Lobby * l = new Lobby(*(ServerRouting*)0, "1", 1);

        assert(l->getAccounts().size() == 0);

        l->addAccount(new TestAccount());

        assert(l->getAccounts().size() == 1);

        delete l;
    }

    {
        Lobby * l = new Lobby(*(ServerRouting*)0, "1", 1);

        assert(l->getAccounts().size() == 0);

        Account * tac = new TestAccount();

        l->addAccount(tac);

        assert(l->getAccounts().size() == 1);

        l->delAccount(tac);

        assert(l->getAccounts().size() == 0);

        delete l;
    }

    {
        Lobby * l = new Lobby(*(ServerRouting*)0, "1", 1);

        assert(l->getAccounts().size() == 0);

        l->delAccount(new TestAccount());

        assert(l->getAccounts().size() == 0);

        delete l;
    }

    {
        Lobby * l = new Lobby(*(ServerRouting*)0, "1", 1);

        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        l->operation(op, res);

        delete l;
    }

    {
        Lobby * l = new Lobby(*(ServerRouting*)0, "1", 1);

        Account * tac = new TestAccount();

        tac->m_connection = new Connection(*(CommClient*)0,
                                           *(ServerRouting*)0,
                                           "foo", "3");

        l->addAccount(tac);

        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        l->operation(op, res);

        delete l;
    }

    {
        Lobby * l = new Lobby(*(ServerRouting*)0, "1", 1);

        Account * tac = new TestAccount();

        tac->m_connection = new Connection(*(CommClient*)0,
                                           *(ServerRouting*)0,
                                           "foo", "3");

        l->addAccount(tac);

        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        op->setTo("3");
        l->operation(op, res);

        delete l;
    }

    {
        Lobby * l = new Lobby(*(ServerRouting*)0, "1", 1);

        Account * tac = new TestAccount();

        tac->m_connection = new Connection(*(CommClient*)0,
                                           *(ServerRouting*)0,
                                           "foo", "3");

        l->addAccount(tac);

        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        op->setTo("2");
        l->operation(op, res);

        delete l;
    }

    {
        Lobby * l = new Lobby(*(ServerRouting*)0, "1", 1);

        Account * tac = new TestAccount();

        l->addAccount(tac);

        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        op->setTo("2");
        l->operation(op, res);

        delete l;
    }

    {
        Lobby * l = new Lobby(*(ServerRouting*)0, "1", 1);

        l->addAccount(new TestAccount());

        Atlas::Message::MapType e;
        l->addToMessage(e);

        delete l;
    }

    {
        Lobby * l = new Lobby(*(ServerRouting*)0, "1", 1);

        l->addAccount(new TestAccount());

        Atlas::Objects::Entity::RootEntity e;
        l->addToEntity(e);

        delete l;
    }


    return 0;
}

// Stub functions

int CommClient::send(const Atlas::Objects::Operation::RootOperation & op)
{
    return 0;
}

Account::Account(Connection * conn,
                 const std::string & uname,
                 const std::string & passwd,
                 const std::string & id,
                 long intId) :
         Router(id, intId),
         m_username(uname), m_password(passwd), m_connection(conn)
{
}

Account::~Account()
{
}

const char * Account::getType() const
{
    return "testaccount";
}

void Account::store() const
{
}

void Account::addToMessage(Atlas::Message::MapType &) const
{
}

void Account::addToEntity(const Atlas::Objects::Entity::RootEntity &) const
{
}


void Account::operation(const Operation &, OpVector &)
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

Connection::Connection(CommClient & client,
                       ServerRouting & svr,
                       const std::string & addr,
                       const std::string & id) :
            Router(id, 3), m_obsolete(false),
                                                m_commClient(client),
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
