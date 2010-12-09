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

#include "server/Peer.h"

#include "server/CommPeer.h"
#include "server/ExternalMind.h"

#include "common/BaseWorld.h"

#include "rulesets/Character.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
#include <string>

class TestWorld : public BaseWorld {
  public:
    explicit TestWorld() : BaseWorld(*(Entity*)0) {
        m_realTime = 100000;
    }

    virtual bool idle(int, int) { return false; }

    virtual Entity * addEntity(Entity * ent) { 
        return 0;
    }

    Entity * test_addEntity(Entity * ent, long intId) { 
        m_eobjects[intId] = ent;
        return 0;
    }
    void test_delEntity(long intId) { 
        m_eobjects.erase(intId);
    }
    virtual Entity * addNewEntity(const std::string &,
                                  const Atlas::Objects::Entity::RootEntity &) {
        return 0;
    }
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         Entity *) { return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    Entity * spawnNewEntity(const std::string & name,
                            const std::string & type,
                            const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
    virtual Task * newTask(const std::string &, Character &) { return 0; }
    virtual Task * activateTask(const std::string &, const std::string &,
                                const std::string &, Character &) { return 0; }
    virtual ArithmeticScript * newArithmetic(const std::string &, Entity *) {
        return 0;
    }
    virtual void message(const Operation & op, Entity & ent) {
        // stub_baseworld_receieved_op = op->getClassNo();
    }
    virtual Entity * findByName(const std::string & name) { return 0; }
    virtual Entity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(Entity *) { }
};

Atlas::Objects::Operation::RootOperation stub_CommClient_sent_op(0);

int main()
{
    TestWorld world;

    {
        Peer * p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        delete p;
    }

    {
        Peer * p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);

        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        p->operation(op, res);

        delete p;
    }

    {
        // Test the setting of authentiaction states
        Peer * p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);

        assert(p->getAuthState() == PEER_INIT);
        p->setAuthState(PEER_AUTHENTICATED);
        assert(p->getAuthState() == PEER_AUTHENTICATED);

        delete p;
    }

    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
        p->operation(op, res);
    }

    // Authenticating (no args)
    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);

        p->setAuthState(PEER_AUTHENTICATING);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
        p->operation(op, res);
    }

    // Authenticating (empty arg)
    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);

        p->setAuthState(PEER_AUTHENTICATING);
        
        Atlas::Objects::Operation::Info op;
        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        OpVector res;
        p->operation(op, res);
    }

    // Authenticating (full arg)
    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);

        p->setAuthState(PEER_AUTHENTICATING);
        
        Atlas::Objects::Operation::Info op;
        Atlas::Objects::Root arg;
        arg->setId("2");
        op->setArgs1(arg);
        OpVector res;
        p->operation(op, res);
    }

    // Authenticated (no args)
    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);

        p->setAuthState(PEER_AUTHENTICATED);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
        p->operation(op, res);
    }

    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
        Atlas::Objects::Operation::Error op;
        OpVector res;
        p->operation(op, res);
    }

    // Not authenticated
    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
        Entity e("3", 3);
        int ret = p->teleportEntity(&e);
        assert(ret == -1);
    }

    // Authenticated
    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Entity e("3", 3);
        int ret = p->teleportEntity(&e);
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 1);
    }

    // Re-teleport same entity
    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
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
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Character e("3", 3);
        int ret = p->teleportEntity(&e);
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 1);
    }

    // Character (externl mind, unconnected)
    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
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
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Character e("3", 3);
        ExternalMind * mind = new ExternalMind(e);
        mind->connect((Connection*)23);
        e.m_externalMind = mind;
        int ret = p->teleportEntity(&e);
        assert(ret == 0);
        assert(stub_CommClient_sent_op.isValid());
        assert(stub_CommClient_sent_op->getArgs().size() == 2);
    }

    // No arg
    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;
         
        p->peerTeleportResponse(op, res);
    }

    // Empty arg, no refno
    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
         
        p->peerTeleportResponse(op, res);
    }

    // Empty arg, made up refno, not CommPeer
    {
        CommClient *peerConn = new CommClient(*(CommServer*)0);
        Peer *p = new Peer(*peerConn, *(ServerRouting*)0, "addr", "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p->peerTeleportResponse(op, res);
    }

    // Empty arg, made up refno
    {
        CommPeer *peerConn = new CommPeer(*(CommServer*)0);
        Peer *p = new Peer(*peerConn, *(ServerRouting*)0, "addr", "1", 1);
        
        Atlas::Objects::Operation::Info op;
        OpVector res;

        Atlas::Objects::Root arg;
        op->setArgs1(arg);
        op->setRefno(23);
         
        p->peerTeleportResponse(op, res);
    }

    // Empty arg, refno that matches earlier teleport, not in world
    {
        CommPeer *peerConn = new CommPeer(*(CommServer*)0);
        Peer *p = new Peer(*peerConn, *(ServerRouting*)0, "addr", "1", 1);
        
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
        CommPeer *peerConn = new CommPeer(*(CommServer*)0);
        Peer *p = new Peer(*peerConn, *(ServerRouting*)0, "addr", "1", 1);
        
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
        CommPeer *peerConn = new CommPeer(*(CommServer*)0);
        Peer *p = new Peer(*peerConn, *(ServerRouting*)0, "addr", "1", 1);
        
        p->setAuthState(PEER_AUTHENTICATED);
        
        Character e("23", 23);
        ExternalMind * mind = new ExternalMind(e);
        mind->connect((Connection*)23);
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
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
        p->cleanTeleports();
    }

    // One teleport to clear
    {
        Peer *p = new Peer(*(CommClient*)0, *(ServerRouting*)0, "addr", "1", 1);
        
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

Script noScript;

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

CommPeer::CommPeer(CommServer & svr) : CommClient(svr)
{
}

CommPeer::~CommPeer()
{
}

void CommPeer::idle(time_t t)
{
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
    stub_CommClient_sent_op = op;
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

ExternalMind::ExternalMind(Entity & e) : Router(e.getId(), e.getIntId()),
                                         m_connection(0), m_entity(e)
{
}

ExternalMind::~ExternalMind()
{
}

void ExternalMind::operation(const Operation & op, OpVector & res)
{
}

void ExternalMind::connect(Connection * c)
{
    m_connection = c;
}

Character::Character(const std::string & id, long intId) :
           Character_parent(id, intId),
               m_movement(*(Movement*)0),
               m_task(0), m_mind(0), m_externalMind(0)
{
}

Character::~Character()
{
}

void Character::operation(const Operation & op, OpVector &)
{
}

void Character::externalOperation(const Operation & op)
{
}


void Character::ImaginaryOperation(const Operation & op, OpVector &)
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
       Thing_parent(id, intId)
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
        LocatedEntity(id, intId), m_motion(0), m_flags(0)
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

void Entity::externalOperation(const Operation & op)
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

void Entity::setAttr(const std::string & name,
                     const Atlas::Message::Element & attr)
{
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
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
               m_script(&noScript), m_type(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

bool LocatedEntity::getAttr(const std::string & name, Atlas::Message::Element & attr) const
{
    return false;
}

bool LocatedEntity::getAttrType(const std::string & name,
                                Atlas::Message::Element & attr,
                                int type) const
{
    return false;
}

void LocatedEntity::setAttr(const std::string & name, const Atlas::Message::Element & attr)
{
    return;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
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

TypeNode::TypeNode() : m_parent(0)
{
}

TypeNode::~TypeNode()
{
}

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(Entity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

Entity * BaseWorld::getEntity(const std::string & id) const
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

Entity * BaseWorld::getEntity(long id) const
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
