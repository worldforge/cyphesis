
add_executable(cyaiclient
    PossessionClient.cpp
    aiclient.cpp
    PossessionAccount.cpp
    ../ClientPropertyManager.cpp)


target_link_libraries(cyaiclient
    client
    rulesai_python
    rulesai
    entityfilter_python
    entityfilter
    rulesbase_python
    rulesbase
    navigation
    modules
    common
    physics
    DetourTileCache
    Detour
    Recast
    )

install(TARGETS cyaiclient DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
