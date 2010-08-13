#include "server/TeleportAuthenticator.h"

#include "common/BaseWorld.h"
#include "common/id.h"
#include "common/log.h"

#include "rulesets/Entity.h"

#include <Atlas/Objects/Operation.h>

#include <cstdlib>

#include <cassert>

using Atlas::Objects::Entity::RootEntity;

int stub_baseworld_receieved_op = -1;
int stub_connection_send_op = -1;
int stub_connection_send_count = 0;

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
        stub_baseworld_receieved_op = op->getClassNo();
    }
    virtual Entity * findByName(const std::string & name) { return 0; }
    virtual Entity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(Entity *) { }
};

int main()
{
    TestWorld world;

    {
        // Initially the instance should be NULL (constructor should work?)
        assert(TeleportAuthenticator::instance() == NULL);
        // Check for correct initialization of singleton
        TeleportAuthenticator::init();
        assert(TeleportAuthenticator::instance() != NULL);
        // Check for correct deletion of singleton
        TeleportAuthenticator::del();
        assert(TeleportAuthenticator::instance() == NULL);
    }

    {
        TeleportAuthenticator::init();

        // Check for correct singleton instancing
        assert(TeleportAuthenticator::instance() != NULL);
        
        // Test isPending() function
        assert(!TeleportAuthenticator::instance()->isPending("test_non_existent_entity_id"));

        // Test adding of teleport entries
        assert(TeleportAuthenticator::instance()->addTeleport("test_entity_id", "test_possess_key") == 0);
        assert(TeleportAuthenticator::instance()->isPending("test_entity_id"));
        assert(TeleportAuthenticator::instance()->addTeleport("test_entity_id", "test_possess_key") == -1);

        // Test removal of teleport entries
        assert(TeleportAuthenticator::instance()->removeTeleport("test_non_existent_entity_id") == -1);
        assert(TeleportAuthenticator::instance()->removeTeleport("test_entity_id") == 0);
        assert(!TeleportAuthenticator::instance()->isPending("test_entity_id"));

        TeleportAuthenticator::del();
        assert(TeleportAuthenticator::instance() == NULL);
    }

    {
        TeleportAuthenticator::init();
        Entity ent("100", 100);
        world.test_addEntity(&ent, 100);
        assert(TeleportAuthenticator::instance() != NULL);

        TeleportAuthenticator::instance()->addTeleport("100", "test_possess_key");

        // Test non-existent ID authentication request
        assert(TeleportAuthenticator::instance()->authenticateTeleport(
                       "101", "test_possess_key") == NULL);

        // Test incorrect possess key authentication request
        assert(TeleportAuthenticator::instance()->authenticateTeleport("100",
                                            "test_wrong_possess_key") == NULL);

        // Test valid authentication request
        assert(TeleportAuthenticator::instance()->authenticateTeleport("100",
                                            "test_possess_key") != NULL);

        TeleportAuthenticator::del();
        assert(TeleportAuthenticator::instance() == NULL);
    }
    
    return 0;
}

// Stubs

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

void log(LogLevel lvl, const std::string & msg)
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

PendingTeleport::PendingTeleport(const std::string &id, const std::string &key) 
                                            :   m_entity_id(id),
                                                m_possess_key(key),
                                                m_valid(false)
{
}

const std::string & PendingTeleport::getPossessKey()
{
    return m_possess_key;
}

const std::string & PendingTeleport::getEntityID()
{
    return m_entity_id;
}

bool PendingTeleport::validate(const std::string &entity_id,
                                const std::string &possess_key)
{
    if(m_entity_id == entity_id && m_possess_key == possess_key) {
        return true;
    } else {
        return false;
    }
}

bool PendingTeleport::setValidated()
{
    m_valid = true;
    return m_valid;
}

bool PendingTeleport::isValidated()
{
    return m_valid;
}

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId), m_motion(0), m_flags(0)
{
}

Entity::~Entity()
{
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

void Entity::addToEntity(const RootEntity & ent) const
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

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
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

Location::Location()
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

void Router::addToEntity(const RootEntity & ent) const
{
}
