
add_executable(cyaiclient
    ClientConnection.cpp
    BaseClient.cpp
    PossessionClient.cpp
    aiclient.cpp
    PossessionAccount.cpp
    MindRegistry.h PendingMind.cpp PendingMind.h)


target_link_libraries(cyaiclient
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
    )

install(TARGETS cyaiclient DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
