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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"
#include "TestWorld.h"
#include "null_stream.h"

#include "server/Connection.h"
#include "server/Lobby.h"
#include "server/Player.h"
#include "server/ServerAccount.h"
#include "server/ServerRouting.h"
#include "server/SystemAccount.h"

#include "rulesets/Character.h"

#include "common/id.h"
#include "common/TypeNode.h"
#include "common/CommSocket.h"

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

class AccountConnectionintegration : public Cyphesis::TestBase {
  protected:
    Entity * m_tlve;
    BaseWorld * m_world;
    ServerRouting * m_server;
    Connection * m_connection;
  public:
    AccountConnectionintegration();

    void setup();
    void teardown();

    void test_account_creation();

    Connection * connection() const { return m_connection; }
    ServerRouting * server() const { return m_server; }
};

AccountConnectionintegration::AccountConnectionintegration()
{
    ADD_TEST(AccountConnectionintegration::test_account_creation);
}

void AccountConnectionintegration::setup()
{
    m_tlve = new Entity("0", 0);
    m_world = new SpawningTestWorld(*m_tlve);
    m_server = new ServerRouting(*m_world,
                                 "testrules",
                                 "testname",
                                 "1", 1, "2", 2);
    m_connection = new Connection(*new TestCommSocket(),
                                  *m_server,
                                  "test_addr",
                                  "3", 3);
}

void AccountConnectionintegration::teardown()
{
    delete m_connection;
    delete m_server;
    delete m_world;
}

static OpVector test_sent_ops;

void AccountConnectionintegration::test_account_creation()
{
    // Basic player account creation
    {

        ASSERT_NOT_NULL(m_connection);
        ASSERT_TRUE(m_connection->objects().empty());

        Create op;
        Anonymous create_arg;
        create_arg->setParents(std::list<std::string>(1, "player"));
        create_arg->setAttr("username", "39d409ec");
        create_arg->setAttr("password", "6a6e71bab281");
        op->setArgs1(create_arg);

        ASSERT_TRUE(test_sent_ops.empty());

        // Send the operation to create the account
        m_connection->externalOperation(op, *m_connection);

        // There should be a response op
        ASSERT_TRUE(!test_sent_ops.empty());
        ASSERT_EQUAL(test_sent_ops.size(), 1u);
        // and the account creation should have created an object bound
        // to this connection.
        ASSERT_TRUE(!m_connection->objects().empty());

        // Check the response is an info indicating successful account
        // creation.
        const Operation & reply = test_sent_ops.front();
        ASSERT_EQUAL(reply->getClassNo(), Atlas::Objects::Operation::INFO_NO);
        // The Info response should have an argument describing the created
        // account
        const std::vector<Root> & reply_args = reply->getArgs();
        ASSERT_TRUE(!reply_args.empty());
        RootEntity account = smart_dynamic_cast<RootEntity>(reply_args.front());
        ASSERT_TRUE(account.isValid());

        // The account ID should be provided
        ASSERT_TRUE(!account->isDefaultId());
        const std::string account_id = account->getId();
        ASSERT_TRUE(!account_id.empty());

        // Check the account has been registered in the server object
        Router * account_router_ptr = m_server->getObject(account_id);
        ASSERT_NOT_NULL(account_router_ptr);

        // Check the account has been logged into the lobby
        const AccountDict & lobby_dict = m_server->m_lobby.getAccounts();
        AccountDict::const_iterator I = lobby_dict.find(account_id);
        ASSERT_TRUE(I != lobby_dict.end());
        Account * account_ptr = I->second;
        ASSERT_EQUAL(account_router_ptr, account_ptr);

        // Basic login as now been established by account creation

        // Set up some other account details
        create_arg->setAttr("username", "89cae312");
        create_arg->setAttr("password", "d730b8bd2d6c");

        // and try an additional account creation, which should fail.
        // Multiple logins are ok, but there is no reason to allow multiple
        // account creations.
        test_sent_ops.clear();
        m_connection->externalOperation(op, *m_connection);
        ASSERT_TRUE(!test_sent_ops.empty());
        ASSERT_EQUAL(test_sent_ops.size(), 1u);

        const Operation & error_reply = test_sent_ops.front();
        ASSERT_EQUAL(error_reply->getClassNo(),
                     Atlas::Objects::Operation::ERROR_NO);

        Player::playableTypes.insert(test_valid_character_type);

        Anonymous character_arg;
        character_arg->setParents(std::list<std::string>(1, test_valid_character_type));
        character_arg->setName("938862f2-4db2-4e8e-b944-7b0935e569db");

        Create character_op;
        character_op->setArgs1(character_arg);
        character_op->setFrom(account_id);

        test_sent_ops.clear();
        m_connection->externalOperation(character_op, *m_connection);
        // FIXME the above went through Account::externalOperation, so there
        // is no reply in res. The reply has gone directly to the Link::send
        // method. Add a way of checking, once there are better stubs.
        ASSERT_TRUE(!test_sent_ops.empty());
        ASSERT_EQUAL(test_sent_ops.size(), 2u);

        const Operation & create_reply = test_sent_ops.front();
        ASSERT_EQUAL(create_reply->getClassNo(),
                     Atlas::Objects::Operation::INFO_NO);


        // TODO Character creation etc?
        // TODO Lobby interaction?
        // TODO Logout ?
    }
}

int main()
{
    AccountConnectionintegration t;

    return t.run();
}

// stubs

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}

#include "rulesets/ExternalMind.h"
#include "rulesets/ExternalProperty.h"
#include "server/Juncture.h"
#include "server/Persistence.h"
#include "server/Ruleset.h"
#include "server/PossessionAuthenticator.h"

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

const char * const CYPHESIS = "cyphesisAccountConnectionintegration";
int timeoffset = 0;
std::string instance(CYPHESIS);

CommSocket::CommSocket(boost::asio::io_service & svr) : m_io_service(svr) { }

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

void ExternalMind::externalOperation(const Operation & op, Link &)
{
}

void ExternalMind::operation(const Operation & op, OpVector & res)
{
}

const std::string & ExternalMind::connectionId()
{
    assert(m_external != 0);
    return m_external->getId();
}

void ExternalMind::linkUp(Link * c)
{
    m_external = c;
}

ExternalProperty::ExternalProperty(ExternalMind * & data) : m_data(data)
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

ExternalProperty * ExternalProperty::copy() const
{
    return 0;
}

Juncture::Juncture(Connection * c,
                   const std::string & id, long iid) :
          ConnectableRouter(id, iid, c),
                                                       m_peer(0)
{
}

Juncture::~Juncture()
{
}

void Juncture::externalOperation(const Operation & op, Link &)
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

Link::Link(CommSocket & socket, const std::string & id, long iid) :
            Router(id, iid), m_encoder(0), m_commSocket(socket)
{
}

Link::~Link()
{
}

void Link::send(const Operation & op) const
{
    test_sent_ops.push_back(op);
}

void Link::sendError(const Operation & op,
                     const std::string &,
                     const std::string &) const
{
}

void Link::disconnect()
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

PossessionAuthenticator * PossessionAuthenticator::m_instance = NULL;

int PossessionAuthenticator::addPossession(const std::string &entity_id,
                                        const std::string &possess_key)
{
    return 0;
}

int PossessionAuthenticator::removePossession(const std::string &entity_id)
{
    return 0;
}

LocatedEntity *PossessionAuthenticator::authenticatePossession(const std::string &entity_id,
                                            const std::string &possess_key)
{
    return 0;
}

Persistence * Persistence::m_instance = NULL;

Persistence::Persistence() : m_db(*(Database*)0)
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

void Persistence::addCharacter(const Account &, const LocatedEntity &)
{
}

void Persistence::delCharacter(const std::string &)
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
         Character(id, intId)
{
}

Creator::~Creator()
{
}

void Creator::operation(const Operation & op, OpVector & res)
{
}

void Creator::externalOperation(const Operation & op, Link &)
{
}

void Creator::mindLookOperation(const Operation & op, OpVector & res)
{
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

int Character::linkExternal(Link * link)
{
    return 0;
}

int Character::unlinkExternal(Link * link)
{
    return 0;
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

void Character::RelayOperation(const Operation & op, OpVector &)
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

void Entity::RelayOperation(const Operation &, OpVector &)
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

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return m_properties[name] = prop;
}

PropertyBase * Entity::modProperty(const std::string & name)
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
void hash_password(const std::string & pwd, const std::string & salt,
                   std::string & hash)
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
