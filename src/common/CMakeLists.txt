add_library(common
        const.cpp
        binreloc.c
        newid.cpp
        id.cpp
        log.cpp
        debug.cpp
        type_utils.cpp
        utils.cpp
        operations.cpp
        custom.cpp
        client_socket.cpp
        globals.cpp
        Database.cpp
        system.cpp
        system_net.cpp
        system_uid.cpp
        system_prefix.cpp
        serialno.cpp
        TypeNode.cpp
        Inheritance.cpp
        Property.cpp
        PropertyManager.cpp
        Router.cpp
        AtlasFileLoader.cpp
        Monitors.cpp
        Variable.cpp
        AtlasStreamClient.cpp
        ClientTask.cpp
        Link.cpp
        Shaker.cpp
        OperationsDispatcher.cpp
        RuleTraversalTask.cpp
        FileSystemObserver.cpp
        AssetsManager.cpp
        RepeatedTask.cpp
        MainLoop.cpp
        net/CommHttpClient.cpp
        net/HttpCache.cpp
        FormattedXMLWriter.cpp
        )

target_link_libraries(common PUBLIC ${GCRYPT_LIBRARIES} remotery)
target_include_directories(common PUBLIC ${GCRYPT_INCLUDE_DIRS})

target_compile_definitions(common PUBLIC -DBINDIR="${CMAKE_INSTALL_FULL_BINDIR}"
        -DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}"
        -DSYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}"
        -DLOCALSTATEDIR="${CMAKE_INSTALL_FULL_LOCALSTATEDIR}")


set(db_files
        Storage.cpp
        DatabaseSQLite.cpp)

if (PostgreSQL_FOUND)
    list(APPEND db_files
            DatabasePostgres.cpp
            CommPSQLSocket.cpp)

endif (PostgreSQL_FOUND)

add_library(db ${db_files})

if (PostgreSQL_FOUND)
    target_link_libraries(db PUBLIC ${PostgreSQL_LIBRARIES})
    target_include_directories(db PUBLIC ${PostgreSQL_INCLUDE_DIRS})
endif (PostgreSQL_FOUND)

target_link_libraries(db PUBLIC ${SQLITE3_LIBRARIES} remotery)
target_include_directories(db PUBLIC ${SQLITE3_INCLUDE_DIRS})




