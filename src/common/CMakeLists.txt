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
        system_net.cpp system_uid.cpp
        system_prefix.cpp
        serialno.cpp
        TypeNode.cpp
        Inheritance.cpp
        Inheritance.h
        Property.cpp
        PropertyManager.cpp
        Router.cpp
        AtlasFileLoader.cpp
        Monitors.cpp
        Variable.cpp
        AtlasStreamClient.cpp
        ClientTask.cpp
        EntityKit.cpp
        ScriptKit.h
        Link.cpp
        Shaker.cpp
        OperationsDispatcher.cpp
        RuleTraversalTask.cpp
        AtlasQuery.h
        operations/Commune.h
        compose.hpp
        operations/Connect.h
        operations/Monitor.h
        nls.h
        op_switch.h
        operations/Possess.h
        Property_impl.h
        PropertyFactory_impl.h
        PropertyInstanceState.h
        random.h
        operations/Setup.h
        sockets.h
        operations/Teleport.h
        operations/Think.h
        operations/Tick.h
        type_utils_impl.h
        types.h
        operations/Update.h
        FileSystemObserver.cpp
        FileSystemObserver.h
        AssetsManager.cpp
        AssetsManager.h
        Singleton.h
        RepeatedTask.cpp
        RepeatedTask.h
        operations/Thought.h
        operations/Relay.h
        Visibility.h
        MainLoop.cpp
        MainLoop.h
        CommAsioClient_impl.h
        TypeStore.h
        )

target_link_libraries(common ${GCRYPT_LIBRARIES})
target_include_directories(common PUBLIC ${GCRYPT_INCLUDE_DIRS})

target_compile_definitions(common PUBLIC -DBINDIR="${CMAKE_INSTALL_FULL_BINDIR}"
        -DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}"
        -DSYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}"
        -DLOCALSTATEDIR="${CMAKE_INSTALL_FULL_LOCALSTATEDIR}")


set(db_files
        DatabaseSQLite.cpp)

if (PostgreSQL_FOUND)
    list(APPEND db_files
            DatabasePostgres.cpp
            CommPSQLSocket.cpp)

endif (PostgreSQL_FOUND)

add_library(db ${db_files})

if (PostgreSQL_FOUND)
    target_link_libraries(db ${PostgreSQL_LIBRARIES})
    target_include_directories(db PUBLIC ${PostgreSQL_INCLUDE_DIRS})
endif (PostgreSQL_FOUND)

target_link_libraries(db ${SQLITE3_LIBRARIES})
target_include_directories(db PUBLIC ${SQLITE3_INCLUDE_DIRS})


add_library(tools
        Storage.cpp
        FormattedXMLWriter.cpp)



