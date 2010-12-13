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

#include "server/CommPeer.h"

#include "server/CommServer.h"
#include "server/ServerRouting.h"
#include "server/Peer.h"

#include "common/log.h"

#include <Atlas/Codec.h>
#include <Atlas/Negotiate.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cstdlib>

#include <cassert>

#define INT_OPTION(_var, _val, _section, _setting, _help) \
int _var = _val; \
int_config_register _var ## _register(_var, _section, _setting, _help);

const time_t reference_seconds = 1000000000; // Some time in the early 21st

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
        return 0;
    }

    virtual void poll(bool can_get = true)
    {
    }
};

class TestCommPeer : public CommPeer
{
  public:
    TestCommPeer(CommServer & svr) : CommPeer(svr)
    {
    }

    void test_setNegotiateState(Atlas::Negotiate::State state)
    {
        if (m_negotiate != 0) {
            delete m_negotiate;
        }
        m_negotiate = new TestNegotiate(state);
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

    void test_setConnectTime(time_t t)
    {
        m_connectTime = t;
    }
};

class TestRouter : public Peer
{
  public:
    TestRouter(CommClient & c, ServerRouting & s) : Peer(c, s, "test_addr", "5", 5)
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
        TestCommPeer * cs = new TestCommPeer(comm_server);

        delete cs;
    }

    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        cs->idle(reference_seconds);

        delete cs;
    }

    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        cs->test_setNegotiateState(Atlas::Negotiate::SUCCEEDED);

        cs->idle(reference_seconds);

        delete cs;
    }

    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        cs->test_setNegotiateState(Atlas::Negotiate::SUCCEEDED);

        cs->idle(reference_seconds + 20);

        delete cs;
    }

    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        cs->setup(new TestRouter(*cs, server));

        cs->idle(reference_seconds);

        delete cs;
    }

    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        TestRouter * tr = new TestRouter(*cs, server);
        cs->setup(tr);
        tr->setAuthState(PEER_AUTHENTICATED);

        cs->idle(reference_seconds);

        delete cs;
    }

    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        TestRouter * tr = new TestRouter(*cs, server);
        cs->setup(tr);
        tr->setAuthState(PEER_FAILED);

        cs->idle(reference_seconds);

        delete cs;
    }

    return 0;
}

// Stub functions

#include <Atlas/Net/Stream.h>

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
    m_timeVal.tv_sec = 0;
    m_timeVal.tv_usec = 0;
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

CommClient::CommClient(CommServer &svr) : CommStreamClient(svr), 
                                        Idle(svr), m_codec(NULL), 
                                        m_encoder(NULL), m_connection(NULL),
                                        m_connectTime(svr.time())
{
    m_negotiate = new Atlas::Net::StreamConnect("cyphesis_test", m_clientIos);
}

CommClient::~CommClient()
{
}

void CommClient::setup(Router * connection)
{
    m_connection = connection;
}

void CommClient::objectArrived(const Atlas::Objects::Root & obj)
{
}

void CommClient::idle(time_t t)
{
}

int CommClient::operation(const Atlas::Objects::Operation::RootOperation &op)
{
    return 0;
}

int CommClient::read()
{
    return 0;
}

void CommClient::dispatch()
{
}

int CommClient::negotiate()
{
    delete m_negotiate;
    m_negotiate = NULL;
    return 0;
}

int CommClient::send(const Atlas::Objects::Operation::RootOperation &op)
{
    return 0;
}

Peer::Peer(CommClient & client,
           ServerRouting & svr,
           const std::string & addr,
           const std::string & id, long iid) :
      Router(id, iid),
      m_state(PEER_INIT),
      m_commClient(client),
      m_server(svr)
{
}

Peer::~Peer()
{
}

PeerAuthState Peer::getAuthState()
{
    return m_state;
}

void Peer::setAuthState(PeerAuthState state)
{
    m_state = state;
}

void Peer::operation(const Operation &op, OpVector &res)
{
}

void Peer::cleanTeleports()
{
}

int opSerialCount = 0;

const char * CYPHESIS = "cyphesis";

class int_config_register {
  public:
    int_config_register(int &, const char *, const char *, const char *);
};

int_config_register::int_config_register(int & var,
                                         const char * section,
                                         const char * setting,
                                         const char * help)
{
}


// Library stubs
