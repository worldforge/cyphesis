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

#include "rulesets/ExternalMind.h"

#include "server/Connection.h"

#include "rulesets/Entity.h"

#include "common/BaseWorld.h"

#include <Atlas/Objects/Operation.h>

#include <cassert>

class TestExternalMind : public ExternalMind
{
  public:
    TestExternalMind(Entity & e) : ExternalMind(e) { }

    void test_deleteEntity(const std::string & id) {
        deleteEntity(id);
    }

    void test_purgeEntity(const LocatedEntity & ent) {
        purgeEntity(ent);
    }

    void test_setLossTime(double t) { m_lossTime = t; }
};

int stub_baseworld_receieved_op = -1;
int stub_link_send_op = -1;
int stub_link_send_count = 0;

class TestWorld : public BaseWorld {
  public:
    explicit TestWorld() : BaseWorld(*(LocatedEntity*)0) {
        m_realTime = 100000;
    }

    virtual bool idle(const SystemTime &) { return false; }
    virtual LocatedEntity * addEntity(LocatedEntity * ent) { 
        return 0;
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
    virtual void message(const Operation & op, LocatedEntity & ent) {
        stub_baseworld_receieved_op = op->getClassNo();
    }
    virtual LocatedEntity * findByName(const std::string & name) { return 0; }
    virtual LocatedEntity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(LocatedEntity *) { }
};

int main()
{
    TestWorld world;

    {
        Entity e("2", 2);

        ExternalMind em(e);
    }

    {
        Entity e("2", 2);

        TestExternalMind em(e);

        em.test_deleteEntity("3");
    }

    // Purge with empty contains
    {
        Entity e("2", 2);
        e.m_contains = new LocatedEntitySet;

        TestExternalMind em(e);

        em.test_purgeEntity(e);
    }

    // Purge with populated contains
    {
        Entity e("2", 2);
        e.m_contains = new LocatedEntitySet;
        e.m_contains->insert(new Entity("3", 3));

        TestExternalMind em(e);

        em.test_purgeEntity(e);
    }

    // Connect to nothing
    {
        Entity e("2", 2);

        ExternalMind em(e);

        em.linkUp(0);
    }

    // Connect to something
    {
        Entity e("2", 2);

        ExternalMind em(e);

        em.linkUp(new Connection(*(CommSocket*)0,
                                 *(ServerRouting*)0,
                                 "addr", "4", 4));
    }

    // Connect to something, then disconnect
    {
        Entity e("2", 2);

        ExternalMind em(e);

        em.linkUp(new Connection(*(CommSocket*)0,
                                 *(ServerRouting*)0,
                                 "addr", "4", 4));
        em.linkUp(0);
    }

    // Connect to something, then check connection ID
    {
        Entity e("2", 2);

        ExternalMind em(e);

        em.linkUp(new Connection(*(CommSocket*)0,
                                 *(ServerRouting*)0,
                                 "addr", "4", 4));
        const std::string & id = em.connectionId();
        assert(id == "4");
    }

    // Send a random operation
    {
        Entity e("2", 2);

        TestExternalMind em(e);
        em.test_setLossTime(BaseWorld::instance().getTime());

        stub_baseworld_receieved_op = -1;
        OpVector res;
        em.operation(Atlas::Objects::Operation::RootOperation(), res);
        assert(stub_baseworld_receieved_op == -1);
    }

    // Send a Delete operation
    {
        Entity e("2", 2);

        TestExternalMind em(e);
        em.test_setLossTime(BaseWorld::instance().getTime());

        stub_baseworld_receieved_op = -1;
        OpVector res;
        em.operation(Atlas::Objects::Operation::Delete(), res);
        assert(stub_baseworld_receieved_op == -1);
    }

    // Send a Delete operation to an ephemeral entity
    {
        Entity e("2", 2);
        e.setFlags(entity_ephem);

        TestExternalMind em(e);
        em.test_setLossTime(BaseWorld::instance().getTime());

        stub_baseworld_receieved_op = -1;
        OpVector res;
        em.operation(Atlas::Objects::Operation::Delete(), res);
        assert(stub_baseworld_receieved_op == -1);
    }

    // Send a non Delete operation to an ephemeral entity
    {
        Entity e("2", 2);
        e.setFlags(entity_ephem);

        TestExternalMind em(e);
        em.test_setLossTime(BaseWorld::instance().getTime());

        stub_baseworld_receieved_op = -1;
        OpVector res;
        em.operation(Atlas::Objects::Operation::RootOperation(), res);
        assert(stub_baseworld_receieved_op == Atlas::Objects::Operation::DELETE_NO);
    }

    // Send a random operation to something that has been recorded as
    // disconnected for a while
    {
        Entity e("2", 2);

        // FIXME hard code the expiry time here.
        TestExternalMind em(e);
        em.test_setLossTime(BaseWorld::instance().getTime() - 60*60 - 5);

        stub_baseworld_receieved_op = -1;
        OpVector res;
        em.operation(Atlas::Objects::Operation::RootOperation(), res);
        assert(stub_baseworld_receieved_op == Atlas::Objects::Operation::DELETE_NO);
    }

    // Send a random operation to a connected mind
    {
        Entity e("2", 2);

        TestExternalMind em(e);

        em.linkUp(new Connection(*(CommSocket*)0,
                                 *(ServerRouting*)0,
                                 "addr", "4", 4));

        stub_link_send_op = -1;
        stub_link_send_count = 0;
        OpVector res;
        em.operation(Atlas::Objects::Operation::RootOperation(), res);
        assert(stub_link_send_op == Atlas::Objects::Operation::ROOT_OPERATION_NO);
        assert(stub_link_send_count == 1);
    }

    // Send a Sight operation to a connected mind
    {
        Entity e("2", 2);

        TestExternalMind em(e);

        em.linkUp(new Connection(*(CommSocket*)0,
                                 *(ServerRouting*)0,
                                 "addr", "4", 4));

        stub_link_send_op = -1;
        stub_link_send_count = 0;
        OpVector res;
        em.operation(Atlas::Objects::Operation::Sight(), res);
        assert(stub_link_send_op == Atlas::Objects::Operation::SIGHT_NO);
        assert(stub_link_send_count == 1);
    }

    // Send a Sight(Set) of hungry operation to a connected mind
    {
        Entity e("2", 2);

        TestExternalMind em(e);

        em.linkUp(new Connection(*(CommSocket*)0,
                                 *(ServerRouting*)0,
                                 "addr", "4", 4));

        stub_link_send_op = -1;
        stub_link_send_count = 0;

        // A sight(set) of a starving entity
        Atlas::Objects::Root arg;
        arg->setAttr("status", 0.05);
        arg->setId(e.getId());
        Atlas::Objects::Operation::Set set;
        set->setArgs1(arg);
        Atlas::Objects::Operation::Sight op;
        op->setArgs1(set);

        // It should trigger an extra Sight(Imaginary)
        OpVector res;
        em.operation(op, res);
        assert(stub_link_send_op == Atlas::Objects::Operation::SIGHT_NO);
        assert(stub_link_send_count > 1);
    }

    // Send a Sight(Set) of starving operation to a connected mind
    {
        Entity e("2", 2);

        TestExternalMind em(e);

        em.linkUp(new Connection(*(CommSocket*)0,
                                 *(ServerRouting*)0,
                                 "addr", "4", 4));

        stub_link_send_op = -1;
        stub_link_send_count = 0;

        // A sight(set) of a starving entity
        Atlas::Objects::Root arg;
        arg->setAttr("status", 0.005);
        arg->setId(e.getId());
        Atlas::Objects::Operation::Set set;
        set->setArgs1(arg);
        Atlas::Objects::Operation::Sight op;
        op->setArgs1(set);

        // It should trigger an extra Sight(Imaginary)
        OpVector res;
        em.operation(op, res);
        assert(stub_link_send_op == Atlas::Objects::Operation::SIGHT_NO);
        assert(stub_link_send_count > 1);
    }

    return 0;
}

// stubs

#include "rulesets/Script.h"

#include "common/log.h"

using Atlas::Message::MapType;

Connection::Connection(CommSocket & client,
                       ServerRouting & svr,
                       const std::string & addr,
                       const std::string & id, long iid) :
            Link(client, id, iid), m_obsolete(false),
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


void Connection::externalOperation(const Operation &, Link &)
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

void Entity::installDelegate(int class_no, const std::string & delegate)
{
}

Domain * Entity::getMovementDomain()
{
    return 0;
}

void Entity::sendWorld(const Operation & op)
{
    BaseWorld::instance().message(op, *this);
}

void Entity::onContainered(const LocatedEntity*)
{
}

void Entity::onUpdated()
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

void LocatedEntity::onContainered(const LocatedEntity*)
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

Location::Location() : m_loc(0)
{
}

Location::Location(LocatedEntity * rf, const Point3D & pos)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
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
    stub_link_send_op = op->getClassNo();
    ++stub_link_send_count;
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

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

void log(LogLevel lvl, const std::string & msg)
{
}
