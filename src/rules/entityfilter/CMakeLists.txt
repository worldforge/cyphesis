add_library(entityfilter
        Filter.cpp
        ParserDefinitions.cpp
        Providers.cpp
        Predicates.cpp
        ProviderFactory.cpp)

add_subdirectory(python)

target_link_libraries(entityfilter
        common)


