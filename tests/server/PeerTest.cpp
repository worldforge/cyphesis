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

#include "server/Peer.h"

#include "common/CommAsioClient_impl.h"
#include "server/CommPeer.h"
#include "rules/simulation/ExternalMind.h"
#include "rules/simulation/Entity.h"

#include "rules/simulation/BaseWorld.h"
#include "common/CommSocket.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
#include <string>
#include <rules/simulation/MindsProperty.h>

class TestCommSocket : public CommSocket
{
  public:
    TestCommSocket() : CommSocket(*(boost::asio::io_context*)0)
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

#include "../TestWorld.h"
class MyTestWorld : public TestWorld {
  public:
    explicit MyTestWorld() : TestWorld() {
    }


    LocatedEntity * test_addEntity(LocatedEntity * ent, long intId) {
        m_eobjects[intId] = ent;
        return ent;
    }
    void test_delEntity(long intId) { 
        m_eobjects.erase(intId);
    }


};

class TestPeer : public Peer {
public:
    TestPeer(CommSocket & client, ServerRouting & svr,
         const std::string & addr, RouterId id)
    : Peer(client, svr, addr, 6767, id)
    {
    }

    const std::string& getAccountType() const {
        return m_accountType;
    }
};

Atlas::Objects::Operation::RootOperation stub_CommClient_sent_op(0);

int main()
{
    MyTestWorld world;

    {
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);
    }

    {
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);

        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        p.operation(op, res);

    }

    {
        // Test the setting of authentiaction states
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);

        assert(p.getAuthState() == PEER_INIT);
        p.setAuthState(PEER_AUTHENTICATED);
        assert(p.getAuthState() == PEER_AUTHENTICATED);

    }

    {
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
        p.operation(op, res);
    }

    // Authenticating (no args)
    {
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);

        p.setAuthState(PEER_AUTHENTICATING);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
        p.operation(op, res);
    }

    // Authenticating (empty arg)
    {
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);

        p.setAuthState(PEER_AUTHENTICATING);
        
        Atlas::Objects::Operation::Info op;
        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        OpVector res;
        p.operation(op, res);
    }

    // Authenticating (full arg)
    {
        TestPeer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 1);

        p.setAuthState(PEER_AUTHENTICATING);
        
        Atlas::Objects::Operation::Info op;
        Atlas::Objects::Root arg;
        arg->setId("2");
        arg->setParent("server");
        op->setArgs1(arg);
        OpVector res;
        p.operation(op, res);
        assert(p.getAccountType() == "server");
    }

    // Authenticated (no args)
    {
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);

        p.setAuthState(PEER_AUTHENTICATED);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
        p.operation(op, res);
    }

    {
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);
        
        Atlas::Objects::Operation::Error op;
        OpVector res;
        p.operation(op, res);
    }

    // Not authenticated
    {
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);
        
        Ref<Entity> e(new Entity(3));
        int ret = p.teleportEntity(e.get());
        assert(ret == -1);
    }

    // Authenticated
    {
        TestCommSocket client;
        Peer p(client, *(ServerRouting*)0, "addr", 6767, 1);
        
        p.setAuthState(PEER_AUTHENTICATED);
        
        Ref<Entity> e(new Entity(3));
        int ret = p.teleportEntity(e.get());
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 1);
    }

    // Re-teleport same entity
    {
        TestCommSocket client;
        Peer p(client, *(ServerRouting*)0, "addr", 6767, 1);
        
        p.setAuthState(PEER_AUTHENTICATED);
        
        Ref<Entity> e(new Entity(3));
        int ret = p.teleportEntity(e.get());
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 1);

        ret = p.teleportEntity(e.get());
        assert(ret != 0);
    }

    // Entity (no mind)
    {
        TestCommSocket client;
        Peer p(client, *(ServerRouting*)0, "addr", 6767, 1);
        
        p.setAuthState(PEER_AUTHENTICATED);
        
        Ref<Entity> e(new Entity(3));
        int ret = p.teleportEntity(e.get());
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 1);
    }

    // Entity (external mind, unconnected)
    {
        TestCommSocket client;
        Peer p(client, *(ServerRouting*)0, "addr", 6767, 1);
        
        p.setAuthState(PEER_AUTHENTICATED);
        
        Ref<Entity> e(new Entity(3));
        ExternalMind mind(1, e);
        auto mindsProp = e->modPropertyClassFixed<MindsProperty>();
        mindsProp->addMind(&mind);
        int ret = p.teleportEntity(e.get());
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 1);
    }

    // Entity (external mind, connected)
    {
        TestCommSocket client;
        Peer p(client, *(ServerRouting*)0, "addr", 6767, 1);
        
        p.setAuthState(PEER_AUTHENTICATED);


        Ref<Entity> e(new Entity(3));
        ExternalMind mind(1, e);
        mind.linkUp((Link*)23);
        auto mindsProp = e->modPropertyClassFixed<MindsProperty>();
        mindsProp->addMind(&mind);

        int ret = p.teleportEntity(e.get());
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 1);
    }

    // No arg
    {
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
         
        p.peerTeleportResponse(op, res);
    }

    // Empty arg, no refno
    {
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
         
        p.peerTeleportResponse(op, res);
    }

    // Empty arg, made up refno, not CommPeer
    {
        TestCommSocket client;
        Peer p(client, *(ServerRouting*)0, "addr", 6767, 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p.peerTeleportResponse(op, res);
    }

    // Empty arg, made up refno
    {
        TestCommSocket client;
        Peer p(client, *(ServerRouting*)0, "addr", 6767, 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p.peerTeleportResponse(op, res);
    }

    // Empty arg, refno that matches earlier teleport, not in world
    {
        TestCommSocket client;
        Peer p(client, *(ServerRouting*)0, "addr", 6767, 1);
        
        p.setAuthState(PEER_AUTHENTICATED);
        
        Ref<Entity> e(new Entity(23));
        int ret = p.teleportEntity(e.get());
        assert(ret == 0);

        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p.peerTeleportResponse(op, res);
    }

    // Empty arg, refno that matches earlier teleport, in world
    {
        TestCommSocket client;
        Peer p(client, *(ServerRouting*)0, "addr", 6767, 1);
        
        p.setAuthState(PEER_AUTHENTICATED);
        
        Ref<Entity> e(new Entity(23));
        int ret = p.teleportEntity(e.get());
        assert(ret == 0);

        world.test_addEntity(e.get(), 23);

        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p.peerTeleportResponse(op, res);

        world.test_delEntity(23);
    }

    // Empty arg, refno that matches earlier teleport, with mind
    {
        TestCommSocket client;
        Peer p(client, *(ServerRouting*)0, "addr", 6767, 1);
        
        p.setAuthState(PEER_AUTHENTICATED);
        
        Ref<Entity> e(new Entity(23));
        ExternalMind mind(1, e);
        mind.linkUp((Link*)23);
        auto mindsProp = e->modPropertyClassFixed<MindsProperty>();
        mindsProp->addMind(&mind);
        int ret = p.teleportEntity(e.get());
        assert(ret == 0);

        world.test_addEntity(e.get(), 23);

        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p.peerTeleportResponse(op, res);

        world.test_delEntity(23);
    }

    // No teleports to clear
    {
        Peer p(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, 1);
        
        p.cleanTeleports();
    }

    // One teleport to clear
    {
        TestCommSocket client;
        Peer p(client, *(ServerRouting*)0, "addr", 6767, 1);
        
        p.setAuthState(PEER_AUTHENTICATED);
        
        Ref<Entity> e(new Entity(23));
        int ret = p.teleportEntity(e.get());
        assert(ret == 0);

        p.cleanTeleports();
    }

    return 0;
}

// stubs

#include "server/TeleportState.h"

#include "rules/Script.h"

#include "common/id.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <Atlas/Negotiate.h>

#include <cstdlib>

using Atlas::Message::MapType;

TeleportState::TeleportState(std::chrono::steady_clock::time_point time) : m_state(TELEPORT_NONE),
                                            m_teleportTime(time)
{
}

void TeleportState::setRequested()
{
    m_state = TELEPORT_REQUESTED;
}

void TeleportState::setCreated()
{
    m_state = TELEPORT_CREATED;
}

void TeleportState::setKey(const std::string & key)
{
    m_possessKey = key;
}


CommPeer::CommPeer(const std::string & name,
        boost::asio::io_context& io_context,
                   Atlas::Objects::Factories& factories) :
        CommAsioClient<boost::asio::ip::tcp>(name, io_context, factories), m_auth_timer(io_context)
{
}

CommPeer::~CommPeer()
{
}

int CommSocket::flush()
{
    return 0;
}

#define STUB_ExternalMind_linkUp
void ExternalMind::linkUp(Link * c)
{
    m_link = c;
}

#include "../stubs/rules/simulation/stubExternalMind.h"
#include "../stubs/rules/simulation/stubMindsProperty.h"
#include "../stubs/rules/simulation/stubThing.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stubProperty.h"


#define STUB_Link_send
void Link::send(const Operation & op) const
{
    stub_CommClient_sent_op = op;
}
#include "../stubs/common/stubLink.h"
#include "../stubs/rules/stubScript.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/rules/stubLocation.h"


#ifndef STUB_BaseWorld_getEntity
#define STUB_BaseWorld_getEntity
Ref<LocatedEntity> BaseWorld::getEntity(const std::string & id) const
{
    return getEntity(integerId(id));
}

Ref<LocatedEntity> BaseWorld::getEntity(long id) const
{
    auto I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second);
        return I->second;
    } else {
        return nullptr;
    }
}
#endif //STUB_BaseWorld_getEntity

#include "../stubs/rules/simulation/stubBaseWorld.h"
#include "../stubs/common/stublog.h"
#include "../stubs/common/stubid.h"
