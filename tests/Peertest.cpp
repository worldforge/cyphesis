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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "null_stream.h"
#include "CommClient_stub_impl.h"
#include "CommStreamClient_stub_impl.h"

#include "server/Peer.h"

#include "server/CommPeer.h"
#include "server/CommServer.h"
#include "rulesets/ExternalMind.h"

#include "common/BaseWorld.h"

#include "rulesets/Character.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <skstream/skstream.h>

#include <cassert>
#include <string>

class TestCommClient : public CommClient<null_stream> {
  public:
    TestCommClient(CommServer & cs,
                   const std::string & n) : CommClient<null_stream>(cs, n) { }
};

class TestWorld : public BaseWorld {
  public:
    explicit TestWorld() : BaseWorld(*(LocatedEntity*)0) {
        m_realTime = 100000;
    }

    virtual bool idle(const SystemTime &) { return false; }

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
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         LocatedEntity *) { return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    LocatedEntity * spawnNewEntity(const std::string & name,
                                   const std::string & type,
                                   const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
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
    CommServer server;

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
        std::list<std::string> parents;
        parents.push_back("server");
        arg->setParents(parents);
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
        TestCommClient client(server, "");
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
        TestCommClient client(server, "");
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
        TestCommClient client(server, "");
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
        TestCommClient client(server, "");
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
        TestCommClient client(server, "");
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
        TestCommClient *peerConn = new TestCommClient(*(CommServer*)0, "name");
        Peer *p = new Peer(*peerConn, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p->peerTeleportResponse(op, res);
    }

    // Empty arg, made up refno
    {
        CommPeer *peerConn = new CommPeer(*(CommServer*)0, "name");
        Peer *p = new Peer(*peerConn, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p->peerTeleportResponse(op, res);
    }

    // Empty arg, refno that matches earlier teleport, not in world
    {
        CommPeer *peerConn = new CommPeer(server, "name");
        Peer *p = new Peer(*peerConn, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
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
        CommPeer *peerConn = new CommPeer(server, "name");
        Peer *p = new Peer(*peerConn, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
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
        CommPeer *peerConn = new CommPeer(server, "name");
        Peer *p = new Peer(*peerConn, *(ServerRouting*)0, "addr", 6767, "1", 1);
        
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
        TestCommClient client(server, "");
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

#include "server/CommServer.h"
#include "server/TeleportState.h"

#include "rulesets/Character.h"
#include "rulesets/Script.h"

#include "common/id.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <Atlas/Negotiate.h>

#include <cstdlib>

using Atlas::Message::MapType;

TeleportState::TeleportState(time_t time) : m_state(TELEPORT_NONE),
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

CommServer::CommServer() : m_congested(false)
{
}

CommServer::~CommServer()
{
}

CommPeer::CommPeer(CommServer & svr, const std::string & n) :
      CommClient<tcp_socket_stream>(svr, n)
{
}

CommPeer::~CommPeer()
{
}

bool CommPeer::eof()
{
    return false;
}

void CommPeer::idle(time_t t)
{
}

template class CommStreamClient<tcp_socket_stream>;
template class CommClient<tcp_socket_stream>;

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

ExternalMind::ExternalMind(Entity & e) : Router(e.getId(), e.getIntId()),
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

Character::Character(const std::string & id, long intId) :
           Thing(id, intId),
               m_movement(*(Movement*)0),
               m_mind(0), m_externalMind(0)
{
}

Character::~Character()
{
}

void Character::operation(const Operation & op, OpVector &)
{
}

void Character::externalOperation(const Operation & op, Link &)
{
}


void Character::ImaginaryOperation(const Operation & op, OpVector &)
{
}

void Character::InfoOperation(const Operation & op, OpVector &)
{
}

void Character::TickOperation(const Operation & op, OpVector &)
{
}

void Character::TalkOperation(const Operation & op, OpVector &)
{
}

void Character::NourishOperation(const Operation & op, OpVector &)
{
}

void Character::UseOperation(const Operation & op, OpVector &)
{
}

void Character::WieldOperation(const Operation & op, OpVector &)
{
}

void Character::AttackOperation(const Operation & op, OpVector &)
{
}

void Character::ActuateOperation(const Operation & op, OpVector &)
{
}

void Character::mindActuateOperation(const Operation &, OpVector &)
{
}

void Character::mindAttackOperation(const Operation &, OpVector &)
{
}

void Character::mindCombineOperation(const Operation &, OpVector &)
{
}

void Character::mindCreateOperation(const Operation &, OpVector &)
{
}

void Character::mindDeleteOperation(const Operation &, OpVector &)
{
}

void Character::mindDivideOperation(const Operation &, OpVector &)
{
}

void Character::mindEatOperation(const Operation &, OpVector &)
{
}

void Character::mindGoalInfoOperation(const Operation &, OpVector &)
{
}

void Character::mindImaginaryOperation(const Operation &, OpVector &)
{
}

void Character::mindLookOperation(const Operation &, OpVector &)
{
}

void Character::mindMoveOperation(const Operation &, OpVector &)
{
}

void Character::mindSetOperation(const Operation &, OpVector &)
{
}

void Character::mindSetupOperation(const Operation &, OpVector &)
{
}

void Character::mindTalkOperation(const Operation &, OpVector &)
{
}

void Character::mindThoughtOperation(const Operation &, OpVector &)
{
}

void Character::mindTickOperation(const Operation &, OpVector &)
{
}

void Character::mindTouchOperation(const Operation &, OpVector &)
{
}

void Character::mindUpdateOperation(const Operation &, OpVector &)
{
}

void Character::mindUseOperation(const Operation &, OpVector &)
{
}

void Character::mindWieldOperation(const Operation &, OpVector &)
{
}


void Character::mindOtherOperation(const Operation &, OpVector &)
{
}

void Character::sendMind(const Operation & op, OpVector & res)
{
}

Thing::Thing(const std::string & id, long intId) :
       Entity(id, intId)
{
}

Thing::~Thing()
{
}

void Thing::DeleteOperation(const Operation & op, OpVector & res)
{
}

void Thing::MoveOperation(const Operation & op, OpVector & res)
{
}

void Thing::SetOperation(const Operation & op, OpVector & res)
{
}

void Thing::LookOperation(const Operation & op, OpVector & res)
{
}

void Thing::CreateOperation(const Operation & op, OpVector & res)
{
}

void Thing::UpdateOperation(const Operation & op, OpVector & res)
{
}

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId), m_motion(0)
{
}

Entity::~Entity()
{
}

void Entity::destroy()
{
    destroyed.emit();
}

void Entity::ActuateOperation(const Operation &, OpVector &)
{
}

void Entity::AppearanceOperation(const Operation &, OpVector &)
{
}

void Entity::AttackOperation(const Operation &, OpVector &)
{
}

void Entity::CombineOperation(const Operation &, OpVector &)
{
}

void Entity::CreateOperation(const Operation &, OpVector &)
{
}

void Entity::DeleteOperation(const Operation &, OpVector &)
{
}

void Entity::DisappearanceOperation(const Operation &, OpVector &)
{
}

void Entity::DivideOperation(const Operation &, OpVector &)
{
}

void Entity::EatOperation(const Operation &, OpVector &)
{
}

void Entity::GetOperation(const Operation &, OpVector &)
{
}

void Entity::InfoOperation(const Operation &, OpVector &)
{
}

void Entity::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Entity::LookOperation(const Operation &, OpVector &)
{
}

void Entity::MoveOperation(const Operation &, OpVector &)
{
}

void Entity::NourishOperation(const Operation &, OpVector &)
{
}

void Entity::SetOperation(const Operation &, OpVector &)
{
}

void Entity::SightOperation(const Operation &, OpVector &)
{
}

void Entity::SoundOperation(const Operation &, OpVector &)
{
}

void Entity::TalkOperation(const Operation &, OpVector &)
{
}

void Entity::TickOperation(const Operation &, OpVector &)
{
}

void Entity::TouchOperation(const Operation &, OpVector &)
{
}

void Entity::UpdateOperation(const Operation &, OpVector &)
{
}

void Entity::UseOperation(const Operation &, OpVector &)
{
}

void Entity::WieldOperation(const Operation &, OpVector &)
{
}

void Entity::externalOperation(const Operation & op, Link &)
{
}

void Entity::operation(const Operation & op, OpVector & res)
{
}

void Entity::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

PropertyBase * Entity::setAttr(const std::string & name,
                               const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return 0;
}

void Entity::installHandler(int class_no, Handler handler)
{
}

void Entity::installDelegate(int class_no, const std::string & delegate)
{
}

Domain * Entity::getMovementDomain()
{
    return 0;
}

void Entity::sendWorld(const Operation & op)
{
}

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

void Entity::callOperation(const Operation & op, OpVector & res)
{
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

int LocatedEntity::getAttr(const std::string & name,
                           Atlas::Message::Element & attr) const
{
    return -1;
}

int LocatedEntity::getAttrType(const std::string & name,
                               Atlas::Message::Element & attr,
                               int type) const
{
    return -1;
}

PropertyBase * LocatedEntity::setAttr(const std::string & name,
                                      const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * LocatedEntity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * LocatedEntity::setProperty(const std::string & name,
                                          PropertyBase * prop)
{
    return 0;
}

void LocatedEntity::installHandler(int, Handler)
{
}

void LocatedEntity::installDelegate(int, const std::string &)
{
}

void LocatedEntity::destroy()
{
}

Domain * LocatedEntity::getMovementDomain()
{
    return 0;
}

void LocatedEntity::sendWorld(const Operation & op)
{
}

void LocatedEntity::onContainered()
{
}

void LocatedEntity::onUpdated()
{
}

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
}

void LocatedEntity::merge(const MapType & ent)
{
}

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

Script::Script()
{
}

/// \brief Script destructor
Script::~Script()
{
}

bool Script::operation(const std::string & opname,
                       const Atlas::Objects::Operation::RootOperation & op,
                       OpVector & res)
{
   return false;
}

void Script::hook(const std::string & function, LocatedEntity * entity)
{
}

void Location::addToMessage(MapType & omap) const
{
}

Location::Location() : m_loc(0)
{
}

Location::Location(LocatedEntity * rf, const Point3D & pos)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

TypeNode::~TypeNode()
{
}

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
