#include "server/TeleportAuthenticator.h"

#include "common/BaseWorld.h"
#include "common/id.h"
#include "common/log.h"

#include "rulesets/Entity.h"

#include <cstdlib>

#include <cassert>

int main()
{
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
        assert(TeleportAuthenticator::instance() != NULL);

        TeleportAuthenticator::instance()->addTeleport("test_entity_id", "test_possess_key");

        // Test non-existent ID authentication request
        assert(TeleportAuthenticator::instance()->authenticateTeleport(
                       "test_non_existent_entity_id", "test_possess_key") == NULL);

        // Test incorrect possess key authentication request
        assert(TeleportAuthenticator::instance()->authenticateTeleport("test_entity_id",
                                            "test_wrong_possess_key") == NULL);

        // Test valid authentication request
        assert(TeleportAuthenticator::instance()->authenticateTeleport("test_entity_id",
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
}

BaseWorld::~BaseWorld()
{
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
}

bool PendingTeleport::isValidated()
{
    return m_valid;
}
