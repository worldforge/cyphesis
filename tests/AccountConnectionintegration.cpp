// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#include "TestWorld.h"
#include "null_stream.h"

#include "server/CommClient.h"
#include "server/CommServer.h"
#include "server/Connection.h"
#include "server/Lobby.h"
#include "server/Player.h"
#include "server/ServerAccount.h"
#include "server/ServerRouting.h"
#include "server/SystemAccount.h"

#include "rulesets/Character.h"

#include "common/id.h"
#include "common/TypeNode.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Imaginary;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Talk;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::smart_dynamic_cast;

static const char * test_valid_character_type = "37702ce7a151";

class SpawningTestWorld : public TestWorld {
  public:
    SpawningTestWorld(Entity & gw) : TestWorld(gw) { }

    virtual Entity * addEntity(Entity * ent) { 
        ent->m_location.m_loc = &m_gameWorld;
        ent->m_location.m_pos = WFMath::Point<3>(0,0,0);
        m_eobjects[ent->getIntId()] = ent;
        return ent;
    }

    virtual Entity * addNewEntity(const std::string & t,
                                  const Atlas::Objects::Entity::RootEntity &) {
        std::string id;
        long intId = newId(id);

        Entity * e = new Character(id, intId);

        e->setType(new TypeNode(t));
        return addEntity(e);
    }
};

class TestCommClient : public CommClient<null_stream> {
  public:
    TestCommClient(CommServer & cs) : CommClient<null_stream>(cs, "") { }
};

class TestContext {
  protected:
    Entity * m_tlve;
    BaseWorld * m_world;
    ServerRouting * m_server;
    CommServer * m_cs;
    Connection * m_connection;
  public:
    TestContext()
    {
        m_tlve = new Entity("0", 0);
        m_world = new SpawningTestWorld(*m_tlve);
        m_server = new ServerRouting(*m_world,
                                     "testrules",
                                     "testname",
                                     "1", 1, "2", 2);
        m_cs = new CommServer;
        m_connection = new Connection(*new TestCommClient(*m_cs),
                                      *m_server,
                                      "test_addr",
                                      "3", 3);

    }

    ~TestContext()
    {
        delete m_connection;
        delete m_cs;
        delete m_server;
        delete m_world;
    }

    Connection * connection() const { return m_connection; }
    ServerRouting * server() const { return m_server; }
};

int main()
{
    {
        TestContext scope;
    }

    // Basic player account creation
    {
        TestContext scope;

        assert(scope.connection() != 0);
        assert(scope.connection()->objects().empty());

        Create op;
        Anonymous create_arg;
        create_arg->setParents(std::list<std::string>(1, "player"));
        create_arg->setAttr("username", "39d409ec");
        create_arg->setAttr("password", "6a6e71bab281");
        op->setArgs1(create_arg);

        OpVector res;
        assert(res.empty());

        // Send the operation to create the account
        scope.connection()->operation(op, res);

        // There should be a response op
        assert(!res.empty());
        assert(res.size() == 1);
        // and the account creation should have created an object bound
        // to this connection.
        assert(!scope.connection()->objects().empty());

        // Check the response is an info indicating successful account
        // creation.
        const Operation & reply = res.front();
        assert(reply->getClassNo() == Atlas::Objects::Operation::INFO_NO);
        // The Info response should have an argument describing the created
        // account
        const std::vector<Root> & reply_args = reply->getArgs();
        assert(!reply_args.empty());
        RootEntity account = smart_dynamic_cast<RootEntity>(reply_args.front());
        assert(account.isValid());

        // The account ID should be provided
        assert(!account->isDefaultId());
        const std::string account_id = account->getId();
        assert(!account_id.empty());

        // Check the account has been registered in the server object
        Router * account_router_ptr = scope.server()->getObject(account_id);
        assert(account_router_ptr != 0);

        // Check the account has been logged into the lobby
        const AccountDict & lobby_dict = scope.server()->m_lobby.getAccounts();
        AccountDict::const_iterator I = lobby_dict.find(account_id);
        assert(I != lobby_dict.end());
        Account * account_ptr = I->second;
        assert(account_router_ptr == account_ptr);

        // Basic login as now been established by account creation

        // Set up some other account details
        create_arg->setAttr("username", "89cae312");
        create_arg->setAttr("password", "d730b8bd2d6c");

        // and try an additional account creation, which should fail.
        // Multiple logins are ok, but there is no reason to allow multiple
        // account creations.
        res.clear();
        scope.connection()->operation(op, res);
        assert(!res.empty());
        assert(res.size() == 1);

        const Operation & error_reply = res.front();
        assert(error_reply->getClassNo() == Atlas::Objects::Operation::ERROR_NO);

        res.clear();

        Player::playableTypes.insert(test_valid_character_type);

        Anonymous character_arg;
        character_arg->setParents(std::list<std::string>(1, test_valid_character_type));
        character_arg->setName("938862f2-4db2-4e8e-b944-7b0935e569db");

        Create character_op;
        character_op->setArgs1(character_arg);
        character_op->setFrom(account_id);

        scope.connection()->operation(character_op, res);
        assert(!res.empty());
        assert(res.size() == 2);

        const Operation & create_reply = res.front();
        assert(create_reply->getClassNo() == Atlas::Objects::Operation::INFO_NO);


        // TODO Character creation etc?
        // TODO Lobby interaction?
        // TODO Logout ?
    }
}

// stubs

#include "server/CommClient.h"
#include "server/ExternalMind.h"
#include "server/ExternalProperty.h"
#include "server/Juncture.h"
#include "server/Persistence.h"
#include "server/Ruleset.h"
#include "server/TeleportAuthenticator.h"

#include "rulesets/Creator.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/PropertyManager.h"
#include "common/Variable.h"

#include <cstdlib>

bool database_flag = false;

namespace consts {

  const char * buildTime = __TIME__;
  const char * buildDate = __DATE__;
  const int buildId = -1;
  const char * version = "test_build";
}

// globals - why do we have these again?

const char * CYPHESIS = "cyphesisAccountConnectionintegration";
int timeoffset = 0;
std::string instance(CYPHESIS);

CommServer::CommServer() : m_congested(false)
{
}

CommServer::~CommServer()
{
}

Idle::Idle(CommServer & svr) : m_idleManager(svr)
{
}

Idle::~Idle()
{
}

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
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

const std::string & ExternalMind::connectionId()
{
    assert(m_connection != 0);
    return m_connection->getId();
}

void ExternalMind::connect(Connection * c)
{
    m_connection = c;
}

ExternalProperty::ExternalProperty(Router * & data) : m_data(data)
{
}

int ExternalProperty::get(Atlas::Message::Element & val) const
{
    return 0;
}

void ExternalProperty::set(const Atlas::Message::Element & val)
{
}

void ExternalProperty::add(const std::string & s,
                         Atlas::Message::MapType & map) const
{
}

void ExternalProperty::add(const std::string & s,
                         const Atlas::Objects::Entity::RootEntity & ent) const
{
}

Juncture::Juncture(Connection * c,
                   const std::string & id, long iid) :
          ConnectedRouter(id, iid, c),
                                                       m_socket(0),
                                                       m_peer(0)
{
}

Juncture::~Juncture()
{
}

void Juncture::operation(const Operation & op, OpVector & res)
{
}

void Juncture::addToMessage(MapType & omap) const
{
}

void Juncture::addToEntity(const RootEntity & ent) const
{
}

Ruleset * Ruleset::m_instance = NULL;

int Ruleset::installRule(const std::string & class_name,
                         const std::string & section,
                         const Root & class_desc)
{
    return 0;
}

int Ruleset::modifyRule(const std::string & class_name,
                        const Root & class_desc)
{
    return 0;
}

TeleportAuthenticator * TeleportAuthenticator::m_instance = NULL;

int TeleportAuthenticator::addTeleport(const std::string &entity_id,
                                        const std::string &possess_key)
{
    return 0;
}

int TeleportAuthenticator::removeTeleport(const std::string &entity_id)
{
    return 0;
}

Entity *TeleportAuthenticator::authenticateTeleport(const std::string &entity_id,
                                            const std::string &possess_key)
{
    return 0;
}

Persistence * Persistence::m_instance = NULL;

Persistence::Persistence() : m_connection(*(Database*)0)
{
}

Persistence * Persistence::instance()
{
    if (m_instance == NULL) {
        m_instance = new Persistence();
    }
    return m_instance;
}

void Persistence::registerCharacters(Account & ac,
                                     const EntityDict & worldObjects)
{
}

Account * Persistence::getAccount(const std::string & name)
{
    return 0;
}

void Persistence::putAccount(const Account & ac)
{
}

void log(LogLevel lvl, const std::string & msg)
{
    std::cout << msg << std::endl;
}

void logEvent(LogEvent lev, const std::string & msg)
{
}

Creator::Creator(const std::string & id, long intId) :
         Creator_parent(id, intId)
{
}

Creator::~Creator()
{
}

void Creator::operation(const Operation & op, OpVector & res)
{
}

void Creator::externalOperation(const Operation & op)
{
}

void Creator::mindLookOperation(const Operation & op, OpVector & res)
{
}

Character::Character(const std::string & id, long intId) :
           Character_parent(id, intId),
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

void Character::externalOperation(const Operation & op)
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

PropertyBase * Entity::setAttr(const std::string & name,
                               const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return m_properties[name] = prop;
}

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_contains(0)
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

void LocatedEntity::onContainered()
{
}

void LocatedEntity::onUpdated()
{
}

PropertyManager * PropertyManager::m_instance = 0;

VariableBase::~VariableBase()
{
}

template <typename T>
Variable<T>::Variable(const T & variable) : m_variable(variable)
{
}

template <typename T>
Variable<T>::~Variable()
{
}

template <typename T>
void Variable<T>::send(std::ostream & o)
{
}

template class Variable<int>;
template class Variable<std::string>;
template class Variable<const char *>;

Monitors * Monitors::m_instance = NULL;

Monitors::Monitors()
{
}

Monitors::~Monitors()
{
}

Monitors * Monitors::instance()
{
    if (m_instance == NULL) {
        m_instance = new Monitors();
    }
    return m_instance;
}

void Monitors::insert(const std::string & key, const Element & val)
{
}

void Monitors::watch(const::std::string & name, VariableBase * monitor)
{
}

Location::Location() : m_loc(0)
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

static long idGenerator = 500;

#include <cstdio>

long newId(std::string & id)
{
    static char buf[32];
    long new_id = ++idGenerator;
    sprintf(buf, "%ld", new_id);
    id = buf;
    assert(!id.empty());
    return new_id;
}

void encrypt_password(const std::string & pwd, std::string & hash)
{
}

int check_password(const std::string & pwd, const std::string & hash)
{
    return 0;
}

bool_config_register::bool_config_register(bool & var,
                                           const char * section,
                                           const char * setting,
                                           const char * help)
{
}
