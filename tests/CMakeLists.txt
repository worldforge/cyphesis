add_definitions(-DTESTDATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

add_library(stubnavigation stubs/navigation/libstub.cpp DatabaseNull.h)
target_link_libraries(stubnavigation PUBLIC Detour)

link_libraries(${PYTHON_LIBRARIES})
link_libraries(${GCRYPT_LIBRARIES})
link_libraries(${BULLET_LIBRARIES})
if (AVAHI_FOUND)
    link_libraries(${AVAHI_LIBRARIES} -lavahi-common)
endif (AVAHI_FOUND)
link_libraries(${PostgreSQL_LIBRARIES})
link_libraries(${READLINE_LIBRARY})

#On platforms which supports it, link the "libSegFault" library which provides nice stack traces
find_library(SEGFAULT_LIBRARY SegFault)
if (SEGFAULT_LIBRARY)
    link_libraries(${SEGFAULT_LIBRARY})
endif (SEGFAULT_LIBRARY)

set(PYTHON_TESTS_LIBS
    rulesai_python
    rulessimulation_python
    entityfilter_python
    rulesbase_python
    rulesai
    rulessimulation
    entityfilter
    rulesbase
    stubnavigation
    modules
    physics
    common)


#Macro for adding a test
macro(wf_add_test TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ../src/common/debug.cpp TestWorld.cpp ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(check ${TEST_NAME})
    target_link_libraries(${TEST_NAME} pycxx ${PYTHON_LIBRARIES})

endmacro()

macro(wf_add_benchmark TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ../src/common/debug.cpp TestWorld.cpp ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(benchmark ${TEST_NAME})
    target_link_libraries(${TEST_NAME} pycxx ${PYTHON_LIBRARIES})

endmacro()

add_custom_target(stubs)
wf_generate_stubs(common)
wf_generate_stubs(modules)
wf_generate_stubs(navigation)
wf_generate_stubs(physics)
wf_generate_stubs(rules)
wf_generate_stubs(server)
wf_generate_stubs(client)

#### Common tests #####
wf_add_test(TestBaseTest.cpp)
wf_add_test(RefTest.cpp)
wf_add_test(OgreMeshDeserializerTest.cpp ../src/rules/simulation/OgreMeshDeserializer.cpp)
wf_add_test(logTest.cpp ../src/common/log.cpp)
wf_add_test(InheritanceTest.cpp ../src/common/Inheritance.cpp ../src/common/custom.cpp)
wf_add_test(PropertyTest.cpp ../src/common/Property.cpp)
wf_add_test(systemTest.cpp ../src/common/system.cpp)
wf_add_test(system_netTest.cpp ../src/common/system_net.cpp)
wf_add_test(system_uidTest.cpp ../src/common/system_uid.cpp)
wf_add_test(type_utilsTest.cpp ../src/common/type_utils.cpp)
wf_add_test(constTest.cpp ../src/common/const.cpp)
wf_add_test(randomTest.cpp)
wf_add_test(AddTest.cpp OperationExerciser.cpp)
wf_add_test(ConnectTest.cpp)
wf_add_test(DropTest.cpp)
wf_add_test(MonitorTest.cpp)
wf_add_test(PickupTest.cpp)
wf_add_test(SetupTest.cpp)
wf_add_test(TickTest.cpp)
wf_add_test(UpdateTest.cpp)
wf_add_test(AtlasFileLoaderTest.cpp ../src/common/AtlasFileLoader.cpp)
wf_add_test(BaseWorldTest.cpp ../src/rules/simulation/BaseWorld.cpp)
wf_add_test(idTest.cpp ../src/common/id.cpp)
wf_add_test(StorageTest.cpp ../src/common/Storage.cpp)
wf_add_test(debugTest.cpp ../src/common/debug.cpp)
wf_add_test(globalsTest.cpp ../src/common/globals.cpp)
target_compile_definitions(globalsTest PUBLIC -DBINDIR="${CMAKE_INSTALL_FULL_BINDIR}" -DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}" -DSYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}" -DLOCALSTATEDIR="${CMAKE_INSTALL_FULL_LOCALSTATEDIR}")
wf_add_test(OperationRouterTest.cpp)
wf_add_test(RouterTest.cpp ../src/common/Router.cpp)
wf_add_test(client_socketTest.cpp ../src/common/client_socket.cpp)
wf_add_test(customTest.cpp ../src/common/custom.cpp)
wf_add_test(MonitorsTest.cpp ../src/common/Monitors.cpp ../src/common/Variable.cpp)
wf_add_test(operationsTest.cpp ../src/common/const.cpp)
wf_add_test(serialnoTest.cpp)
wf_add_test(newidTest.cpp ../src/common/newid.cpp)
wf_add_test(TypeNodeTest.cpp ../src/common/TypeNode.cpp ../src/common/Property.cpp)
wf_add_test(FormattedXMLWriterTest.cpp ../src/common/FormattedXMLWriter.cpp)
wf_add_test(PropertyFactoryTest.cpp ../src/common/Property.cpp)
wf_add_test(PropertyManagerTest.cpp ../src/common/PropertyManager.cpp)
wf_add_test(VariableTest.cpp ../src/common/Variable.cpp)
wf_add_test(AtlasStreamClientTest.cpp ../src/common/AtlasStreamClient.cpp)
wf_add_test(ClientTaskTest.cpp ../src/common/ClientTask.cpp)
wf_add_test(utilsTest.cpp ../src/common/utils.cpp)
wf_add_test(SystemTimeTest.cpp)
wf_add_test(ShakerTest.cpp ../src/common/Shaker.cpp)
wf_add_test(ScriptKitTest.cpp)
wf_add_test(EntityKitTest.cpp ../src/common/EntityKit.cpp)
wf_add_test(LinkTest.cpp ../src/common/Link.cpp)
wf_add_test(CommSocketTest.cpp)
wf_add_test(composeTest.cpp)
wf_add_test(FileSystemObserverIntegrationTest.cpp ../src/common/FileSystemObserver.cpp)
target_link_libraries(FileSystemObserverIntegrationTest common)

# PHYSICS_TESTS
wf_add_test(BBoxTest.cpp ../src/physics/BBox.cpp ../src/common/const.cpp)
wf_add_test(Vector3DTest.cpp ../src/physics/Vector3D.cpp)
wf_add_test(QuaternionTest.cpp ../src/physics/Quaternion.cpp)
wf_add_test(transformTest.cpp)
wf_add_test(distanceTest.cpp ../src/physics/Vector3D ../src/rules/Location ../src/rules/EntityLocation)
wf_add_test(ShapeTest.cpp ../src/physics/Shape.cpp ../src/physics/Course.cpp)
wf_add_test(CourseTest.cpp ../src/physics/Course.cpp)

# MODULE_TESTS

wf_add_test(DateTimeTest.cpp ../src/modules/DateTime.cpp)
wf_add_test(LocationTest.cpp ../src/rules/Location.cpp ../src/rules/EntityLocation.cpp)
target_link_libraries(LocationTest physics)
wf_add_test(WorldTimeTest.cpp ../src/modules/WorldTime.cpp ../src/modules/DateTime.cpp)
wf_add_test(EntityRefTest.cpp ../src/modules/WeakEntityRef.cpp)
wf_add_test(TerrainContextTest.cpp ../src/modules/TerrainContext.cpp)

# RULESETS_TESTS

set(ENTITYEXERCISE TestPropertyManager.cpp IGEntityExerciser.cpp EntityExerciser.cpp)

wf_add_test(LocatedEntityTest.cpp EntityExerciser.cpp ../src/rules/LocatedEntity.cpp TestPropertyManager.cpp)
wf_add_test(EntityTest.cpp ${ENTITYEXERCISE} ../src/rules/simulation/Entity.cpp ../src/rules/LocatedEntity.cpp)
wf_add_test(ThingTest.cpp ${ENTITYEXERCISE} ../src/rules/simulation/Thing.cpp)
wf_add_test(WorldTest.cpp ${ENTITYEXERCISE} ../src/rules/simulation/World.cpp)
wf_add_test(ThingupdatePropertiesTest.cpp ../src/rules/simulation/Thing.cpp ../src/common/Property.cpp)
wf_add_test(TaskTest.cpp ../src/rules/simulation/Task.cpp)
wf_add_test(EntityPropertyTest.cpp ../src/rules/simulation/EntityProperty.cpp ../src/modules/WeakEntityRef.cpp ../src/common/Property.cpp)
wf_add_test(AllPropertyTest.cpp PropertyExerciser.cpp ../src/rules/simulation/AreaProperty.cpp
    ../src/rules/AtlasProperties.cpp
    ../src/rules/simulation/CalendarProperty.cpp
    ../src/rules/simulation/EntityProperty.cpp
    ../src/rules/simulation/LineProperty.cpp
    ../src/rules/simulation/StatisticsProperty.cpp
    ../src/rules/simulation/TerrainProperty.cpp
    ../src/rules/simulation/TerrainEffectorProperty.cpp
    ../src/modules/WeakEntityRef.cpp
    ../src/modules/DateTime.cpp
    ../src/modules/TerrainContext.cpp
    ../src/common/Property.cpp
    ../src/common/Inheritance.cpp
    ../src/common/custom.cpp
    ../src/common/operations.cpp
    ../src/physics/Shape.cpp
    ../src/physics/Course.cpp)
wf_add_test(ScriptTest.cpp ../src/rules/Script.cpp)
wf_add_test(AreaPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/AreaProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(BBoxPropertyTest.cpp PropertyCoverage.cpp ../src/rules/BBoxProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(CalendarPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/CalendarProperty.cpp
    ../src/modules/DateTime.cpp
    ../src/common/Property.cpp)
wf_add_test(LinePropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/LineProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(MindPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/MindProperty.cpp
    ../src/common/Property.cpp)
target_link_libraries(MindPropertyTest pycxx)

wf_add_test(ModePropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/ModeProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(SolidPropertyTest.cpp PropertyCoverage.cpp ../src/rules/SolidProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(StatisticsPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/StatisticsProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(StatusPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/StatusProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(SuspendedPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/SuspendedProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(TerrainModPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TerrainModProperty.cpp
    ../src/rules/simulation/TerrainProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(PythonClassTest.cpp python_testers.cpp ../src/rules/python/PythonClass.cpp)
target_link_libraries(PythonClassTest pycxx)
wf_add_test(TerrainPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TerrainProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(TransientPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TransientProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(TasksPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TasksProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(InternalPropertiesTest.cpp PropertyCoverage.cpp ../src/rules/simulation/InternalProperties.cpp
    ../src/common/Property.cpp)
wf_add_test(EntityPropertiesTest.cpp PropertyCoverage.cpp ../src/rules/EntityProperties.cpp
    ../src/common/Property.cpp)
wf_add_test(AtlasPropertiesTest.cpp PropertyCoverage.cpp ../src/rules/AtlasProperties.cpp
    ../src/common/Property.cpp)
wf_add_test(SpawnPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/SpawnProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(SpawnerPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/SpawnerProperty.cpp
    ../src/common/Property.cpp)
wf_add_test(VisibilityPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/VisibilityProperty.cpp
    ../src/common/Property.cpp)

wf_add_test(BaseMindTest.cpp ../src/rules/ai/BaseMind.cpp ../src/rules/ai/MemMap.cpp)
wf_add_test(MemEntityTest.cpp ../src/rules/MemEntity.cpp)
wf_add_test(MemMapTest.cpp ../src/rules/ai/MemMap.cpp)
wf_add_test(MovementTest.cpp ../src/rules/simulation/Movement.cpp)
wf_add_test(PedestrianTest.cpp ../src/rules/simulation/Pedestrian.cpp ../src/rules/simulation/Movement.cpp)
wf_add_test(ExternalMindTest.cpp ../src/rules/simulation/ExternalMind.cpp)
wf_add_test(PythonContextTest.cpp ../src/rules/python/PythonContext.cpp)
wf_add_test(ArithmeticScriptTest.cpp)
wf_add_test(PythonArithmeticScriptTest.cpp python_testers.cpp ../src/rules/python/PythonArithmeticScript.cpp)
target_link_libraries(PythonArithmeticScriptTest pycxx)

wf_add_test(ArithmeticFactoryTest.cpp)
wf_add_test(PythonArithmeticFactoryTest.cpp python_testers.cpp ../src/rules/python/PythonArithmeticFactory.cpp
    ../src/rules/python/PythonClass.cpp)
target_link_libraries(PythonArithmeticFactoryTest pycxx)
wf_add_test(TerrainModTest.cpp ../src/rules/simulation/TerrainModTranslator.cpp)
wf_add_test(TerrainEffectorPropertyTest.cpp ../src/rules/simulation/TerrainEffectorProperty.cpp)
wf_add_test(GeometryPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/GeometryProperty.cpp
    ../src/common/Property.cpp)

#Python ruleset tests

wf_add_test(Python_APITest.cpp python_testers.cpp)
target_link_libraries(Python_APITest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_QuaternionTest.cpp python_testers.cpp)
target_link_libraries(Py_QuaternionTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_Vector3DTest.cpp python_testers.cpp)
target_link_libraries(Py_Vector3DTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_Point3DTest.cpp python_testers.cpp)
target_link_libraries(Py_Point3DTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_BBoxTest.cpp python_testers.cpp)
target_link_libraries(Py_BBoxTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_LocationTest.cpp python_testers.cpp)
target_link_libraries(Py_LocationTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_RootEntityTest.cpp python_testers.cpp)
target_link_libraries(Py_RootEntityTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_OperationTest.cpp python_testers.cpp)
target_link_libraries(Py_OperationTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_OplistTest.cpp python_testers.cpp)
target_link_libraries(Py_OplistTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_ThingTest.cpp python_testers.cpp TestPropertyManager.cpp)
target_link_libraries(Py_ThingTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_MapTest.cpp python_testers.cpp)
target_link_libraries(Py_MapTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_TaskTest.cpp python_testers.cpp)
target_link_libraries(Py_TaskTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_WorldTest.cpp python_testers.cpp)
target_link_libraries(Py_WorldTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_MessageTest.cpp python_testers.cpp)
target_link_libraries(Py_MessageTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_WorldTimeTest.cpp python_testers.cpp)
target_link_libraries(Py_WorldTimeTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_PropertyTest.cpp python_testers.cpp)
target_link_libraries(Py_PropertyTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_TerrainPropertyTest.cpp python_testers.cpp)
target_link_libraries(Py_TerrainPropertyTest ${PYTHON_TESTS_LIBS})

#wf_add_test(Py_TerrainModPropertyTest.cpp python_testers.cpp)
#target_link_libraries(Py_TerrainModPropertyTest ${PYTHON_TESTS_LIBS})

#wf_add_test(Py_ShapeTest.cpp python_testers.cpp)
#target_link_libraries(Py_ShapeTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_FilterTest.cpp python_testers.cpp)
target_link_libraries(Py_FilterTest ${PYTHON_TESTS_LIBS})

wf_add_test(PythonWrapperTest.cpp python_testers.cpp)
target_link_libraries(PythonWrapperTest ${PYTHON_TESTS_LIBS})

#Entity filter tests

wf_add_test(EntityFilterTest.cpp
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

wf_add_test(EntityFilterParserTest.cpp
    ../src/rules/simulation/EntityProperty.cpp
    ../src/rules/simulation/Entity.cpp
    ../src/rules/BBoxProperty.cpp
    ../src/rules/LocatedEntity.cpp
    ../src/modules/WeakEntityRef.cpp
    ../src/common/Property.cpp
    ../src/common/TypeNode.cpp
    ../src/common/PropertyManager.cpp)
target_link_libraries(EntityFilterParserTest entityfilter)

wf_add_test(EntityFilterProvidersTest.cpp ../src/rules/simulation/EntityProperty.cpp
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

wf_add_test(GeometryPropertyIntegration.cpp ../src/rules/simulation/GeometryProperty.cpp)
target_link_libraries(GeometryPropertyIntegration rulessimulation rulesbase physics modules common)

wf_add_benchmark(PhysicalDomainBenchmark.cpp ../src/rules/simulation/PhysicalDomain.cpp)
target_link_libraries(PhysicalDomainBenchmark rulessimulation rulesbase physics modules common)

wf_add_test(PhysicalDomainIntegrationTest.cpp ../src/rules/simulation/PhysicalDomain.cpp)
target_link_libraries(PhysicalDomainIntegrationTest rulessimulation rulesbase physics modules common)

wf_add_test(TerrainPropertyIntegration.cpp ../src/rules/simulation/TerrainModProperty.cpp
    ../src/rules/simulation/TerrainEffectorProperty.cpp
    ../src/rules/simulation/TerrainProperty.cpp
    ../src/rules/simulation/TerrainModTranslator.cpp
    ../src/rules/simulation/Entity.cpp
    ../src/rules/LocatedEntity.cpp
    ../src/rules/Domain.cpp
    ../src/rules/AtlasProperties.cpp
    ../src/rules/simulation/BaseWorld.cpp
    ../src/common/Property.cpp
    ../src/common/PropertyManager.cpp
    ../src/common/Router.cpp
    ../src/common/operations.cpp
    ../src/common/id.cpp)

target_link_libraries(TerrainPropertyIntegration modules physics)

wf_add_test(PropertyEntityIntegration.cpp ../src/rules/simulation/Entity.cpp
    ../src/rules/LocatedEntity.cpp
    ../src/common/Property.cpp
    ../src/common/TypeNode.cpp)
wf_add_test(TerrainModPropertyIntegration.cpp ../src/rules/simulation/Entity.cpp
    ../src/rules/LocatedEntity.cpp
    ../src/rules/simulation/TerrainEffectorProperty.cpp
    ../src/rules/simulation/TerrainModProperty.cpp
    ../src/rules/simulation/TerrainModTranslator.cpp
    ../src/rules/simulation/TerrainProperty.cpp
    ../src/modules/WeakEntityRef.cpp
    ../src/modules/TerrainContext.cpp
    ../src/common/Property.cpp
    ../src/common/TypeNode.cpp)
wf_add_test(StatisticsPropertyIntegration.cpp ../src/rules/simulation/StatisticsProperty.cpp
    ../src/rules/simulation/Entity.cpp
    ../src/rules/LocatedEntity.cpp)
wf_add_test(ContainedVisibilityPropertyIntegration.cpp ../src/rules/simulation/ContainedVisibilityProperty.cpp
    ../src/rules/simulation/Entity.cpp
    ../src/rules/LocatedEntity.cpp)
wf_add_test(BaseMindMapEntityIntegration.cpp
    ../src/rules/ai/BaseMind.cpp
        ../src/rules/MemEntity.cpp
        ../src/rules/LocatedEntity.cpp
    ../src/rules/ai/MemMap.cpp
    ../src/rules/BBoxProperty.cpp
    ../src/rules/SolidProperty.cpp
    ../src/rules/simulation/InternalProperties.cpp
    ../src/rules/Location.cpp
    ../src/rules/EntityLocation.cpp
    ../src/common/Property.cpp
    ../src/client/ClientPropertyManager.cpp)
wf_add_test(AreaPropertyIntegration.cpp ../src/rules/simulation/AreaProperty.cpp
    ../src/rules/simulation/Entity.cpp
    ../src/rules/LocatedEntity.cpp
    ../src/physics/Course.cpp
    ../src/physics/Shape.cpp)
wf_add_test(SuspendedPropertyIntegration.cpp ../src/rules/simulation/SuspendedProperty.cpp
    ../src/rules/simulation/Entity.cpp
    ../src/rules/LocatedEntity.cpp)
target_link_libraries(SuspendedPropertyIntegration common)
wf_add_test(ThingIntegration.cpp)
target_link_libraries(ThingIntegration rulessimulation rulesbase modules common physics ${PYTHON_TESTS_LIBS})


# CLIENT_TESTS
wf_add_test(Py_CreatorClientTest.cpp python_testers.cpp ../src/client/cyclient/CyPy_CreatorClient.cpp
    ../src/client/cyclient/CyPy_ObserverClient.cpp
    ../src/client/cyclient/Python_ClientAPI.cpp)
target_link_libraries(Py_CreatorClientTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_ObserverClientTest.cpp python_testers.cpp ../src/client/cyclient/CyPy_CreatorClient.cpp
    ../src/client/cyclient/CyPy_ObserverClient.cpp
    ../src/client/cyclient/Python_ClientAPI.cpp)
target_link_libraries(Py_ObserverClientTest ${PYTHON_TESTS_LIBS})

wf_add_test(ClientConnectionTest.cpp ../src/client/cyclient/ClientConnection.cpp)
wf_add_test(BaseClientTest.cpp ../src/client/cyclient/BaseClient.cpp)
wf_add_test(ClientPropertyManagerTest.cpp ../src/client/ClientPropertyManager.cpp
    ../src/common/PropertyManager.cpp)


# CLIENT_INTEGRATION_TESTS
wf_add_test(ClientConnectionIntegration.cpp ../src/client/cyclient/ClientConnection.cpp
    ../src/common/AtlasStreamClient.cpp)

# SERVER_TESTS

wf_add_test(RulesetTest.cpp ../src/server/Ruleset.cpp)
wf_add_test(EntityBuilderTest.cpp ../src/server/EntityBuilder.cpp)
wf_add_test(PropertyFlagTest.cpp TestPropertyManager.cpp ../src/rules/simulation/Thing.cpp
    ../src/rules/simulation/Entity.cpp
    ../src/rules/LocatedEntity.cpp
    ../src/common/Property.cpp)
wf_add_test(AccountTest.cpp ../src/server/Account.cpp
    ../src/common/debug.cpp)
wf_add_test(AdminTest.cpp ../src/server/Admin.cpp
    ../src/common/debug.cpp)
wf_add_test(ServerAccountTest.cpp ../src/server/ServerAccount.cpp)
wf_add_test(PlayerTest.cpp ../src/server/Player.cpp
    ../src/common/debug.cpp)
wf_add_test(EntityFactoryTest.cpp ../src/server/EntityFactory.cpp
    ../src/common/EntityKit.cpp)
wf_add_test(buildidTest.cpp ${PROJECT_BINARY_DIR}/src/server/buildid.cpp)
wf_add_test(ConnectionTest.cpp ../src/server/Connection.cpp)
wf_add_test(TrustedConnectionTest.cpp ../src/server/TrustedConnection.cpp)
wf_add_test(WorldRouterTest.cpp ../src/server/WorldRouter.cpp)
wf_add_test(PeerTest.cpp ../src/server/Peer.cpp)
wf_add_test(LobbyTest.cpp ../src/server/Lobby.cpp)
wf_add_test(SpawnTest.cpp)
wf_add_test(SpawnEntityTest.cpp ../src/server/SpawnEntity.cpp)
wf_add_test(ArithmeticBuilderTest.cpp ../src/server/ArithmeticBuilder.cpp)
target_link_libraries(ArithmeticBuilderTest pycxx)

wf_add_test(ServerRoutingTest.cpp ../src/server/ServerRouting.cpp)
wf_add_test(StorageManagerTest.cpp ../src/server/StorageManager.cpp)
wf_add_test(HttpCacheTest.cpp ../src/server/HttpCache.cpp)

# SERVER_COMM_TESTS
wf_add_test(CommPeerTest.cpp ../src/server/CommPeer.cpp)
wf_add_test(CommMDNSPublisherTest.cpp ../src/server/CommMDNSPublisher.cpp)
wf_add_test(TeleportAuthenticatorTest.cpp ../src/server/PossessionAuthenticator.cpp)
wf_add_test(TeleportStateTest.cpp ../src/server/TeleportState.cpp)
wf_add_test(PendingTeleportTest.cpp ../src/server/PendingPossession.cpp)
wf_add_test(JunctureTest.cpp ../src/server/Juncture.cpp)
wf_add_test(ConnectableRouterTest.cpp)
wf_add_test(OpRuleHandlerTest.cpp ../src/server/OpRuleHandler.cpp)
wf_add_test(EntityRuleHandlerTest.cpp ../src/server/EntityRuleHandler.cpp)

wf_add_test(PropertyRuleHandlerTest.cpp ../src/server/PropertyRuleHandler.cpp)
wf_add_test(IdleConnectorTest.cpp ../src/server/IdleConnector.cpp)
wf_add_test(PersistenceTest.cpp ../src/server/Persistence.cpp)
wf_add_test(SystemAccountTest.cpp ../src/server/SystemAccount.cpp)
wf_add_test(CorePropertyManagerTest.cpp ../src/server/CorePropertyManager.cpp)

# SERVER_INTEGRATION_TESTS

wf_add_test(WorldRouterIntegration.cpp ../src/server/WorldRouter.cpp
    ../src/server/EntityBuilder.cpp
    ../src/server/EntityFactory.cpp
    ../src/server/EntityRuleHandler.cpp
    ../src/server/SpawnEntity.cpp
    ../src/server/ArithmeticBuilder.cpp
    ../src/rules/Domain.cpp
    ../src/rules/LocatedEntity.cpp
    ../src/rules/simulation/Entity.cpp
    ../src/rules/simulation/Thing.cpp
    ../src/rules/simulation/World.cpp
    ../src/rules/simulation/BaseWorld.cpp)
target_link_libraries(WorldRouterIntegration modules physics common pycxx)
wf_add_test(RulesetIntegration.cpp ../src/server/Ruleset.cpp
    ../src/server/EntityBuilder.cpp
    ../src/server/EntityFactory.cpp
    ../src/server/OpRuleHandler.cpp
    ../src/server/EntityRuleHandler.cpp
    ../src/server/ArchetypeRuleHandler.cpp
    ../src/server/PropertyRuleHandler.cpp
    ../src/server/Persistence.cpp
    ../src/rules/simulation/BaseWorld.cpp)
target_link_libraries(RulesetIntegration common pycxx)
#Rulesetintegration_LDFLAGS = $(PYTHON_LINKER_FLAGS)
wf_add_test(ConnectionShakerIntegration.cpp ../src/server/Connection.cpp
    ../src/common/Shaker.cpp)
wf_add_test(ConnectionCreatorIntegration.cpp
    ../src/server/Connection.cpp)
target_link_libraries(ConnectionCreatorIntegration rulessimulation rulesbase modules physics common pycxx)
wf_add_test(TrustedConnectionCreatorIntegration.cpp
    ../src/server/Connection.cpp)
target_link_libraries(TrustedConnectionCreatorIntegration rulessimulation rulesbase modules physics common pycxx)
wf_add_test(AccountConnectionCharacterIntegration.cpp
    ../src/server/Account.cpp
    ../src/server/Connection.cpp)
target_link_libraries(AccountConnectionCharacterIntegration rulessimulation rulesbase modules physics common pycxx)
wf_add_test(AccountIntegration.cpp)
target_link_libraries(AccountIntegration server ${PYTHON_TESTS_LIBS})

wf_add_test(EntityFactoryTypeNodeIntegration.cpp TestPropertyManager.cpp ../src/server/EntityFactory.cpp
    ../src/common/EntityKit.cpp
    ../src/common/TypeNode.cpp
    ../src/common/PropertyManager.cpp
    ../src/common/Property.cpp)
wf_add_test(AccountConnectionIntegration.cpp ../src/server/Account.cpp
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
wf_add_test(AccountServerLobbyIntegration.cpp ../src/server/Account.cpp
    ../src/server/ServerRouting.cpp
    ../src/server/Lobby.cpp)


# Other TESTS
#wf_add_test(MasterTest.cpp ${PROJECT_SOURCE_DIR}/server/Master.cpp)
#target_link_libraries(MasterTest common)


# TOOLS_TESTS
wf_add_test(AdminClientTest.cpp ../src/tools/AdminClient.cpp)
wf_add_test(FlusherTest.cpp ../src/tools/Flusher.cpp
    ../src/common/ClientTask.cpp)
wf_add_test(OperationMonitorTest.cpp ../src/tools/OperationMonitor.cpp
    ../src/common/ClientTask.cpp)


# PYTHON_TESTS
#wf_add_test(python_class.cpp)
#target_link_libraries(python_class scriptpython rulessimulation rulesetmind rulesbase modules physics common)
