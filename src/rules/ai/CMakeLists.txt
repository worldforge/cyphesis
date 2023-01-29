add_library(rulesai
        BaseMind.cpp
        MemEntity.cpp
        MemMap.cpp
        AwareMind.cpp
        AwareMindFactory.cpp
        AwarenessStore.cpp
        AwarenessStoreProvider.cpp
        SharedTerrain.cpp
        TypeResolver.cpp
        )

target_link_libraries(rulesai PUBLIC
        remotery
        navigation
        entityfilter
        physics
        rulesbase
        ${mercator_LIBRARIES}
        )

target_link_directories(rulesai PUBLIC
        ${mercator_LIB_DIR}
        )

target_include_directories(rulesai PUBLIC
        ${mercator_INCLUDE_DIR}
        )


add_subdirectory(python)

