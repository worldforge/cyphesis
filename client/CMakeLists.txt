
add_executable(cyclient
    CharacterClient.cpp
    CreatorClient.cpp
    BaseClient.cpp
    ObserverClient.cpp
    ClientConnection.cpp
    Python_ClientAPI.cpp
    ClientPropertyManager.cpp
    client.cpp
    CyPy_CreatorClient.cpp CyPy_CreatorClient.h
    CyPy_ObserverClient.cpp CyPy_ObserverClient.h)

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
