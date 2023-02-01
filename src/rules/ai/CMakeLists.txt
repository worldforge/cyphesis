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
        mercator::mercator)

add_subdirectory(python)

