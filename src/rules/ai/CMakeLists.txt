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

target_link_libraries(rulesai
        remotery)


add_subdirectory(python)

