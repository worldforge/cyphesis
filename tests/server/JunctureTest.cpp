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

#include "server/Juncture.h"

#include "server/CommPeer.h"
#include "server/Connection.h"
#include "server/Peer.h"
#include "server/ServerRouting.h"
#include "common/CommAsioClient_impl.h"

#include "common/operations/Connect.h"
#include "common/CommSocket.h"

#include <Atlas/Objects/Operation.h>

#include <cassert>


class BaseWorld;

Atlas::Objects::Factories factories;
class StubSocket : public CommSocket
{
  public:
    StubSocket(boost::asio::io_context& io_context) : CommSocket(io_context)
    {
    }

    virtual void disconnect()
    {
    }

    virtual int flush()
    {
        return 0;
    }

};

// Test class to expose protected methods.
class TestJuncture : public Juncture
{
  public:
    TestJuncture(Connection * c = 0) : Juncture(c, 1) { }

    void test_onPeerLost() { onPeerLost(); }
    void test_onPeerReplied(const Operation & op) { onPeerReplied(op); }

    void test_addPeer(Peer * p) { m_peer = p; }
//    void test_addSocket(CommPeer * cp) { m_socket = cp; }
};

int stub_CommPeer_connect_return = 0;

int main()
{
    {
        Juncture j(0, 1);

    }

    {
        Juncture j(0, 1);

        OpVector res;
        Operation op;
        j.operation(op, res);

    }

    {
        Juncture j(0, 1);

        OpVector res;
        Atlas::Objects::Operation::Login op;
        j.operation(op, res);

    }

    // Login op, no args
    {
        Juncture j(0, 1);

        OpVector res;
        Operation op;
        j.LoginOperation(op, res);

    }

    // Login op, empty arg
    {
        Juncture j(0, 1);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        
        j.LoginOperation(op, res);

    }

    // Login op, username in arg
    {
        Juncture j(0, 1);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        op->setArgs1(arg);
        
        j.LoginOperation(op, res);

    }

    // Login op, bad username in arg
    {
        Juncture j(0, 1);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", 0x69e362c6);
        op->setArgs1(arg);
        
        j.LoginOperation(op, res);

    }

    // Login op, username & password in arg
    {
        Juncture j(0, 1);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        arg->setAttr("password", "a12a2f3a-03a4-11e0-8379-001ec93e7c08");
        op->setArgs1(arg);
        
        j.LoginOperation(op, res);

    }

    // Login op, username & bad password in arg
    {
        Juncture j(0, 1);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        arg->setAttr("password", 0x12a2f3aL);
        op->setArgs1(arg);
        
        j.LoginOperation(op, res);

    }

    // Login op, username & password in arg, connected
    {
        ServerRouting sr(*(BaseWorld*)0, *(Persistence*)nullptr, "", "", 3);
        TestJuncture j(0);
        boost::asio::io_context io_context;
        CommPeer cp("", io_context, factories);
        Peer peer(cp, sr, "", 6767, 4);
        j.test_addPeer(&peer);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        arg->setAttr("password", "a12a2f3a-03a4-11e0-8379-001ec93e7c08");
        op->setArgs1(arg);
        
        j.LoginOperation(op, res);

    }

    // Login op, username & password in arg, connected already authenticating
    {
        ServerRouting sr(*(BaseWorld*)0, *(Persistence*)nullptr, "", "", 3);

        TestJuncture j(0);

        boost::asio::io_context io_context;
        CommPeer cp("", io_context, factories);
        Peer p(cp, sr, "", 6767, 4);
        j.test_addPeer(&p);

        p.setAuthState(PEER_AUTHENTICATING);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        arg->setAttr("password", "a12a2f3a-03a4-11e0-8379-001ec93e7c08");
        op->setArgs1(arg);
        
        j.LoginOperation(op, res);

    }

    // Login op, username & password in arg, connected, with serialno
    {
        ServerRouting sr(*(BaseWorld*)0, *(Persistence*)nullptr, "", "", 3);

        TestJuncture j(0);

        boost::asio::io_context io_context;
        CommPeer cp("", io_context, factories);
        Peer p(cp, sr, "", 6767, 4);
        j.test_addPeer(&p);

        OpVector res;

        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("username", "69e362c6-03a4-11e0-9608-001ec93e7c08");
        arg->setAttr("password", "a12a2f3a-03a4-11e0-8379-001ec93e7c08");
        op->setArgs1(arg);
        op->setSerialno(0x6dc5b5eaL);
        
        j.LoginOperation(op, res);

    }

    {
        Juncture j(0, 1);

        OpVector res;
        Operation op;
        j.OtherOperation(op, res);

    }

    {
        Juncture j(0, 1);

        OpVector res;
        Atlas::Objects::Operation::Connect op;
        j.OtherOperation(op, res);

    }

    // Connect op, no args
    {
        Juncture j(0, 1);

        OpVector res;
        Operation op;
        j.customConnectOperation(op, res);

    }

    // Connect op, no args, already connected
    {
        ServerRouting sr(*(BaseWorld*)0, *(Persistence*)nullptr, "", "", 3);
        TestJuncture j(0);

        boost::asio::io_context io_context;
        CommPeer cp("", io_context, factories);
        Peer p(cp, sr, "", 6767, 4);
        j.test_addPeer(&p);

        OpVector res;
        Operation op;
        j.customConnectOperation(op, res);

    }

    // Connect op, hostname in arg
    {
        Juncture j(0, 1);

        OpVector res;
        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("hostname", "3752ca4a-03a9-11e0-bd8a-001ec93e7c08");

        op->setArgs1(arg);

        j.customConnectOperation(op, res);

    }

    // Connect op, bad hostname in arg
    {
        Juncture j(0, 1);

        OpVector res;
        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("hostname", 0x3752ca4aL);

        op->setArgs1(arg);

        j.customConnectOperation(op, res);

    }

    // Connect op, hostname and port in arg, connected this end
    {
        ServerRouting sr(*(BaseWorld*)0, *(Persistence*)nullptr, "", "", 3);
        boost::asio::io_context io_context;
        StubSocket cc(io_context);
        Connection c(cc, sr, "", 4);

        Juncture j(&c, 1);

        OpVector res;
        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("hostname", "3752ca4a-03a9-11e0-bd8a-001ec93e7c08");
        arg->setAttr("port", 0x03a9);

        op->setArgs1(arg);

        j.customConnectOperation(op, res);

    }

    // Connect op, hostname and port in arg, connected this end, connect fails
    {
        stub_CommPeer_connect_return = -1;

        ServerRouting sr(*(BaseWorld*)0, *(Persistence*)nullptr, "", "", 3);
        boost::asio::io_context io_context;
        StubSocket cc(io_context);
        Connection c(cc, sr, "", 4);

        Juncture j(&c, 1);

        OpVector res;
        Operation op;
        Atlas::Objects::Root arg;
        arg->setAttr("hostname", "3752ca4a-03a9-11e0-bd8a-001ec93e7c08");
        arg->setAttr("port", 0x03a9);

        op->setArgs1(arg);

        j.customConnectOperation(op, res);

        stub_CommPeer_connect_return = 0;

    }

    // Teleport unconnected
    {
        TestJuncture j(0);

        j.teleportEntity(0);

    }

    // Teleport connected
    {
        TestJuncture j(0);
        Peer peer(*(CommPeer*)0, *(ServerRouting*)0, "", 6767, 4);
        j.test_addPeer(&peer);
        j.teleportEntity(0);

    }

    {
        TestJuncture j(0);

        j.test_onPeerLost();

    }

    // Peer replied, unconnected this end
    {
        TestJuncture j(0);

        Operation op;
        j.test_onPeerReplied(op);

    }

    // Peer replied, connected this end
    {
        Connection c(*(CommSocket*)0,
                                        *(ServerRouting*)0, "", 4);

        TestJuncture j(&c);

        Operation op;
        j.test_onPeerReplied(op);

    }

    return 0;
}

// stubs

#include "common/log.h"

#include <Atlas/Negotiate.h>
#include "../stubs/common/stubLink.h"

namespace Atlas { namespace Objects { namespace Operation {

int CONNECT_NO = 500;

} } }
#include "../stubs/server/stubServerRouting.h"
#include "../stubs/server/stubLobby.h"
#include "../stubs/common/stubInheritance.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stubProperty.h"
#include "../stubs/server/stubCommPeer.h"

Peer::Peer(CommSocket & client,
           ServerRouting & svr,
           const std::string & addr,
           int port,
           RouterId id) :
      Link(client, id),
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

void Peer::externalOperation(const Operation & op, Link &)
{
}

void Peer::operation(const Operation &op, OpVector &res)
{
}

int Peer::teleportEntity(const LocatedEntity * ent)
{
    return 0;
}

void Peer::cleanTeleports()
{
}

#include "../stubs/server/stubConnection.h"
#include "../stubs/server/stubConnectableRouter.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stublog.h"
#include <common/Shaker.h>

Shaker::Shaker()
{
}

std::string Shaker::generateSalt(size_t length)
{
    return "";
}
