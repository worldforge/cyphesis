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

#include "server/CommClient.h"

#include "server/CommServer.h"
#include "server/ServerRouting.h"

#include "common/log.h"

#include <Atlas/Codec.h>
#include <Atlas/Negotiate.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootOperation.h>

#include <cstdlib>

#include <cassert>

class TestCodec : public Atlas::Codec
{
  public:
    virtual void poll(bool can_get = true)
    {
    }

    virtual void streamBegin()
    {
    }

    virtual void streamMessage()
    {
    }

    virtual void streamEnd()
    {
    }

    virtual void mapMapItem(const std::string& name)
    {
    }

    virtual void mapListItem(const std::string& name)
    {
    }

    virtual void mapIntItem(const std::string& name, long)
    {
    }

    virtual void mapFloatItem(const std::string& name, double)
    {
    }

    virtual void mapStringItem(const std::string& name, const std::string&)
    {
    }

    virtual void mapEnd()
    {
    }

    virtual void listMapItem()
    {
    }

    virtual void listListItem()
    {
    }

    virtual void listIntItem(long)
    {
    }

    virtual void listFloatItem(double)
    {
    }

    virtual void listStringItem(const std::string&)
    {
    }

    virtual void listEnd()
    {
    }
};

class TestNegotiate : public Atlas::Negotiate
{
  public:
    Atlas::Negotiate::State m_state;

    TestNegotiate(Atlas::Negotiate::State state) : m_state(state)
    {
    }

    virtual Atlas::Negotiate::State getState()
    {
        return m_state;
    }

    virtual Atlas::Codec * getCodec(Atlas::Bridge &)
    {
        return new TestCodec;
    }

    virtual void poll(bool can_get = true)
    {
    }
};

class TestCommClient : public CommClient
{
  public:
    TestCommClient(CommServer & svr, int fd) : CommClient(svr, fd)
    {
    }

    TestCommClient(CommServer & svr) : CommClient(svr)
    {
    }

    void test_setEncoder()
    {
        m_codec = new TestCodec;
        m_encoder = new Atlas::Objects::ObjectsEncoder(*m_codec);
    }

    void test_negotiate()
    {
        negotiate();
    }

    void test_setNegotiateState(Atlas::Negotiate::State state)
    {
        if (m_negotiate != 0) {
            delete m_negotiate;
        }
        m_negotiate = new TestNegotiate(state);
    }

    void test_setConnection(Router * r)
    {
        m_connection = r;
    }

    void test_operation(const Atlas::Objects::Operation::RootOperation&op)
    {
        operation(op);
    }

    void test_openSocket()
    {
        int fds[2];
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, &fds[0]) != 0) {
            exit(1);
        }

        m_clientIos.setSocket(fds[0]);

        pid_t p = fork();
        if (p == -1) {
            exit(1);
        }
        if (p == 0) {
            char buf[1];
            while(::read(fds[1], &buf[0], 1) == 1);
            exit(0);
        }
    }

    void test_idle(int i)
    {
        idle(i);
    }

    void test_setConnectTime(time_t t)
    {
        m_connectTime = t;
    }
};

class TestRouter : public Router
{
  public:
    TestRouter() : Router("5", 5)
    {
    }

    virtual void operation(const Operation&, OpVector&)
    {
    }
};

int main()
{
    ServerRouting server(*(BaseWorld*)0, "mason", "test_server",
                         "1", 1,
                         "2", 2);

    CommServer comm_server(server);

    {
        new TestCommClient(comm_server);
    }

    {
        new TestCommClient(comm_server, 0);
    }

    {
        CommClient * cs = new TestCommClient(comm_server);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setEncoder();

        delete cs;
    }

    {
        CommClient * cs = new TestCommClient(comm_server);

        cs->setup(0);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_negotiate();

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setNegotiateState(Atlas::Negotiate::SUCCEEDED);

        cs->test_negotiate();

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setNegotiateState(Atlas::Negotiate::FAILED);

        cs->test_negotiate();

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setConnectTime(0);
        cs->test_idle(0);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setNegotiateState(Atlas::Negotiate::SUCCEEDED);
        cs->test_negotiate();

        cs->test_setConnectTime(0);
        cs->test_idle(0);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setConnectTime(0);
        cs->test_idle(20);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->read();

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setEncoder();
        cs->read();

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        Atlas::Objects::Operation::RootOperation op;
        cs->send(op);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_openSocket();

        Atlas::Objects::Operation::RootOperation op;
        cs->send(op);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_openSocket();
        cs->test_setEncoder();

        Atlas::Objects::Operation::RootOperation op;
        cs->send(op);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setConnection(new TestRouter);

        Atlas::Objects::Operation::RootOperation op;
        cs->test_operation(op);

        delete cs;
    }

    return 0;
}

// Stub functions

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

CommStreamClient::CommStreamClient(CommServer & svr, int fd) :
                  CommSocket(svr),
                  m_clientIos(fd)
{
}

CommStreamClient::CommStreamClient(CommServer & svr) :
                  CommSocket(svr)
{
}

CommStreamClient::~CommStreamClient()
{
}

int CommStreamClient::getFd() const
{
    return m_clientIos.getSocket();
}

bool CommStreamClient::isOpen() const
{
    return m_clientIos.is_open();
}

bool CommStreamClient::eof()
{
    return (m_clientIos.fail() ||
            m_clientIos.peek() == std::iostream::traits_type::eof());
}

Idle::Idle(CommServer & svr) : m_idleManager(svr)
{
}

Idle::~Idle()
{
}

void log(LogLevel, const std::string & msg)
{
}

ServerRouting::ServerRouting(BaseWorld & wrld,
                             const std::string & ruleset,
                             const std::string & name,
                             const std::string & id, long intId,
                             const std::string & lId, long lIntId) :
        Router(id, intId),
        m_svrRuleset(ruleset), m_svrName(name),
        m_numClients(0), m_world(wrld), m_lobby(*(Lobby*)0)
{
}

ServerRouting::~ServerRouting()
{
}

void ServerRouting::addToMessage(Atlas::Message::MapType & omap) const
{
}

void ServerRouting::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

CommServer::CommServer(ServerRouting & svr) : m_congested(false), m_server(svr)
{
}

CommServer::~CommServer()
{
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

// Library stubs
