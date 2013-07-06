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

#include "server/CommClient_impl.h"

#include "server/CommServer.h"
#include "server/CommStreamClient_impl.h"

#include "common/Link.h"
#include "common/log.h"

#include <Atlas/Codec.h>
#include <Atlas/Negotiate.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <skstream/skstream.h>

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

template class CommStreamClient<tcp_socket_stream>;
template class CommClient<tcp_socket_stream>;

class TestCommClient : public CommClient<tcp_socket_stream>
{
  public:
    TestCommClient(CommServer & svr, int fd) :
          CommClient<tcp_socket_stream>(svr, "", fd)
    {
    }

    TestCommClient(CommServer & svr) : CommClient<tcp_socket_stream>(svr, "")
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

    void test_setConnection(Link * r)
    {
        m_link = r;
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

    void test_setupQueue()
    {
        Atlas::Objects::Operation::RootOperation op;
        m_opQueue.push_back(op);
    }

    void test_objectArrived(const Atlas::Objects::Root & obj)
    {
        objectArrived(obj);
    }
};

class TestLink : public Link
{
  protected:
    int m_reply;
  public:
    TestLink(CommSocket & cs, int reply = 0) : Link(cs, "5", 5), m_reply(reply)
    {
    }

    virtual void externalOperation(const Operation & op, Link &)
    {
    }

    virtual void operation(const Operation&, OpVector&res)
    {
        if (m_reply) {
            Atlas::Objects::Operation::RootOperation op;
            res.push_back(op);
        }
    }
};

int main()
{
    CommServer comm_server;

    {
        new TestCommClient(comm_server);
    }

    {
        new TestCommClient(comm_server, 0);
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setEncoder();

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

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

        cs->test_setConnection(new TestLink(*cs));

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

        cs->test_setConnection(new TestLink(*cs));

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

        cs->test_setConnection(new TestLink(*cs));

        Atlas::Objects::Operation::RootOperation op;
        cs->test_operation(op);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setConnection(new TestLink(*cs, 1));

        Atlas::Objects::Operation::RootOperation op;
        cs->test_operation(op);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setConnection(new TestLink(*cs, 1));

        Atlas::Objects::Operation::RootOperation op;
        op->setSerialno(23);
        cs->test_operation(op);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setConnection(new TestLink(*cs, 1));

        cs->dispatch();

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        cs->test_setConnection(new TestLink(*cs, 1));

        cs->test_setupQueue();
        cs->dispatch();

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        Atlas::Objects::Entity::RootEntity ent;
        cs->test_objectArrived(ent);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        Atlas::Objects::Entity::RootEntity ent;
        ent->setParents(std::list<std::string>());
        cs->test_objectArrived(ent);

        delete cs;
    }

    {
        TestCommClient * cs = new TestCommClient(comm_server);

        Atlas::Objects::Operation::RootOperation op;
        cs->test_objectArrived(op);

        delete cs;
    }

    return 0;
}

// Stub functions

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
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

CommServer::CommServer() : m_congested(false)
{
}

CommServer::~CommServer()
{
}

Link::Link(CommSocket & socket, const std::string & id, long iid) :
            Router(id, iid), m_encoder(0), m_commSocket(socket)
{
}

Link::~Link()
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
