add_definitions(-DTESTDATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

if (AVAHI_FOUND)
    link_libraries(${AVAHI_LIBRARIES} -lavahi-common)
endif (AVAHI_FOUND)
if (PostgreSQL_FOUND)
    link_libraries(PostgreSQL::PostgreSQL)
endif (PostgreSQL_FOUND)

#Just link all libraries here.
link_libraries(
        pycxx
        ${BULLET_LIBRARIES}
        mercator::mercator
        wfmath::wfmath
        Atlas::Atlas
        Atlas::AtlasCodecs
        Atlas::AtlasNet
        Atlas::AtlasMessage
        Atlas::AtlasObjects
        Atlas::AtlasFilters
        varconf::varconf
        libsigcpp::sigc++
        remotery
        Boost::filesystem
        readline::readline
        Detour
        libxdg-basedir::libxdg-basedir
        libgcrypt::libgcrypt
)

link_directories(
        ${BULLET_LIBRARY_DIR}
)

include_directories(
        ${BULLET_INCLUDE_DIR}
)

#On platforms which supports it, link the "libSegFault" library which provides nice stack traces
find_library(SEGFAULT_LIBRARY SegFault)
if (SEGFAULT_LIBRARY)
    message(STATUS "Adding the SegFault library for nice stack traces for tests.")
    link_libraries(${SEGFAULT_LIBRARY})
endif (SEGFAULT_LIBRARY)

add_library(stubnavigation
        stubs/navigation/libstub.cpp
        ../src/navigation/fastlz.c)

set(PYTHON_TESTS_LIBS
        rulesai_python
        rulessimulation_python
        entityfilter_python
        rulesbase_python
        physics_python
        atlas_python
        common_python
        pythonbase
        rulesai
        rulessimulation
        entityfilter
        stubnavigation
        rulesbase
        modules
        physics
        common
        remotery)


#Macro for adding a test
macro(wf_add_test TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ../src/common/debug.cpp TestWorld.cpp ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    target_compile_options(${TEST_NAME} PUBLIC "-w")
    add_dependencies(check ${TEST_NAME})

    #If Python has been built statically (as with Conan) we need to enable exports so the dynamic loading works.
    set_property(TARGET ${TEST_NAME} PROPERTY ENABLE_EXPORTS 1)
endmacro()

macro(wf_add_benchmark TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ../src/common/debug.cpp TestWorld.cpp ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(benchmark ${TEST_NAME})

    #If Python has been built statically (as with Conan) we need to enable exports so the dynamic loading works.
    set_property(TARGET ${TEST_NAME} PROPERTY ENABLE_EXPORTS 1)

endmacro()

add_custom_target(stubs)
wf_generate_stubs(common)
wf_generate_stubs(modules)
wf_generate_stubs(navigation)
wf_generate_stubs(physics)
wf_generate_stubs(rules)
wf_generate_stubs(server)
wf_generate_stubs(client)
wf_generate_stubs(pythonbase)

#### Common tests #####
wf_add_test(TestBaseTest.cpp)
wf_add_test(modules/RefTest.cpp)

wf_add_test(common/OperationsDispatcherTest.cpp)
target_link_libraries(OperationsDispatcherTest modules common)

wf_add_test(common/logTest.cpp ../src/common/log.cpp)
wf_add_test(common/InheritanceTest.cpp ../src/common/Inheritance.cpp ../src/common/custom.cpp)
wf_add_test(common/PropertyTest.cpp ../src/common/Property.cpp)
wf_add_test(common/systemTest.cpp ../src/common/system.cpp)
wf_add_test(common/system_netTest.cpp ../src/common/system_net.cpp)
wf_add_test(common/system_uidTest.cpp ../src/common/system_uid.cpp)
wf_add_test(common/type_utilsTest.cpp ../src/common/type_utils.cpp)
wf_add_test(common/constTest.cpp ../src/common/const.cpp)
wf_add_test(common/randomTest.cpp)
wf_add_test(common/ConnectTest.cpp)
wf_add_test(common/MonitorTest.cpp)
wf_add_test(common/SetupTest.cpp)
wf_add_test(common/TickTest.cpp)
wf_add_test(common/UpdateTest.cpp)
wf_add_test(common/AtlasFileLoaderTest.cpp ../src/common/AtlasFileLoader.cpp)
wf_add_test(server/BaseWorldTest.cpp ../src/rules/simulation/BaseWorld.cpp)
wf_add_test(common/idTest.cpp ../src/common/id.cpp)
wf_add_test(common/StorageTest.cpp ../src/common/Storage.cpp)
wf_add_test(common/debugTest.cpp ../src/common/debug.cpp)
wf_add_test(common/globalsTest.cpp ../src/common/globals.cpp)
target_compile_definitions(globalsTest PUBLIC -DBINDIR="${CMAKE_INSTALL_FULL_BINDIR}" -DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}" -DSYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}" -DLOCALSTATEDIR="${CMAKE_INSTALL_FULL_LOCALSTATEDIR}")
wf_add_test(common/OperationRouterTest.cpp)
wf_add_test(common/RouterTest.cpp ../src/common/Router.cpp)
wf_add_test(common/client_socketTest.cpp ../src/common/client_socket.cpp)
wf_add_test(common/customTest.cpp ../src/common/custom.cpp)
wf_add_test(common/MonitorsTest.cpp ../src/common/Monitors.cpp ../src/common/Variable.cpp)
wf_add_test(common/newidTest.cpp ../src/common/newid.cpp)
wf_add_test(common/TypeNodeTest.cpp ../src/common/TypeNode.cpp ../src/common/Property.cpp)
wf_add_test(common/FormattedXMLWriterTest.cpp ../src/common/FormattedXMLWriter.cpp)
wf_add_test(common/PropertyFactoryTest.cpp ../src/common/Property.cpp)
wf_add_test(common/PropertyManagerTest.cpp ../src/common/PropertyManager.cpp)
wf_add_test(common/VariableTest.cpp ../src/common/Variable.cpp)
wf_add_test(common/AtlasStreamClientTest.cpp ../src/common/AtlasStreamClient.cpp)
wf_add_test(common/ClientTaskTest.cpp ../src/common/ClientTask.cpp)
wf_add_test(common/utilsTest.cpp ../src/common/utils.cpp)
wf_add_test(common/ShakerTest.cpp ../src/common/Shaker.cpp)
wf_add_test(common/ScriptKitTest.cpp)
wf_add_test(rules/EntityKitTest.cpp)
wf_add_test(common/LinkTest.cpp ../src/common/Link.cpp)
wf_add_test(common/CommSocketTest.cpp)
wf_add_test(common/composeTest.cpp)
wf_add_test(common/FileSystemObserverIntegrationTest.cpp ../src/common/FileSystemObserver.cpp)
target_link_libraries(FileSystemObserverIntegrationTest common)

# PHYSICS_TESTS
wf_add_test(physics/BBoxTest.cpp ../src/physics/BBox.cpp ../src/common/const.cpp)
wf_add_test(physics/Vector3DTest.cpp ../src/physics/Vector3D.cpp)
wf_add_test(physics/QuaternionTest.cpp ../src/physics/Quaternion.cpp)
wf_add_test(physics/transformTest.cpp)
wf_add_test(physics/distanceTest.cpp ../src/physics/Vector3D ../src/rules/Location ../src/rules/EntityLocation)
wf_add_test(physics/ShapeTest.cpp ../src/physics/Shape.cpp ../src/physics/Course.cpp)
wf_add_test(physics/CourseTest.cpp ../src/physics/Course.cpp)

# MODULE_TESTS

wf_add_test(modules/DateTimeTest.cpp ../src/modules/DateTime.cpp)
wf_add_test(rules/LocationTest.cpp ../src/rules/Location.cpp ../src/rules/EntityLocation.cpp)
target_link_libraries(LocationTest physics)
wf_add_test(rules/EntityRefTest.cpp ../src/modules/WeakEntityRef.cpp)
wf_add_test(modules/TerrainContextTest.cpp)

# RULESETS_TESTS

set(ENTITYEXERCISE TestPropertyManager.cpp IGEntityExerciser.cpp common/EntityExerciser.cpp)

wf_add_test(rules/OgreMeshDeserializerTest.cpp ../src/rules/simulation/OgreMeshDeserializer.cpp)
wf_add_test(rules/ModifierTest.cpp ../src/rules/Modifier.cpp)
wf_add_test(rules/LocatedEntityTest.cpp common/EntityExerciser.cpp ../src/rules/LocatedEntity.cpp ../src/rules/AtlasProperties TestPropertyManager.cpp)
wf_add_test(rules/EntityTest.cpp ${ENTITYEXERCISE} ../src/rules/simulation/Entity.cpp ../src/rules/LocatedEntity.cpp)
wf_add_test(rules/ThingTest.cpp ${ENTITYEXERCISE} ../src/rules/simulation/Thing.cpp)
wf_add_test(rules/WorldTest.cpp ${ENTITYEXERCISE} ../src/rules/simulation/World.cpp)
wf_add_test(rules/ThingupdatePropertiesTest.cpp ../src/rules/simulation/Thing.cpp ../src/common/Property.cpp)
wf_add_test(rules/TaskTest.cpp ../src/rules/simulation/Task.cpp)
target_link_libraries(TaskTest ${PYTHON_TESTS_LIBS})
wf_add_test(rules/simulation/EntityPropertyTest.cpp ../src/rules/simulation/EntityProperty.cpp ../src/modules/WeakEntityRef.cpp ../src/common/Property.cpp)
wf_add_test(rules/simulation/AllPropertyTest.cpp PropertyExerciser.cpp ../src/rules/simulation/AreaProperty.cpp
        ../src/rules/AtlasProperties.cpp
        ../src/rules/simulation/CalendarProperty.cpp
        ../src/rules/simulation/EntityProperty.cpp
        ../src/rules/simulation/LineProperty.cpp
        ../src/rules/simulation/TerrainProperty.cpp
        ../src/rules/simulation/TerrainEffectorProperty.cpp
        ../src/modules/WeakEntityRef.cpp
        ../src/modules/DateTime.cpp
        ../src/common/Property.cpp
        ../src/common/Inheritance.cpp
        ../src/common/custom.cpp
        ../src/common/operations.cpp
        ../src/physics/Shape.cpp
        ../src/physics/Course.cpp)
wf_add_test(rules/ScriptTest.cpp ../src/rules/Script.cpp)
wf_add_test(rules/simulation/AreaPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/AreaProperty.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/BBoxPropertyTest.cpp PropertyCoverage.cpp ../src/rules/BBoxProperty.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/simulation/CalendarPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/CalendarProperty.cpp
        ../src/modules/DateTime.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/simulation/LinePropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/LineProperty.cpp
        ../src/common/Property.cpp)
wf_add_test(server/MindPropertyTest.cpp PropertyCoverage.cpp ../src/server/MindProperty.cpp
        ../src/common/Property.cpp)
target_link_libraries(MindPropertyTest pycxx)

wf_add_test(rules/ModePropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/ModeProperty.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/SolidPropertyTest.cpp PropertyCoverage.cpp ../src/rules/SolidProperty.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/StatusPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/StatusProperty.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/SuspendedPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/SuspendedProperty.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/TerrainModPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TerrainModProperty.cpp
        ../src/rules/simulation/TerrainProperty.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/PythonClassTest.cpp python_testers.cpp ../src/pythonbase/PythonClass.cpp ../src/pythonbase/PythonMalloc.cpp)
target_link_libraries(PythonClassTest pycxx)
wf_add_test(rules/TerrainPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TerrainProperty.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/TransientPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TransientProperty.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/TasksPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TasksProperty.cpp
        ../src/common/Property.cpp)
target_link_libraries(TasksPropertyTest ${PYTHON_TESTS_LIBS})
wf_add_test(common/EntityPropertiesTest.cpp PropertyCoverage.cpp ../src/rules/EntityProperties.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/AtlasPropertiesTest.cpp PropertyCoverage.cpp ../src/rules/AtlasProperties.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/SpawnPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/SpawnProperty.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/VisibilityPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/VisibilityProperty.cpp
        ../src/common/Property.cpp)

wf_add_test(rules/ai/BaseMindTest.cpp ../src/rules/ai/BaseMind.cpp ../src/rules/ai/MemMap.cpp)
wf_add_test(rules/MemEntityTest.cpp ../src/rules/ai/MemEntity.cpp)
wf_add_test(rules/ai/MemMapTest.cpp ../src/rules/ai/MemMap.cpp ../src/rules/ai/MemEntity.cpp)
wf_add_test(rules/MovementTest.cpp ../src/rules/simulation/Movement.cpp)
wf_add_test(server/ExternalMindTest.cpp ../src/rules/simulation/ExternalMind.cpp)
wf_add_test(rules/PythonContextTest.cpp ../src/pythonbase/PythonContext.cpp)
target_link_libraries(PythonContextTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/TerrainModTest.cpp ../src/rules/simulation/TerrainModTranslator.cpp)
wf_add_test(rules/TerrainEffectorPropertyTest.cpp ../src/rules/simulation/TerrainEffectorProperty.cpp)
wf_add_test(rules/simulation/GeometryPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/GeometryProperty.cpp
        ../src/common/Property.cpp)

#Python ruleset tests

wf_add_test(rules/Python_APITest.cpp python_testers.cpp)
target_link_libraries(Python_APITest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_QuaternionTest.cpp python_testers.cpp)
target_link_libraries(Py_QuaternionTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_Vector3DTest.cpp python_testers.cpp)
target_link_libraries(Py_Vector3DTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_Point3DTest.cpp python_testers.cpp)
target_link_libraries(Py_Point3DTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_BBoxTest.cpp python_testers.cpp)
target_link_libraries(Py_BBoxTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_LocationTest.cpp python_testers.cpp)
target_link_libraries(Py_LocationTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_RootEntityTest.cpp python_testers.cpp)
target_link_libraries(Py_RootEntityTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_OperationTest.cpp python_testers.cpp)
target_link_libraries(Py_OperationTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_OplistTest.cpp python_testers.cpp)
target_link_libraries(Py_OplistTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_ThingTest.cpp python_testers.cpp TestPropertyManager.cpp)
target_link_libraries(Py_ThingTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/ai/Py_MapTest.cpp python_testers.cpp TestPropertyManager.cpp)
target_link_libraries(Py_MapTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_TaskTest.cpp python_testers.cpp)
target_link_libraries(Py_TaskTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_WorldTest.cpp python_testers.cpp)
target_link_libraries(Py_WorldTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_MessageTest.cpp python_testers.cpp)
target_link_libraries(Py_MessageTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_PropertyTest.cpp python_testers.cpp)
target_link_libraries(Py_PropertyTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_TerrainPropertyTest.cpp python_testers.cpp)
target_link_libraries(Py_TerrainPropertyTest ${PYTHON_TESTS_LIBS})

#wf_add_test(Py_TerrainModPropertyTest.cpp python_testers.cpp)
#target_link_libraries(Py_TerrainModPropertyTest ${PYTHON_TESTS_LIBS})

#wf_add_test(Py_ShapeTest.cpp python_testers.cpp)
#target_link_libraries(Py_ShapeTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/Py_FilterTest.cpp python_testers.cpp)
target_link_libraries(Py_FilterTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/PythonWrapperTest.cpp python_testers.cpp)
target_link_libraries(PythonWrapperTest ${PYTHON_TESTS_LIBS})

#Entity filter tests

wf_add_test(rules/entityfilter/EntityFilterTest.cpp
        ../src/rules/simulation/EntityProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/BBoxProperty.cpp
        ../src/rules/LocatedEntity.cpp
        ../src/rules/simulation/ModeDataProperty.cpp
        ../src/modules/WeakEntityRef.cpp
        ../src/common/Property.cpp
        ../src/common/TypeNode.cpp
        ../src/common/PropertyManager.cpp)
target_link_libraries(EntityFilterTest entityfilter)
target_compile_definitions(EntityFilterTest PRIVATE -DBOOST_SPIRIT_DEBUG)

wf_add_test(rules/entityfilter/EntityFilterParserTest.cpp
        ../src/rules/simulation/EntityProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/BBoxProperty.cpp
        ../src/rules/LocatedEntity.cpp
        ../src/rules/PhysicalProperties.cpp
        ../src/rules/Point3Property.cpp
        ../src/rules/Vector3Property.cpp
        ../src/rules/QuaternionProperty.cpp
        ../src/modules/WeakEntityRef.cpp
        ../src/common/Property.cpp
        ../src/common/TypeNode.cpp
        ../src/common/PropertyManager.cpp)
target_link_libraries(EntityFilterParserTest entityfilter)

wf_add_test(rules/entityfilter/EntityFilterProvidersTest.cpp ../src/rules/simulation/EntityProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/BBoxProperty.cpp
        ../src/rules/LocatedEntity.cpp
        ../src/rules/simulation/ModeDataProperty.cpp
        ../src/modules/WeakEntityRef.cpp
        ../src/common/Property.cpp
        ../src/common/TypeNode.cpp
        ../src/common/PropertyManager.cpp)
target_link_libraries(EntityFilterProvidersTest entityfilter)


# RULESETS_INTEGRATION

wf_add_test(rules/simulation/GeometryPropertyIntegration.cpp ../src/rules/simulation/GeometryProperty.cpp)
target_link_libraries(GeometryPropertyIntegration ${PYTHON_TESTS_LIBS})

wf_add_benchmark(server/PhysicalDomainBenchmark.cpp ../src/rules/simulation/PhysicalDomain.cpp)
target_link_libraries(PhysicalDomainBenchmark ${PYTHON_TESTS_LIBS})

wf_add_test(server/PhysicalDomainIntegrationTest.cpp ../src/rules/simulation/PhysicalDomain.cpp)
target_link_libraries(PhysicalDomainIntegrationTest ${PYTHON_TESTS_LIBS})

wf_add_test(rules/PropertyEntityIntegration.cpp ../src/rules/simulation/Entity.cpp
        ../src/rules/LocatedEntity.cpp
        ../src/common/Property.cpp
        ../src/common/TypeNode.cpp
        ../src/rules/Modifier.cpp)
wf_add_test(rules/TerrainModPropertyIntegration.cpp ../src/rules/simulation/Entity.cpp
        ../src/rules/LocatedEntity.cpp
        ../src/rules/PhysicalProperties.cpp
        ../src/rules/Point3Property.cpp
        ../src/rules/Vector3Property.cpp
        ../src/rules/QuaternionProperty.cpp
        ../src/rules/simulation/TerrainEffectorProperty.cpp
        ../src/rules/simulation/TerrainModProperty.cpp
        ../src/rules/simulation/TerrainModTranslator.cpp
        ../src/rules/simulation/TerrainProperty.cpp
        ../src/modules/WeakEntityRef.cpp
        ../src/common/Property.cpp
        ../src/common/TypeNode.cpp)
wf_add_test(rules/simulation/ContainedVisibilityPropertyIntegration.cpp ../src/rules/simulation/ContainedVisibilityProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/LocatedEntity.cpp)
wf_add_test(rules/ai/BaseMindMapEntityIntegration.cpp
        ../src/rules/ai/BaseMind.cpp
        ../src/rules/ai/MemEntity.cpp
        ../src/rules/LocatedEntity.cpp
        ../src/rules/ai/MemMap.cpp
        ../src/rules/BBoxProperty.cpp
        ../src/rules/SolidProperty.cpp
        ../src/rules/Vector3Property.cpp
        ../src/rules/Point3Property.cpp
        ../src/rules/QuaternionProperty.cpp
        ../src/rules/PhysicalProperties.cpp
        ../src/rules/Location.cpp
        ../src/rules/EntityLocation.cpp
        ../src/common/Property.cpp
        ../src/client/ClientPropertyManager.cpp)
wf_add_test(rules/simulation/AreaPropertyIntegration.cpp ../src/rules/simulation/AreaProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/LocatedEntity.cpp
        ../src/physics/Course.cpp
        ../src/physics/Shape.cpp)
wf_add_test(rules/SuspendedPropertyIntegration.cpp ../src/rules/simulation/SuspendedProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/LocatedEntity.cpp)
target_link_libraries(SuspendedPropertyIntegration common)
wf_add_test(rules/ThingIntegration.cpp)
target_link_libraries(ThingIntegration rulessimulation rulesbase modules common physics ${PYTHON_TESTS_LIBS})
wf_add_test(rules/RulesIntegrationTest.cpp)
target_link_libraries(RulesIntegrationTest rulessimulation rulesbase modules common physics ${PYTHON_TESTS_LIBS})
wf_add_test(rules/ObserverIntegrationTest.cpp)
target_link_libraries(ObserverIntegrationTest rulessimulation rulesbase modules common physics ${PYTHON_TESTS_LIBS})
wf_add_test(rules/TerrainPropertyIntegration.cpp)
target_link_libraries(TerrainPropertyIntegration rulessimulation rulesbase modules common physics ${PYTHON_TESTS_LIBS})


# CLIENT_TESTS
wf_add_test(client/Py_CreatorClientTest.cpp python_testers.cpp ../src/client/cyclient/CyPy_CreatorClient.cpp
        ../src/client/cyclient/CyPy_ObserverClient.cpp
        ../src/client/cyclient/Python_ClientAPI.cpp)
target_link_libraries(Py_CreatorClientTest ${PYTHON_TESTS_LIBS})

wf_add_test(client/Py_ObserverClientTest.cpp python_testers.cpp ../src/client/cyclient/CyPy_CreatorClient.cpp
        ../src/client/cyclient/CyPy_ObserverClient.cpp
        ../src/client/cyclient/Python_ClientAPI.cpp)
target_link_libraries(Py_ObserverClientTest ${PYTHON_TESTS_LIBS})

wf_add_test(client/ClientConnectionTest.cpp ../src/client/cyclient/ClientConnection.cpp)
wf_add_test(client/BaseClientTest.cpp ../src/client/cyclient/BaseClient.cpp)
wf_add_test(client/ClientPropertyManagerTest.cpp ../src/client/ClientPropertyManager.cpp
        ../src/common/PropertyManager.cpp)


# CLIENT_INTEGRATION_TESTS
wf_add_test(client/ClientConnectionIntegration.cpp ../src/client/cyclient/ClientConnection.cpp
        ../src/common/AtlasStreamClient.cpp)

# SERVER_TESTS

wf_add_test(server/ArchetypeFactoryIntegrationTest.cpp
        ../src/server/ArchetypeFactory.cpp
        ../src/server/ArchetypeRuleHandler.cpp
        ../src/server/EntityBuilder.cpp
        ../src/server/EntityFactory.cpp
        ../src/server/EntityRuleHandler.cpp
        ../src/rules/simulation/CorePropertyManager.cpp)
target_link_libraries(ArchetypeFactoryIntegrationTest ${PYTHON_TESTS_LIBS})

wf_add_test(server/RulesetTest.cpp ../src/server/Ruleset.cpp)
wf_add_test(server/EntityBuilderTest.cpp ../src/server/EntityBuilder.cpp)
wf_add_test(common/PropertyFlagTest.cpp TestPropertyManager.cpp ../src/rules/simulation/Thing.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/LocatedEntity.cpp
        ../src/common/Property.cpp)
wf_add_test(server/AccountTest.cpp ../src/server/Account.cpp
        ../src/common/debug.cpp)
wf_add_test(server/AdminTest.cpp ../src/server/Admin.cpp
        ../src/common/debug.cpp)
wf_add_test(server/ServerAccountTest.cpp ../src/server/ServerAccount.cpp)
wf_add_test(server/PlayerTest.cpp ../src/server/Player.cpp
        ../src/common/debug.cpp)
wf_add_test(server/EntityFactoryTest.cpp ../src/server/EntityFactory.cpp
        ../src/rules/Modifier.cpp)
wf_add_test(common/buildidTest.cpp ${PROJECT_BINARY_DIR}/src/server/buildid.cpp)
wf_add_test(server/ConnectionTest.cpp ../src/server/Connection.cpp)
wf_add_test(server/TrustedConnectionTest.cpp ../src/server/TrustedConnection.cpp)
wf_add_test(server/WorldRouterTest.cpp ../src/rules/simulation/WorldRouter.cpp)
wf_add_test(server/PeerTest.cpp ../src/server/Peer.cpp)
wf_add_test(server/LobbyTest.cpp ../src/server/Lobby.cpp)


wf_add_test(server/ServerRoutingTest.cpp ../src/server/ServerRouting.cpp)
wf_add_test(server/StorageManagerTest.cpp ../src/server/StorageManager.cpp)
wf_add_test(server/HttpHandlingTest.cpp ../src/common/net/HttpHandling.cpp)

# SERVER_COMM_TESTS
wf_add_test(server/CommPeerTest.cpp ../src/server/CommPeer.cpp)
wf_add_test(server/CommMDNSPublisherTest.cpp ../src/server/CommMDNSPublisher.cpp)
wf_add_test(server/TeleportAuthenticatorTest.cpp ../src/server/PossessionAuthenticator.cpp)
wf_add_test(server/TeleportStateTest.cpp ../src/server/TeleportState.cpp)
wf_add_test(server/PendingTeleportTest.cpp ../src/server/PendingPossession.cpp)
wf_add_test(server/JunctureTest.cpp ../src/server/Juncture.cpp)
wf_add_test(server/ConnectableRouterTest.cpp)
wf_add_test(server/OpRuleHandlerTest.cpp ../src/server/OpRuleHandler.cpp)
wf_add_test(server/EntityRuleHandlerTest.cpp ../src/server/EntityRuleHandler.cpp)

wf_add_test(server/PropertyRuleHandlerTest.cpp ../src/server/PropertyRuleHandler.cpp)
wf_add_test(server/IdleConnectorTest.cpp ../src/server/IdleConnector.cpp)
wf_add_test(server/PersistenceTest.cpp ../src/server/Persistence.cpp)
wf_add_test(server/SystemAccountTest.cpp ../src/server/SystemAccount.cpp)
wf_add_test(server/CorePropertyManagerTest.cpp ../src/rules/simulation/CorePropertyManager.cpp)

# SERVER_INTEGRATION_TESTS

wf_add_test(server/WorldRouterIntegration.cpp ../src/rules/simulation/WorldRouter.cpp
        ../src/server/EntityBuilder.cpp
        ../src/server/EntityFactory.cpp
        ../src/server/EntityRuleHandler.cpp
        ../src/rules/Domain.cpp
        ../src/rules/LocatedEntity.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/simulation/Thing.cpp
        ../src/rules/simulation/World.cpp
        ../src/rules/simulation/BaseWorld.cpp
        ../src/rules/Modifier.cpp)
target_link_libraries(WorldRouterIntegration modules physics common pycxx)
wf_add_test(server/RulesetIntegration.cpp ../src/server/Ruleset.cpp
        ../src/server/EntityBuilder.cpp
        ../src/server/EntityFactory.cpp
        ../src/server/OpRuleHandler.cpp
        ../src/server/EntityRuleHandler.cpp
        ../src/server/ArchetypeRuleHandler.cpp
        ../src/server/PropertyRuleHandler.cpp
        ../src/server/Persistence.cpp
        ../src/rules/simulation/BaseWorld.cpp
        ../src/rules/Modifier.cpp)
target_link_libraries(RulesetIntegration common pycxx)
#Rulesetintegration_LDFLAGS = $(PYTHON_LINKER_FLAGS)
wf_add_test(server/ConnectionShakerIntegration.cpp ../src/server/Connection.cpp
        ../src/common/Shaker.cpp)
wf_add_test(server/ConnectionCreatorIntegration.cpp
        ../src/server/Connection.cpp)
target_link_libraries(ConnectionCreatorIntegration ${PYTHON_TESTS_LIBS})
wf_add_test(server/TrustedConnectionCreatorIntegration.cpp
        ../src/server/Connection.cpp)
target_link_libraries(TrustedConnectionCreatorIntegration ${PYTHON_TESTS_LIBS})
wf_add_test(server/AccountConnectionCharacterIntegration.cpp
        ../src/server/Account.cpp
        ../src/server/Connection.cpp)
target_link_libraries(AccountConnectionCharacterIntegration ${PYTHON_TESTS_LIBS})
wf_add_test(server/AccountIntegration.cpp)
target_link_libraries(AccountIntegration server ${PYTHON_TESTS_LIBS})

wf_add_test(server/EntityFactoryTypeNodeIntegration.cpp TestPropertyManager.cpp ../src/server/EntityFactory.cpp
        ../src/common/TypeNode.cpp
        ../src/common/PropertyManager.cpp
        ../src/common/Property.cpp
        ../src/rules/Modifier.cpp)
wf_add_test(server/AccountConnectionIntegration.cpp ../src/server/Account.cpp
        ../src/server/Admin.cpp
        ../src/server/Connection.cpp
        ../src/server/Lobby.cpp
        ../src/server/Player.cpp
        ../src/server/ServerAccount.cpp
        ../src/server/ServerRouting.cpp
        ../src/server/SystemAccount.cpp
        ../src/rules/simulation/BaseWorld.cpp
        ../src/common/Inheritance.cpp
        ../src/common/Property.cpp
        ../src/common/Router.cpp
        ../src/common/TypeNode.cpp
        ../src/common/custom.cpp
        ../src/common/operations.cpp)
target_link_libraries(AccountConnectionIntegration physics)
wf_add_test(server/AccountServerLobbyIntegration.cpp ../src/server/Account.cpp
        ../src/server/ServerRouting.cpp
        ../src/server/Lobby.cpp)


# Other TESTS
#wf_add_test(MasterTest.cpp ${PROJECT_SOURCE_DIR}/server/Master.cpp)
#target_link_libraries(MasterTest common)


# TOOLS_TESTS
wf_add_test(tools/AdminClientTest.cpp ../src/tools/AdminClient.cpp)
wf_add_test(tools/FlusherTest.cpp ../src/tools/Flusher.cpp
        ../src/common/ClientTask.cpp)
wf_add_test(tools/OperationMonitorTest.cpp ../src/tools/OperationMonitor.cpp
        ../src/common/ClientTask.cpp)
wf_add_test(tools/EntityExporterTest.cpp ../src/tools/EntityExporterBase.cpp)
target_link_libraries(EntityExporterTest common)


# PYTHON_TESTS
#wf_add_test(python_class.cpp)
#target_link_libraries(python_class scriptpython rulessimulation rulesetmind rulesbase modules physics common)

wf_add_test(navigation/SteeringIntegration.cpp)
target_link_libraries(SteeringIntegration
        navigation
        rulesai
        rulesbase
        modules
        common
        physics
        DetourTileCache
        Detour
        Recast)
