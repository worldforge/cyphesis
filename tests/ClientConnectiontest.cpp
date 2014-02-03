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

#include "client/ClientConnection.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::RootOperation;

class ClientConnectiontest : public Cyphesis::TestBase
{
    ClientConnection * cc;
  public:
    ClientConnectiontest();

    void setup();
    void teardown();

    void test_sequence();
};

ClientConnectiontest::ClientConnectiontest()
{
    ADD_TEST(ClientConnectiontest::test_sequence);
}

void ClientConnectiontest::setup()
{
    cc = new ClientConnection();
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
        op->setParents(std::list<std::string>());
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

#include "common/AtlasStreamClient.h"

void AtlasStreamClient::output(const Element & item, int depth) const
{
}

void AtlasStreamClient::output(const Root & ent) const
{
}

void AtlasStreamClient::objectArrived(const Root & obj)
{
}

void AtlasStreamClient::operation(const RootOperation & op)
{
}

void AtlasStreamClient::infoArrived(const RootOperation & op)
{
}

void AtlasStreamClient::appearanceArrived(const RootOperation & op)
{
}

void AtlasStreamClient::disappearanceArrived(const RootOperation & op)
{
}

void AtlasStreamClient::sightArrived(const RootOperation & op)
{
}

void AtlasStreamClient::soundArrived(const RootOperation & op)
{
}

void AtlasStreamClient::loginSuccess(const Atlas::Objects::Root & arg)
{
}

/// \brief Called when an Error operation arrives
///
/// @param op Operation to be processed
void AtlasStreamClient::errorArrived(const RootOperation & op)
{
}

AtlasStreamClient::AtlasStreamClient() : reply_flag(false), error_flag(false),
                                         serialNo(512), m_currentTask(0),
                                         m_spacing(2)
{
}

AtlasStreamClient::~AtlasStreamClient()
{
}

void AtlasStreamClient::send(const RootOperation & op)
{
}

int AtlasStreamClient::connect(const std::string & host, int port)
{
    return 0;
}

int AtlasStreamClient::connectLocal(const std::string & filename)
{
    return 0;
}

int AtlasStreamClient::cleanDisconnect()
{
    return 0;
}

int AtlasStreamClient::negotiate()
{
    return 0;
}

int AtlasStreamClient::login(const std::string & username,
                             const std::string & password)
{
    return 0;
}

int AtlasStreamClient::create(const std::string & type,
                              const std::string & username,
                              const std::string & password)
{
    return 0;
}

int AtlasStreamClient::waitForLoginResponse()
{
    return 0;
}

int AtlasStreamClient::poll(int timeOut, int msec)
{
    return -1;
}

int AtlasStreamClient::runTask(ClientTask * task, const std::string & arg)
{
    return 0;
}

int AtlasStreamClient::endTask()
{
    return 0;
}
