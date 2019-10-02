

add_executable(cycmd
    cycmd.cpp
    Flusher.cpp
    OperationMonitor.cpp
    EntityExporterBase.cpp
    EntityExporter.cpp
    EntityImporterBase.cpp
    EntityImporter.cpp
    AdminClient.cpp
    IdContext.cpp
    AccountContext.cpp
    AvatarContext.cpp
    ConnectionContext.cpp
    JunctureContext.cpp
    Interactive.cpp)

target_link_libraries(cycmd ${READLINE_LIBRARY})

target_link_libraries(cycmd
        common)
install(TARGETS cycmd DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cyconfig cyconfig.cpp)
target_link_libraries(cyconfig
        common)
install(TARGETS cyconfig DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cypasswd cypasswd.cpp)
target_link_libraries(cypasswd
        tools
        common
        db)
install(TARGETS cypasswd DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cydb cydb.cpp)

target_link_libraries(cydb ${READLINE_LIBRARY})

target_link_libraries(cydb
        tools
        common
        db)
install(TARGETS cydb DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cypython cypython.cpp)
target_link_libraries(cypython ${READLINE_LIBRARY})
target_link_libraries(cypython
        common)
install(TARGETS cypython DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cyexport cyexport.cpp EntityExporterBase.cpp
    EntityExporter.cpp AgentCreationTask.cpp
    WaitForDeletionTask.cpp
        )
target_link_libraries(cyexport
        common)
install(TARGETS cyexport DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cyimport cyimport.cpp EntityImporterBase.cpp
    EntityImporter.cpp AgentCreationTask.cpp
    EntityTraversalTask.cpp WaitForDeletionTask.cpp)
target_link_libraries(cyimport
        common)
install(TARGETS cyimport DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

