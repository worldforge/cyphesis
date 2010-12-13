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

#include "server/Juncture.h"

#include "server/CommPeer.h"
#include "server/CommServer.h"
#include "common/Connect.h"
#include "server/Connection.h"
#include "server/Peer.h"
#include "server/ServerRouting.h"

#include <Atlas/Objects/Operation.h>

#include <cassert>

class BaseWorld;

// Test class to expose protected methods.
class TestJuncture : public Juncture
{
  public:
    TestJuncture(Connection * c = 0) : Juncture(c, "1", 1) { }

    void test_onPeerLost() { onPeerLost(); }
    void test_onPeerReplied(const Operation & op) { onPeerReplied(op); }

    void test_addPeer(Peer * p) { m_peer = p; }
    void test_addSocket(CommPeer * cp) { m_socket = cp; }
};

int stub_CommPeer_connect_return = 0;

int main()
{
    {
        Juncture * j = new Juncture(0, "1", 1);

        delete j;
    }

    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Operation op;
        j->operation(op, res);

        delete j;
    }

    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Atlas::Objects::Operation::Login op;
        j->operation(op, res);

        delete j;
    }

    // Login op, no args
    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Operation op;
        j->LoginOperation(op, res);

        delete j;
    }

    // Login op, empty arg
    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        
        j->LoginOperation(op, res);

        delete j;
    }

    // Login op, username in arg
    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        op->setArgs1(arg);
        
        j->LoginOperation(op, res);

        delete j;
    }

    // Login op, bad username in arg
    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", 0x69e362c6);
        op->setArgs1(arg);
        
        j->LoginOperation(op, res);

        delete j;
    }

    // Login op, username & password in arg
    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        arg->setAttr("password", "a12a2f3a-03a4-11e0-8379-001ec93e7c08");
        op->setArgs1(arg);
        
        j->LoginOperation(op, res);

        delete j;
    }

    // Login op, username & bad password in arg
    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        arg->setAttr("password", 0x12a2f3aL);
        op->setArgs1(arg);
        
        j->LoginOperation(op, res);

        delete j;
    }

    // Login op, username & password in arg, connected
    {
        ServerRouting sr(*(BaseWorld*)0, "", "", "2", 2, "3", 3);
        CommServer cs(sr);

        TestJuncture * j = new TestJuncture(0);

        CommPeer * cp = new CommPeer(cs);
        j->test_addPeer(new Peer(*cp, sr, "", "4", 4));
        j->test_addSocket(cp);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        arg->setAttr("password", "a12a2f3a-03a4-11e0-8379-001ec93e7c08");
        op->setArgs1(arg);
        
        j->LoginOperation(op, res);

        delete j;
    }

    // Login op, username & password in arg, connected already authenticating
    {
        ServerRouting sr(*(BaseWorld*)0, "", "", "2", 2, "3", 3);
        CommServer cs(sr);

        TestJuncture * j = new TestJuncture(0);

        CommPeer * cp = new CommPeer(cs);
        Peer * p = new Peer(*cp, sr, "", "4", 4);
        j->test_addPeer(p);
        j->test_addSocket(cp);

        p->setAuthState(PEER_AUTHENTICATING);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        arg->setAttr("password", "a12a2f3a-03a4-11e0-8379-001ec93e7c08");
        op->setArgs1(arg);
        
        j->LoginOperation(op, res);

        delete j;
    }

    // Login op, username & password in arg, connected, with serialno
    {
        ServerRouting sr(*(BaseWorld*)0, "", "", "2", 2, "3", 3);
        CommServer cs(sr);

        TestJuncture * j = new TestJuncture(0);

        CommPeer * cp = new CommPeer(cs);
        j->test_addPeer(new Peer(*cp, sr, "", "4", 4));
        j->test_addSocket(cp);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        arg->setAttr("password", "a12a2f3a-03a4-11e0-8379-001ec93e7c08");
        op->setArgs1(arg);
        op->setSerialno(0x6dc5b5eaL);
        
        j->LoginOperation(op, res);

        delete j;
    }

    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Operation op;
        j->OtherOperation(op, res);

        delete j;
    }

    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Atlas::Objects::Operation::Connect op;
        j->OtherOperation(op, res);

        delete j;
    }

    // Connect op, no args
    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Operation op;
        j->customConnectOperation(op, res);

        delete j;
    }

    // Connect op, no args, already connected
    {
        TestJuncture * j = new TestJuncture(0);

        j->test_addPeer(new Peer(*(CommPeer*)0, *(ServerRouting*)0, "", "4", 4));

        OpVector res;
        Operation op;
        j->customConnectOperation(op, res);

        delete j;
    }

    // Connect op, hostname in arg
    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("hostname", "3752ca4a-03a9-11e0-bd8a-001ec93e7c08");

        op->setArgs1(arg);

        j->customConnectOperation(op, res);

        delete j;
    }

    // Connect op, bad hostname in arg
    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("hostname", 0x3752ca4aL);

        op->setArgs1(arg);

        j->customConnectOperation(op, res);

        delete j;
    }

    // Connect op, hostname and port in arg, connected this end
    {
        ServerRouting sr(*(BaseWorld*)0, "", "", "2", 2, "3", 3);
        CommServer cs(sr);
        CommClient * cc = new CommClient(cs, 23);
        Connection * c = new Connection(*cc, sr, "", "4", 4);

        Juncture * j = new Juncture(c, "1", 1);

        OpVector res;
        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("hostname", "3752ca4a-03a9-11e0-bd8a-001ec93e7c08");
        arg->setAttr("port", 0x03a9);

        op->setArgs1(arg);

        j->customConnectOperation(op, res);

        delete j;
    }

    // Connect op, hostname and port in arg, connected this end, connect fails
    {
        stub_CommPeer_connect_return = -1;

        ServerRouting sr(*(BaseWorld*)0, "", "", "2", 2, "3", 3);
        CommServer cs(sr);
        CommClient * cc = new CommClient(cs, 23);
        Connection * c = new Connection(*cc, sr, "", "4", 4);

        Juncture * j = new Juncture(c, "1", 1);

        OpVector res;
        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("hostname", "3752ca4a-03a9-11e0-bd8a-001ec93e7c08");
        arg->setAttr("port", 0x03a9);

        op->setArgs1(arg);

        j->customConnectOperation(op, res);

        stub_CommPeer_connect_return = 0;

        delete j;
    }

    // Teleport unconnected
    {
        TestJuncture * j = new TestJuncture(0);

        j->teleportEntity(0);

        delete j;
    }

    // Teleport connected
    {
        TestJuncture * j = new TestJuncture(0);

        j->test_addPeer(new Peer(*(CommPeer*)0, *(ServerRouting*)0, "", "4", 4));
        j->teleportEntity(0);

        delete j;
    }

    {
        TestJuncture * j = new TestJuncture(0);

        j->test_onPeerLost();

        delete j;
    }

    // Peer replied, unconnected this end
    {
        TestJuncture * j = new TestJuncture(0);

        Operation op;
        j->test_onPeerReplied(op);

        delete j;
    }

    // Peer replied, connected this end
    {
        Connection * c = new Connection(*(CommClient*)0,
                                        *(ServerRouting*)0, "", "4", 4);

        TestJuncture * j = new TestJuncture(c);

        Operation op;
        j->test_onPeerReplied(op);

        delete j;
    }

    return 0;
}

// stubs

#include "common/log.h"

#include <Atlas/Negotiate.h>

namespace Atlas { namespace Objects { namespace Operation {

int CONNECT_NO = 500;

} } }

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


CommPeer::CommPeer(CommServer & svr) : CommClient(svr)
{
}

CommPeer::~CommPeer()
{
}

void CommPeer::idle(time_t t)
{
}

bool CommPeer::eof()
{
    return false;
}

int CommPeer::read()
{
    return 0;
}

int CommPeer::connect(const std::string & host, int port, long)
{
    return stub_CommPeer_connect_return;
}

void CommPeer::setup(Router * connection)
{
}

CommClient::CommClient(CommServer &svr, int) : CommStreamClient(svr), 
                                        Idle(svr), m_codec(NULL), 
                                        m_encoder(NULL), m_connection(NULL),
                                        m_connectTime(0)
{
    m_negotiate = NULL;
}

CommClient::CommClient(CommServer &svr) : CommStreamClient(svr), 
                                        Idle(svr), m_codec(NULL), 
                                        m_encoder(NULL), m_connection(NULL),
                                        m_connectTime(0)
{
    m_negotiate = NULL;
}

CommClient::~CommClient()
{
}

void CommClient::setup(Router * connection)
{
}

void CommClient::objectArrived(const Atlas::Objects::Root & obj)
{
}

void CommClient::idle(time_t t)
{
}

int CommClient::operation(const Operation &op)
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

int CommClient::send(const Operation &op)
{
    return 0;
}

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

CommServer::CommServer(ServerRouting & svr) : m_epollFd(-1),
                                              m_congested(false),
                                              m_server(svr)
{
}

CommServer::~CommServer()
{
}

int CommServer::addSocket(CommSocket*)
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

int Peer::teleportEntity(const Entity * ent)
{
    return 0;
}

void Peer::cleanTeleports()
{
}

Connection::Connection(CommClient & client,
                       ServerRouting & svr,
                       const std::string & addr,
                       const std::string & id, long iid) :
            Router(id, iid), m_obsolete(false),
                                                m_commClient(client),
                                                m_server(svr)
{
}

Account * Connection::newAccount(const std::string & type,
                                 const std::string & username,
                                 const std::string & passwd,
                                 const std::string & id, long intId)
{
    return 0;
}

int Connection::verifyCredentials(const Account &,
                                  const Atlas::Objects::Root &) const
{
    return 0;
}


Connection::~Connection()
{
}


void Connection::operation(const Operation &, OpVector &)
{
}

void Connection::LoginOperation(const Operation &, OpVector &)
{
}

void Connection::LogoutOperation(const Operation &, OpVector &)
{
}

void Connection::CreateOperation(const Operation &, OpVector &)
{
}

void Connection::GetOperation(const Operation &, OpVector &)
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

void Router::error(const Operation & op,
                   const std::string & errstring,
                   OpVector & res,
                   const std::string & to) const
{
}

void log(LogLevel lvl, const std::string & msg)
{
}
