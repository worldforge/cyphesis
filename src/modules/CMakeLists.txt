add_library(modules
        DateTime.cpp
        WorldTime.cpp
        WeakEntityRef.cpp)


target_link_libraries(modules PUBLIC
        common
        )
