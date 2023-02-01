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

#include "client/cyclient/ClientConnection.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::RootOperation;

Atlas::Objects::Factories factories;

class ClientConnectionintegration : public Cyphesis::TestBase
{
    ClientConnection * cc;
    boost::asio::io_context m_io_context;
  public:
    ClientConnectionintegration();

    void setup();
    void teardown();

    void test_sequence();
};

ClientConnectionintegration::ClientConnectionintegration()
{
    ADD_TEST(ClientConnectionintegration::test_sequence);
}

void ClientConnectionintegration::setup()
{
    cc = new ClientConnection(m_io_context, factories);
}

void ClientConnectionintegration::teardown()
{
    delete cc;
}

void ClientConnectionintegration::test_sequence()
{
    // Try all the method calls when not connected

    cc->login("username", "password");
    cc->create("player", "username", "password");
    cc->wait();

    {
        RootOperation op;
        cc->send(op);
    }

    cc->pop();
    cc->pending();

    {
        Root obj;
        RootOperation op;
        cc->operation(op);
        cc->errorArrived(op);
        cc->infoArrived(op);
        cc->objectArrived(op);

        Anonymous op_arg;
        op->setArgs1(op_arg);
        cc->infoArrived(op);

        op->setFrom("1");
        cc->infoArrived(op);

        op->setParent("");
        cc->operation(op);
        cc->objectArrived(op);

        Info i;
        cc->objectArrived(i);

        Error e;
        cc->objectArrived(e);

        cc->objectArrived(obj);
        obj->setParent("");
        cc->objectArrived(obj);
    }
}

int main()
{
    ClientConnectionintegration t;

    return t.run();
}

// stubs

#include "common/debug.h"
#include "../stubs/common/stublog.h"
