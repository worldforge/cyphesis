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

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::RootOperation;

Atlas::Objects::Factories factories;

class ClientConnectiontest : public Cyphesis::TestBase
{
    boost::asio::io_context io_context;
    ClientConnection * cc{};
  public:
    ClientConnectiontest();

    void setup() override;
    void teardown() override;

    void test_sequence();
};

ClientConnectiontest::ClientConnectiontest()
{
    ADD_TEST(ClientConnectiontest::test_sequence);
}

void ClientConnectiontest::setup()
{
    cc = new ClientConnection(io_context, factories);
}

void ClientConnectiontest::teardown()
{
    delete cc;
}

void ClientConnectiontest::test_sequence()
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

        Anonymous op_arg;
        op->setArgs1(op_arg);

        op->setFrom("1");
        op->setParent("");
        cc->operation(op);

        Info i;
        cc->operation(i);

        Error e;
        cc->operation(e);
    }
}

int main()
{
    ClientConnectiontest t;

    return t.run();
}

// stubs
#define STUB_AtlasStreamClient_poll
int AtlasStreamClient::poll(const std::chrono::steady_clock::duration& duration)
{
    return -1;
}
#include "../stubs/common/stubAtlasStreamClient.h"
