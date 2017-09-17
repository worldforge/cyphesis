
add_executable(cyclient
        CharacterClient.cpp
        CreatorClient.cpp
        BaseClient.cpp
        ObserverClient.cpp
        ClientConnection.cpp
        Py_ObserverClient.cpp
        Py_CreatorClient.cpp
        Python_ClientAPI.cpp
        ClientPropertyManager.cpp
        client.cpp)

target_link_libraries(cyclient ${PYTHON_LIBRARIES})

target_link_libraries(cyclient
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

install(TARGETS cyclient DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
