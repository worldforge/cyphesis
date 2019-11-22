configure_file(buildid.cpp.in buildid.cpp)

add_library(comm
        CommHttpClient.cpp
        CommMetaClient.cpp
        CommMDNSPublisher.cpp
        HttpCache.cpp
        IdleConnector.cpp
        CommAsioListener_impl.h)

if (AVAHI_FOUND)
    #For some reason it seems we need to also link to avahi-common
    target_link_libraries(comm ${AVAHI_LIBRARIES} -lavahi-common)
endif (AVAHI_FOUND)

set(cyphesis_files
        Account.cpp
        Player.cpp
        Admin.cpp
        SystemAccount.cpp
        ServerAccount.cpp
        Persistence.cpp
        EntityFactory.cpp
        EntityFactory_impl.h
        ServerRouting.cpp
        SpawnEntity.cpp
        Spawn.h
        WorldRouter.cpp
        StorageManager.cpp
        CorePropertyManager.cpp
        Ruleset.cpp
        EntityRuleHandler.cpp
        ArchetypeRuleHandler.cpp
        ArchetypeFactory.cpp
        OpRuleHandler.cpp
        PropertyRuleHandler.cpp
        EntityBuilder.cpp
        ArithmeticBuilder.cpp
        Lobby.cpp
        Connection.cpp
        TrustedConnection.cpp
        Peer.cpp
        Juncture.cpp
        TeleportState.cpp
        PossessionAuthenticator.cpp
        TeleportProperty.cpp
        PendingPossession.cpp
        CommPeer.cpp
        CommPythonClient.cpp
        server.cpp
        ExternalMindsConnection.cpp
        ExternalMindsManager.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/buildid.cpp)

if (PostgreSQL_FOUND)
    list(APPEND cyphesis_files
            CommPSQLSocket.cpp)
endif (PostgreSQL_FOUND)

add_library(server
        ${cyphesis_files})

if (PostgreSQL_FOUND)
    target_compile_definitions(server PRIVATE POSTGRES_FOUND)
    target_link_libraries(server ${PostgreSQL_LIBRARIES})
    target_include_directories(server PUBLIC ${PostgreSQL_INCLUDE_DIRS})
endif (PostgreSQL_FOUND)

add_executable(cyphesis
        server.cpp)

target_link_libraries(cyphesis
        server
        navigation
        DetourTileCache
        Detour
        Recast
        rulesai_python
        rulesai
        rulessimulation_python
        rulessimulation
        entityfilter_python
        entityfilter
        rulesbase_python
        rulesbase
        modules
        common
        db
        physics
        comm
        metaserverapi
        )

install(TARGETS cyphesis DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})


#add_executable(cyslave
#        Account.cpp
#        Player.cpp
#        Admin.cpp
#        Persistence.cpp
#        EntityFactory.cpp
#        ServerRouting.cpp
#        WorldRouter.cpp
#        TaskFactory.cpp
#        CorePropertyManager.cpp
#        EntityBuilder.cpp
#        Lobby.cpp
#        Connection.cpp
#        TrustedConnection.cpp
#        SlaveClientConnection.cpp
#        Peer.cpp
#        Master.cpp
#        HttpCache.cpp
#        CommPeer.cpp
#        CommHttpClient.cpp
#        CommMaster.cpp
#        TeleportState.cpp
#        slave.cpp)
#
#target_link_libraries(cyslave
#        scriptpython
#        navigation
#        DetourTileCache
#        Detour
#        Recast
#        rulesetmind
#        rulessimulation
#        entityfilter
#        rulesbase
#        modules
#        common
#        physics
#        metaserverapi
#        comm)
