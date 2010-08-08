#include "server/TeleportAuthenticator.h"

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
