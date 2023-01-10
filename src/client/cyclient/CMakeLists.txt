
add_executable(cyclient
        CharacterClient.cpp
        CreatorClient.cpp
        BaseClient.cpp
        ObserverClient.cpp
        ClientConnection.cpp
        Python_ClientAPI.cpp
        client.cpp
        CyPy_CreatorClient.cpp
        CyPy_ObserverClient.cpp)

target_link_libraries(cyclient
        client
        navigation
        DetourTileCache
        Detour
        Recast
        rulesai_python
        rulesai
        entityfilter_python
        entityfilter
        atlas_python
        common_python
        physics_python
        rulesbase_python
        rulesbase
        modules
        common
        physics
        )

install(TARGETS cyclient DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
