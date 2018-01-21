configure_file(buildid.cpp.in buildid.cpp)

add_library(comm
    CommHttpClient.cpp
    CommMetaClient.cpp
    CommMDNSPublisher.cpp
    HttpCache.cpp
    CommAsioListener.cpp
    CommAsioClient.cpp
    IdleConnector.cpp
    CommAsioClient_impl.h
    CommAsioListener_impl.h)

if (AVAHI_FOUND)
    #For some reason it seems we need to also link to avahi-common
    target_link_libraries(comm ${AVAHI_LIBRARIES} -lavahi-common)
endif (AVAHI_FOUND)

add_executable(cyphesis
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
    TaskFactory.cpp
    CorePropertyManager.cpp
    Ruleset.cpp
    RuleHandler.cpp
    EntityRuleHandler.cpp
    ArchetypeRuleHandler.cpp
    ArchetypeFactory.cpp
    OpRuleHandler.cpp
    PropertyRuleHandler.cpp
    TaskRuleHandler.cpp
    EntityBuilder.cpp
    ArithmeticBuilder.cpp
    Lobby.cpp
    ConnectableRouter.cpp
    Connection.cpp
    TrustedConnection.cpp
    Peer.cpp
    Juncture.cpp
    CommPSQLSocket.cpp
    TeleportState.cpp
    PossessionAuthenticator.cpp
    TeleportProperty.cpp
    PendingPossession.cpp
    CommPeer.cpp
    CommPythonClient.cpp
    server.cpp
    MindInspector.cpp
    ExternalMindsConnection.cpp
    ExternalMindsManager.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/buildid.cpp)


target_link_libraries(cyphesis
    scriptpython
    navigation
    DetourTileCache
    Detour
    Recast
    rulesetmind
    rulesetentity
    entityfilter
    rulesetbase
    modules
    common
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
#        rulesetentity
#        entityfilter
#        rulesetbase
#        modules
#        common
#        physics
#        metaserverapi
#        comm)
