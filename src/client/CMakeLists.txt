add_subdirectory(cyclient)
add_subdirectory(aiclient)


add_library(client
        ClientPropertyManager.cpp
        BaseClient.cpp)

target_link_libraries(client PUBLIC
        common)
