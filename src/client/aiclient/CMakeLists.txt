
add_executable(cyaiclient
        PossessionClient.cpp
        aiclient.cpp
        PossessionAccount.cpp)

#If Python has been built statically (as with Conan) we need to enable exports so the dynamic loading works.
if (PYTHON_IS_STATIC)
    set_property(TARGET cyaiclient PROPERTY ENABLE_EXPORTS 1)
endif ()

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
