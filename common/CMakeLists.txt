add_library(common
    const.cpp
    binreloc.c
    newid.cpp id.cpp
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
    PropertyFactory.cpp
    PropertyManager.cpp
    OperationRouter.cpp
    Router.cpp
    BaseWorld.cpp
    AtlasFileLoader.cpp
    Monitors.cpp
    Variable.cpp
    AtlasStreamClient.cpp
    ClientTask.cpp
    SystemTime.cpp
    EntityKit.cpp
    ScriptKit.h
    TaskKit.cpp
    CommSocket.cpp
    Link.cpp
    atlas_helpers.cpp
    Shaker.cpp
    OperationsDispatcher.cpp
    RuleTraversalTask.cpp
    AtlasQuery.h
    Actuate.h
    Add.h
    Affect.h
    Attack.h
    Burn.h
    Commune.h
    compose.hpp
    Connect.h
    Drop.h
    Eat.h
    Monitor.h
    nls.h
    Nourish.h
    op_switch.h
    Pickup.h
    Possess.h
    Property_impl.h
    PropertyFactory_impl.h
    PropertyInstanceState.h
    random.h
    Setup.h
    sockets.h
    Teleport.h
    Think.h
    Tick.h
    type_utils_impl.h
    types.h
    Unseen.h
    Update.h
    FileSystemObserver.cpp
    FileSystemObserver.h
    AssetsManager.cpp
    AssetsManager.h
    Singleton.h
    Change.h)

target_link_libraries(common ${GCRYPT_LIBRARIES})
target_include_directories(common PUBLIC ${GCRYPT_INCLUDE_DIRS})

target_compile_definitions(common PUBLIC -DBINDIR="${CMAKE_INSTALL_FULL_BINDIR}" -DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}" -DSYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}" -DLOCALSTATEDIR="${CMAKE_INSTALL_FULL_LOCALSTATEDIR}")

add_library(db
    DatabasePostgres.cpp
    DatabasePostgres.h
    DatabaseSQLite.cpp
    DatabaseSQLite.h)
target_link_libraries(db ${PostgreSQL_LIBRARIES})
target_include_directories(db PUBLIC ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(db ${SQLITE3_LIBRARIES})
target_include_directories(db PUBLIC ${SQLITE3_INCLUDE_DIRS})


add_library(tools
    Storage.cpp
    FormattedXMLWriter.cpp)



