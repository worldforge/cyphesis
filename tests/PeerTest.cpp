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

#include "null_stream.h"

#include "server/Peer.h"

#include "server/CommAsioClient_impl.h"
#include "server/CommPeer.h"
#include "rulesets/ExternalMind.h"

#include "common/BaseWorld.h"
#include "common/CommSocket.h"

#include "rulesets/Character.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
#include <string>

class TestCommSocket : public CommSocket
{
  public:
    TestCommSocket() : CommSocket(*(boost::asio::io_service*)0)
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

class TestWorld : public BaseWorld {
  public:
    explicit TestWorld() : BaseWorld(*(LocatedEntity*)0) {
    }

    virtual bool idle() { return false; }

    virtual LocatedEntity * addEntity(LocatedEntity * ent) { 
        return 0;
    }

    LocatedEntity * test_addEntity(LocatedEntity * ent, long intId) { 
        m_eobjects[intId] = ent;
        return 0;
    }
    void test_delEntity(long intId) { 
        m_eobjects.erase(intId);
    }
    virtual LocatedEntity * addNewEntity(const std::string &,
                                  const Atlas::Objects::Entity::RootEntity &) {
        return 0;
    }
    void delEntity(LocatedEntity * obj) {}
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         LocatedEntity *) { return 0; }
    int removeSpawnPoint(LocatedEntity *) {return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    LocatedEntity * spawnNewEntity(const std::string & name,
                                   const std::string & type,
                                   const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
    virtual int moveToSpawn(const std::string & name,
                            Location& location){return 0;}
    virtual Task * newTask(const std::string &, LocatedEntity &) { return 0; }
    virtual Task * activateTask(const std::string &, const std::string &,
                                LocatedEntity *, LocatedEntity &) { return 0; }
    virtual ArithmeticScript * newArithmetic(const std::string &,
                                             LocatedEntity *) {
        return 0;
    }
    virtual void message(const Operation & op,
                         LocatedEntity & ent) {
        // stub_baseworld_receieved_op = op->getClassNo();
    }
    virtual LocatedEntity * findByName(const std::string & name) { return 0; }
    virtual LocatedEntity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(LocatedEntity *) { }
};

class TestPeer : public Peer {
public:
    TestPeer(CommSocket & client, ServerRouting & svr,
         const std::string & addr, const std::string & id, long iid)
    : Peer(client, svr, addr, 6767, id, iid)
    {
    }

    const std::string& getAccountType() const {
        return m_accountType;
    }
};

Atlas::Objects::Operation::RootOperation stub_CommClient_sent_op(0);

int main()
{
    TestWorld world;

    {
        Peer * p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);
        delete p;
    }

    {
        Peer * p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);

        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        p->operation(op, res);

        delete p;
    }

    {
        // Test the setting of authentiaction states
        Peer * p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);

        assert(p->getAuthState() == PEER_INIT);
        p->setAuthState(PEER_AUTHENTICATED);
        assert(p->getAuthState() == PEER_AUTHENTICATED);

        delete p;
    }

    {
        Peer *p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
        p->operation(op, res);
    }

    // Authenticating (no args)
    {
        Peer *p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);

        p->setAuthState(PEER_AUTHENTICATING);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
        p->operation(op, res);
    }

    // Authenticating (empty arg)
    {
        Peer *p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);

        p->setAuthState(PEER_AUTHENTICATING);
        
        Atlas::Objects::Operation::Info op;
        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        OpVector res;
        p->operation(op, res);
    }

    // Authenticating (full arg)
    {
        TestPeer *p = new TestPeer(*(CommSocket*)0, *(ServerRouting*)0, "addr", "1", 1);

        p->setAuthState(PEER_AUTHENTICATING);
        
        Atlas::Objects::Operation::Info op;
        Atlas::Objects::Root arg;
        arg->setId("2");
        arg->setParent("server");
        op->setArgs1(arg);
        OpVector res;
        p->operation(op, res);
        assert(p->getAccountType() == "server");
    }

    // Authenticated (no args)
    {
        Peer *p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);

        p->setAuthState(PEER_AUTHENTICATED);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
        p->operation(op, res);
    }

    {
        Peer *p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        Atlas::Objects::Operation::Error op;
        OpVector res;
        p->operation(op, res);
    }

    // Not authenticated
    {
        Peer *p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        Entity e("3", 3);
        int ret = p->teleportEntity(&e);
        assert(ret == -1);
    }

    // Authenticated
    {
        TestCommSocket client;
        Peer *p = new Peer(client, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Entity e("3", 3);
        int ret = p->teleportEntity(&e);
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 1);
    }

    // Re-teleport same entity
    {
        TestCommSocket client;
        Peer *p = new Peer(client, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Entity e("3", 3);
        int ret = p->teleportEntity(&e);
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 1);

        ret = p->teleportEntity(&e);
        assert(ret != 0);
    }

    // Character (no mind)
    {
        TestCommSocket client;
        Peer *p = new Peer(client, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Character e("3", 3);
        int ret = p->teleportEntity(&e);
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 1);
    }

    // Character (externl mind, unconnected)
    {
        TestCommSocket client;
        Peer *p = new Peer(client, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Character e("3", 3);
        e.m_externalMind = new ExternalMind(e);
        int ret = p->teleportEntity(&e);
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 1);
    }

    // Character (externl mind, connected)
    {
        TestCommSocket client;
        Peer *p = new Peer(client, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Character e("3", 3);
        ExternalMind * mind = new ExternalMind(e);
        mind->linkUp((Link*)23);
        e.m_externalMind = mind;
        int ret = p->teleportEntity(&e);
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 2);
    }

    // No arg
    {
        Peer *p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
         
        p->peerTeleportResponse(op, res);
    }

    // Empty arg, no refno
    {
        Peer *p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
         
        p->peerTeleportResponse(op, res);
    }

    // Empty arg, made up refno, not CommPeer
    {
        TestCommSocket client;
        Peer *p = new Peer(client, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p->peerTeleportResponse(op, res);
    }

    // Empty arg, made up refno
    {
        TestCommSocket client;
        Peer *p = new Peer(client, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p->peerTeleportResponse(op, res);
    }

    // Empty arg, refno that matches earlier teleport, not in world
    {
        TestCommSocket client;
        Peer *p = new Peer(client, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Entity e("23", 23);
        int ret = p->teleportEntity(&e);
        assert(ret == 0);

        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p->peerTeleportResponse(op, res);
    }

    // Empty arg, refno that matches earlier teleport, in world
    {
        TestCommSocket client;
        Peer *p = new Peer(client, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Entity e("23", 23);
        int ret = p->teleportEntity(&e);
        assert(ret == 0);

        world.test_addEntity(&e, 23);

        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p->peerTeleportResponse(op, res);

        world.test_delEntity(23);
    }

    // Empty arg, refno that matches earlier teleport, with mind
    {
        TestCommSocket client;
        Peer *p = new Peer(client, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Character e("23", 23);
        ExternalMind * mind = new ExternalMind(e);
        mind->linkUp((Link*)23);
        e.m_externalMind = mind;
        int ret = p->teleportEntity(&e);
        assert(ret == 0);

        world.test_addEntity(&e, 23);

        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p->peerTeleportResponse(op, res);

        world.test_delEntity(23);
    }

    // No teleports to clear
    {
        Peer *p = new Peer(*(CommSocket*)0, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        p->cleanTeleports();
    }

    // One teleport to clear
    {
        TestCommSocket client;
        Peer *p = new Peer(client, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Entity e("23", 23);
        int ret = p->teleportEntity(&e);
        assert(ret == 0);

        p->cleanTeleports();
    }

    return 0;
}

// stubs

#include "server/TeleportState.h"

#include "rulesets/Character.h"
#include "rulesets/Script.h"

#include "common/id.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <Atlas/Negotiate.h>

#include <cstdlib>

using Atlas::Message::MapType;

TeleportState::TeleportState(boost::posix_time::ptime time) : m_state(TELEPORT_NONE),
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
        boost::asio::io_service& io_service) :
        CommAsioClient<boost::asio::ip::tcp>(name, io_service), m_auth_timer(io_service)
{
}

CommPeer::~CommPeer()
{
}

CommSocket::CommSocket(boost::asio::io_service& io_service) : m_io_service(io_service) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
}

ExternalMind::ExternalMind(LocatedEntity & e) : Router(e.getId(), e.getIntId()),
                                         m_external(0), m_entity(e)
{
}

ExternalMind::~ExternalMind()
{
}

void ExternalMind::externalOperation(const Operation &, Link &)
{
}

void ExternalMind::operation(const Operation & op, OpVector & res)
{
}

void ExternalMind::linkUp(Link * c)
{
    m_external = c;
}

#include "stubs/rulesets/stubCharacter.h"
#include "stubs/rulesets/stubThing.h"
#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/common/stubRouter.h"

Link::Link(CommSocket & socket, const std::string & id, long iid) :
            Router(id, iid), m_encoder(0), m_commSocket(socket)
{
}

Link::~Link()
{
}

void Link::send(const Operation & op) const
{
    stub_CommClient_sent_op = op;
}

void Link::disconnect()
{
}

#include "stubs/rulesets/stubScript.h"
#include "stubs/common/stubTypeNode.h"
#include "stubs/modules/stubLocation.h"


BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    long intId = integerId(id);

    EntityDict::const_iterator I = m_eobjects.find(intId);
    if (I != m_eobjects.end()) {
        assert(I->second != 0);
        return I->second;
    } else {
        return 0;
    }
}

LocatedEntity * BaseWorld::getEntity(long id) const
{
    EntityDict::const_iterator I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second != 0);
        return I->second;
    } else {
        return 0;
    }
}

void log(LogLevel lvl, const std::string & msg)
{
}

void logEvent(LogEvent lev, const std::string & msg)
{
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}
