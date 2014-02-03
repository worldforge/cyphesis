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

#include "server/CommPeer.h"

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
    TestCommPeer(boost::asio::io_service & svr) : CommPeer("", svr)
    {
    }

    void test_setNegotiateState(Atlas::Negotiate::State state)
    {
        if (m_negotiate != 0) {
            delete m_negotiate;
        }
        m_negotiate = new TestNegotiate(state);
    }

};

class TestPeer : public Peer
{
  public:
    TestPeer(CommSocket & c, ServerRouting & s) : Peer(c, s, "test_addr", 6767, "5", 5)
    {
    }
};

int main()
{
    ServerRouting server(*(BaseWorld*)0, "deeds", "test_server",
                         "1", 1,
                         "2", 2);

    boost::asio::io_service comm_server;
    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        comm_server.poll();
        delete cs;
    }

    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        cs->test_setNegotiateState(Atlas::Negotiate::SUCCEEDED);
        comm_server.poll();

        delete cs;
    }

    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        cs->test_setNegotiateState(Atlas::Negotiate::SUCCEEDED);
        comm_server.poll();

        delete cs;
    }

    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        cs->setup(new TestPeer(*cs, server));
        comm_server.poll();

        delete cs;
    }

    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        TestPeer * tr = new TestPeer(*cs, server);
        cs->setup(tr);
        tr->setAuthState(PEER_AUTHENTICATED);
        comm_server.poll();

        delete cs;
    }

    {
        TestCommPeer * cs = new TestCommPeer(comm_server);

        TestPeer * tr = new TestPeer(*cs, server);
        cs->setup(tr);
        tr->setAuthState(PEER_FAILED);
        comm_server.poll();

        delete cs;
    }

    return 0;
}

// Stub functions

#include <Atlas/Net/Stream.h>

CommSocket::CommSocket(boost::asio::io_service & svr) : m_io_service(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
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

void ServerRouting::externalOperation(const Operation & op, Link &)
{
}

void ServerRouting::operation(const Operation &, OpVector &)
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

Peer::Peer(CommSocket & client,
           ServerRouting & svr,
           const std::string & addr,
           int port,
           const std::string & id, long iid) :
      Link(client, id, iid),
      m_state(PEER_INIT),
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

void Peer::externalOperation(const Operation &op, Link &)
{
}

void Peer::operation(const Operation &op, OpVector &res)
{
}

void Peer::cleanTeleports()
{
}

int opSerialCount = 0;

const char * const CYPHESIS = "cyphesis";

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

#include <common/Shaker.h>

Shaker::Shaker()
{
}

std::string Shaker::generateSalt(size_t length)
{
    return "";
}


// Library stubs
