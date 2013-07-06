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

#include "TestBase.h"

#include "server/TCPListenFactory.h"

#include "server/CommClientKit.h"
#include "server/CommServer.h"

#include "common/compose.hpp"
#include "common/debug.h"

#include <boost/make_shared.hpp>

#include <cassert>

using String::compose;

class TestCommClientKit : public CommClientKit
{
  public:
    ~TestCommClientKit();

    int newCommClient(CommServer &, int, const std::string &);
};

TestCommClientKit::~TestCommClientKit()
{
}

int TestCommClientKit::newCommClient(CommServer &, int, const std::string &)
{
    return 0;
}

class TCPListenFactorytest : public Cyphesis::TestBase
{
  protected:
    CommServer * m_commServer;
    boost::shared_ptr<CommClientKit> m_commClientKit;

    static bool CommServer_addSocket_called;
    static int CommTCPListener_setup_return;
  public:
    TCPListenFactorytest();

    void setup();
    void teardown();

    void test_listen();
    void test_listen_fail();

    static void set_CommServer_addSocket_called();
    static int get_CommTCPListener_setup_return();
};

TCPListenFactorytest::TCPListenFactorytest() : m_commServer(0L)
{
    ADD_TEST(TCPListenFactorytest::test_listen);
    ADD_TEST(TCPListenFactorytest::test_listen_fail);
}

void TCPListenFactorytest::setup()
{
    m_commServer = new CommServer;
    m_commClientKit = boost::make_shared<TestCommClientKit>();
}

void TCPListenFactorytest::teardown()
{
    m_commClientKit.reset();
    delete m_commServer;
}

void TCPListenFactorytest::test_listen()
{
    CommServer_addSocket_called = false;
    CommTCPListener_setup_return = 0;

    int ret = TCPListenFactory::listen(*m_commServer, 6767, m_commClientKit);

    ASSERT_EQUAL(ret, 0);
    ASSERT_TRUE(CommServer_addSocket_called);
}

void TCPListenFactorytest::test_listen_fail()
{
    CommServer_addSocket_called = false;
    CommTCPListener_setup_return = -1;

    int ret = TCPListenFactory::listen(*m_commServer, 6767, m_commClientKit);

    ASSERT_EQUAL(ret, -1);
    ASSERT_TRUE(!CommServer_addSocket_called);
}

void TCPListenFactorytest::set_CommServer_addSocket_called()
{
    CommServer_addSocket_called = true;
}

int TCPListenFactorytest::get_CommTCPListener_setup_return()
{
    return CommTCPListener_setup_return;
}

bool TCPListenFactorytest::CommServer_addSocket_called = true;
int TCPListenFactorytest::CommTCPListener_setup_return = 0;

int main()
{
    TCPListenFactorytest t;

    return t.run();
}

// stubs

#include "CommStreamListener_stub_impl.h"

#include "server/CommServer.h"
#include "server/CommTCPListener.h"

CommServer::CommServer() : m_epollFd(-1),
                           m_congested(false),
                           m_tick(0)
{
}

CommServer::~CommServer()
{
}

int CommServer::addSocket(CommSocket * cs)
{
    TCPListenFactorytest::set_CommServer_addSocket_called();
    return 0;
}

CommClientKit::~CommClientKit()
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

CommTCPListener::CommTCPListener(CommServer & svr,
                                 const boost::shared_ptr<CommClientKit> & kit) :
                 CommStreamListener<tcp_socket_server>(svr, kit)
{
}

CommTCPListener::~CommTCPListener()
{
}

int CommTCPListener::accept()
{
    return -1;
}

int CommTCPListener::setup(struct addrinfo * i)
{
    return TCPListenFactorytest::get_CommTCPListener_setup_return();
}
