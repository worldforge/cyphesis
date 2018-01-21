add_library(navigation
    Awareness.cpp
    fastlz.c
    Steering.cpp
    AwarenessUtils.h
    IHeightProvider.h)

target_include_directories(navigation PUBLIC external/RecastDetour/Recast/Include external/RecastDetour/Detour/Include external/RecastDetour/DetourTileCache/Include)

add_subdirectory(external/RecastDetour/Recast)
add_subdirectory(external/RecastDetour/Detour)
add_subdirectory(external/RecastDetour/DetourTileCache)
