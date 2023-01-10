
add_executable(cyaiclient
        PossessionClient.cpp
        aiclient.cpp
        PossessionAccount.cpp)


target_link_libraries(cyaiclient
        client
        rulesai_python
        rulesai
        entityfilter_python
        entityfilter
        navigation
        rulesbase_python
        atlas_python
        common_python
        physics_python
        rulesbase
        modules
        common
        physics
        DetourTileCache
        Detour
        Recast
        remotery
        )

install(TARGETS cyaiclient DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
