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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"

#include "server/TCPListenFactory.h"

#include "server/CommServer.h"

#include "common/compose.hpp"
#include "common/debug.h"

#include <cassert>

using String::compose;

class TCPListenFactorytest : public Cyphesis::TestBase
{
  protected:
    CommServer * m_commServer;
  public:
    TCPListenFactorytest();

    void setup();
    void teardown();

    void test_null();
};

TCPListenFactorytest::TCPListenFactorytest() : m_commServer(0L)
{
    ADD_TEST(TCPListenFactorytest::test_null);
}

void TCPListenFactorytest::setup()
{
    m_commServer = new CommServer;
}

void TCPListenFactorytest::teardown()
{
}

void TCPListenFactorytest::test_null()
{
    ASSERT_NOT_NULL(m_commServer);
}

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

int CommServer::addSocket(CommSocket * cs)
{
    return 0;
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
    if (m_listener.open(i) != 0) {
        return -1;
    }
    return 0;
}
