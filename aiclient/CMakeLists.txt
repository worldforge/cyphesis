
add_executable(cyaiclient
    ../common/BaseClient.cpp
    PossessionClient.cpp
    aiclient.cpp
    PossessionAccount.cpp
    )


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
