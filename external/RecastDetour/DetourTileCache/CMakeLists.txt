add_library(DetourTileCache
        Source/DetourTileCache.cpp
        Source/DetourTileCacheBuilder.cpp)

target_include_directories(DetourTileCache PUBLIC Include ../Detour/Include)
target_link_libraries(DetourTileCache PUBLIC Detour)
