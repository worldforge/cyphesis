// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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

#include "Sink.h"
#include "TestBase.h"

#include "common/CommSocket.h"
#include "common/Link.h"

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

class TestLink : public Link
{
  public:
    TestLink(CommSocket & socket, const std::string & id, long iid);
    virtual ~TestLink();

    virtual void externalOperation(const Operation & op, Link &);
    virtual void operation(const Operation&, OpVector&);
};

TestLink::TestLink(CommSocket & socket, const std::string & id, long iid) :
      Link(socket, id, iid)
{
}

TestLink::~TestLink()
{
}

void TestLink::externalOperation(const Operation & op, Link &)
{
}

void TestLink::operation(const Operation&, OpVector&)
{
}


class TestCommSocket : public CommSocket
{
  public:
    explicit TestCommSocket(CommServer & svr);

    virtual ~TestCommSocket();

    virtual int getFd() const;
    virtual bool isOpen() const;
    virtual bool eof();
    virtual int read();
    virtual void dispatch();
    virtual void disconnect();
    virtual int flush();

};

TestCommSocket::TestCommSocket(CommServer & svr) : CommSocket(svr)
{
}

TestCommSocket::~TestCommSocket()
{
}

int TestCommSocket::getFd() const
{
    return 0;
}

bool TestCommSocket::isOpen() const
{
    return false;
}

bool TestCommSocket::eof()
{
    return true;
}

int TestCommSocket::read()
{
    return 0;
}

void TestCommSocket::dispatch()
{
}

class Linktest : public Cyphesis::TestBase
{
  private:
    CommSocket * m_socket;
    Link * m_link;
    Atlas::Bridge * m_bridge;
    Atlas::Objects::ObjectsEncoder * m_encoder;

    static bool CommSocket_flush_called;
    static bool CommSocket_disconnect_called;
  public:
    Linktest();

    void setup();
    void teardown();

    void test_send();
    void test_send_connected();
    void test_sendError();
    void test_sendError_connected();
    void test_disconnect();

    static void set_CommSocket_flush_called();
    static void set_CommSocket_disconnect_called();
};

void TestCommSocket::disconnect()
{
    Linktest::set_CommSocket_disconnect_called();
}

int TestCommSocket::flush()
{
    Linktest::set_CommSocket_flush_called();
    return 0;
}

bool Linktest::CommSocket_flush_called = false;
bool Linktest::CommSocket_disconnect_called = false;

void Linktest::set_CommSocket_flush_called()
{
    CommSocket_flush_called = true;
}

void Linktest::set_CommSocket_disconnect_called()
{
    CommSocket_disconnect_called = true;
}

Linktest::Linktest()
{
    ADD_TEST(Linktest::test_send);
    ADD_TEST(Linktest::test_send_connected);
    ADD_TEST(Linktest::test_sendError);
    ADD_TEST(Linktest::test_sendError_connected);
    ADD_TEST(Linktest::test_disconnect);
}

void Linktest::setup()
{
    m_bridge = new Sink;
    m_socket = new TestCommSocket(*(CommServer*)0);
    m_link = new TestLink(*m_socket, "1", 1);
    m_encoder = 0;
}

void Linktest::teardown()
{
    delete m_link;
    delete m_socket;
    delete m_bridge;
    delete m_encoder;
}

void Linktest::test_send()
{
    CommSocket_flush_called = false;

    Operation op;

    m_link->send(op);

    ASSERT_TRUE(!CommSocket_flush_called);
}

void Linktest::test_send_connected()
{
    CommSocket_flush_called = false;

    m_encoder = new Atlas::Objects::ObjectsEncoder(*m_bridge);
    m_link->setEncoder(m_encoder);

    Operation op;

    m_link->send(op);

    ASSERT_TRUE(CommSocket_flush_called);
}

void Linktest::test_sendError()
{
    CommSocket_flush_called = false;

    Operation op;

    m_link->sendError(op, "test error message", "");

    ASSERT_TRUE(!CommSocket_flush_called);
}

void Linktest::test_sendError_connected()
{
    CommSocket_flush_called = false;

    m_encoder = new Atlas::Objects::ObjectsEncoder(*m_bridge);
    m_link->setEncoder(m_encoder);

    Operation op;

    m_link->sendError(op, "test error message", "");

    ASSERT_TRUE(CommSocket_flush_called);
}

void Linktest::test_disconnect()
{
    CommSocket_disconnect_called = false;

    m_link->disconnect();

    ASSERT_TRUE(CommSocket_disconnect_called);
}

int main()
{
    Linktest t;

    return t.run();
}

// stubs

#include "common/Router.h"

Router::Router(const std::string & id, long intId) : m_id(id), m_intId(intId)
{
}

Router::~Router()
{
}

void Router::buildError(const Operation & op,
                        const std::string & errstring,
                        const Operation & e,
                        const std::string & to) const
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
}
