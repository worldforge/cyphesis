
add_executable(cyaiclient
    ../common/BaseClient.cpp
    PossessionClient.cpp
    aiclient.cpp
    PossessionAccount.cpp
    AiClientPropertyManager.cpp)


target_link_libraries(cyaiclient
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
