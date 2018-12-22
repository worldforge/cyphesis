add_definitions(-DTESTDATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

add_library(stubnavigation stubs/navigation/libstub.cpp DatabaseNull.h)

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
    add_executable(${TEST_NAME} ${TEST_FILE} ${PROJECT_SOURCE_DIR}/common/debug.cpp TestWorld.cpp ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(check ${TEST_NAME})
    target_link_libraries(${TEST_NAME} pycxx ${PYTHON_LIBRARIES})

endmacro()

macro(wf_add_benchmark TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ${PROJECT_SOURCE_DIR}/common/debug.cpp ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(benchmark ${TEST_NAME})
    target_link_libraries(${TEST_NAME} pycxx ${PYTHON_LIBRARIES})

endmacro()


#### Common tests #####
wf_add_test(TestBaseTest.cpp)
wf_add_test(RefTest.cpp)
wf_add_test(OgreMeshDeserializerTest.cpp ../rules/simulation/OgreMeshDeserializer.cpp)
wf_add_test(logTest.cpp ${PROJECT_SOURCE_DIR}/common/log.cpp)
wf_add_test(InheritanceTest.cpp ${PROJECT_SOURCE_DIR}/common/Inheritance.cpp ${PROJECT_SOURCE_DIR}/common/custom.cpp)
wf_add_test(PropertyTest.cpp ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(systemTest.cpp ${PROJECT_SOURCE_DIR}/common/system.cpp)
wf_add_test(system_netTest.cpp ${PROJECT_SOURCE_DIR}/common/system_net.cpp)
wf_add_test(system_uidTest.cpp ${PROJECT_SOURCE_DIR}/common/system_uid.cpp)
wf_add_test(type_utilsTest.cpp ${PROJECT_SOURCE_DIR}/common/type_utils.cpp)
wf_add_test(constTest.cpp ${PROJECT_SOURCE_DIR}/common/const.cpp)
wf_add_test(randomTest.cpp)
wf_add_test(AddTest.cpp OperationExerciser.cpp)
wf_add_test(AttackTest.cpp)
wf_add_test(BurnTest.cpp)
wf_add_test(ConnectTest.cpp)
wf_add_test(DropTest.cpp)
wf_add_test(EatTest.cpp)
wf_add_test(MonitorTest.cpp)
wf_add_test(NourishTest.cpp)
wf_add_test(PickupTest.cpp)
wf_add_test(SetupTest.cpp)
wf_add_test(TickTest.cpp)
wf_add_test(UnseenTest.cpp)
wf_add_test(UpdateTest.cpp)
wf_add_test(AtlasFileLoaderTest.cpp ${PROJECT_SOURCE_DIR}/common/AtlasFileLoader.cpp)
wf_add_test(BaseWorldTest.cpp ../rules/simulation/BaseWorld.cpp)
wf_add_test(idTest.cpp ${PROJECT_SOURCE_DIR}/common/id.cpp)
wf_add_test(StorageTest.cpp ${PROJECT_SOURCE_DIR}/common/Storage.cpp)
wf_add_test(debugTest.cpp ${PROJECT_SOURCE_DIR}/common/debug.cpp)
wf_add_test(globalsTest.cpp ${PROJECT_SOURCE_DIR}/common/globals.cpp)
target_compile_definitions(globalsTest PUBLIC -DBINDIR="${CMAKE_INSTALL_FULL_BINDIR}" -DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}" -DSYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}" -DLOCALSTATEDIR="${CMAKE_INSTALL_FULL_LOCALSTATEDIR}")
wf_add_test(OperationRouterTest.cpp)
wf_add_test(RouterTest.cpp ${PROJECT_SOURCE_DIR}/common/Router.cpp)
wf_add_test(client_socketTest.cpp ${PROJECT_SOURCE_DIR}/common/client_socket.cpp)
wf_add_test(customTest.cpp ${PROJECT_SOURCE_DIR}/common/custom.cpp)
wf_add_test(MonitorsTest.cpp ${PROJECT_SOURCE_DIR}/common/Monitors.cpp ${PROJECT_SOURCE_DIR}/common/Variable.cpp)
wf_add_test(operationsTest.cpp ${PROJECT_SOURCE_DIR}/common/const.cpp)
wf_add_test(serialnoTest.cpp)
wf_add_test(newidTest.cpp ${PROJECT_SOURCE_DIR}/common/newid.cpp)
wf_add_test(TypeNodeTest.cpp ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(FormattedXMLWriterTest.cpp ${PROJECT_SOURCE_DIR}/common/FormattedXMLWriter.cpp)
wf_add_test(PropertyFactoryTest.cpp ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(PropertyManagerTest.cpp ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp)
wf_add_test(VariableTest.cpp ${PROJECT_SOURCE_DIR}/common/Variable.cpp)
wf_add_test(AtlasStreamClientTest.cpp ${PROJECT_SOURCE_DIR}/common/AtlasStreamClient.cpp)
wf_add_test(ClientTaskTest.cpp ${PROJECT_SOURCE_DIR}/common/ClientTask.cpp)
wf_add_test(utilsTest.cpp ${PROJECT_SOURCE_DIR}/common/utils.cpp)
wf_add_test(SystemTimeTest.cpp)
wf_add_test(ShakerTest.cpp ${PROJECT_SOURCE_DIR}/common/Shaker.cpp)
wf_add_test(ScriptKitTest.cpp)
wf_add_test(EntityKitTest.cpp ${PROJECT_SOURCE_DIR}/common/EntityKit.cpp)
wf_add_test(LinkTest.cpp ${PROJECT_SOURCE_DIR}/common/Link.cpp)
wf_add_test(CommSocketTest.cpp)
wf_add_test(composeTest.cpp)
wf_add_test(FileSystemObserverIntegrationTest.cpp ${PROJECT_SOURCE_DIR}/common/FileSystemObserver.cpp)
target_link_libraries(FileSystemObserverIntegrationTest common)

# PHYSICS_TESTS
wf_add_test(BBoxTest.cpp ${PROJECT_SOURCE_DIR}/physics/BBox.cpp ${PROJECT_SOURCE_DIR}/common/const.cpp)
wf_add_test(Vector3DTest.cpp ${PROJECT_SOURCE_DIR}/physics/Vector3D.cpp)
wf_add_test(QuaternionTest.cpp ${PROJECT_SOURCE_DIR}/physics/Quaternion.cpp)
wf_add_test(transformTest.cpp)
wf_add_test(distanceTest.cpp ${PROJECT_SOURCE_DIR}/physics/Vector3D ${PROJECT_SOURCE_DIR}/rules/Location ${PROJECT_SOURCE_DIR}/rules/EntityLocation)
wf_add_test(ShapeTest.cpp ${PROJECT_SOURCE_DIR}/physics/Shape.cpp ${PROJECT_SOURCE_DIR}/physics/Course.cpp)
wf_add_test(CourseTest.cpp ${PROJECT_SOURCE_DIR}/physics/Course.cpp)

# MODULE_TESTS

wf_add_test(DateTimeTest.cpp ${PROJECT_SOURCE_DIR}/modules/DateTime.cpp)
wf_add_test(LocationTest.cpp ../rules/Location.cpp ../rules/EntityLocation.cpp)
target_link_libraries(LocationTest physics)
wf_add_test(WorldTimeTest.cpp ${PROJECT_SOURCE_DIR}/modules/WorldTime.cpp ${PROJECT_SOURCE_DIR}/modules/DateTime.cpp)
wf_add_test(EntityRefTest.cpp ${PROJECT_SOURCE_DIR}/modules/WeakEntityRef.cpp)
wf_add_test(TerrainContextTest.cpp ${PROJECT_SOURCE_DIR}/modules/TerrainContext.cpp)

# RULESETS_TESTS

set(ENTITYEXERCISE TestPropertyManager.cpp IGEntityExerciser.cpp EntityExerciser.cpp)

wf_add_test(LocatedEntityTest.cpp EntityExerciser.cpp ../rules/LocatedEntity.cpp)
wf_add_test(EntityTest.cpp ${ENTITYEXERCISE} ../rules/simulation/Entity.cpp ../rules/LocatedEntity.cpp)
wf_add_test(StackableTest.cpp ${ENTITYEXERCISE} ../rules/simulation/Stackable.cpp)
wf_add_test(ThingTest.cpp ${ENTITYEXERCISE} ../rules/simulation/Thing.cpp)
wf_add_test(WorldTest.cpp ${ENTITYEXERCISE} ../rules/simulation/World.cpp)
wf_add_test(ThingupdatePropertiesTest.cpp ../rules/simulation/Thing.cpp ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(TaskTest.cpp ../rules/simulation/Task.cpp)
wf_add_test(EntityPropertyTest.cpp ../rules/simulation/EntityProperty.cpp ${PROJECT_SOURCE_DIR}/modules/WeakEntityRef.cpp ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(AllPropertyTest.cpp PropertyExerciser.cpp ../rules/simulation/AreaProperty.cpp
    ../rules/AtlasProperties.cpp
    ../rules/simulation/CalendarProperty.cpp
    ../rules/simulation/EntityProperty.cpp
    ../rules/simulation/LineProperty.cpp
    ../rules/simulation/StatisticsProperty.cpp
    ../rules/simulation/TerrainProperty.cpp
    ../rules/simulation/TerrainEffectorProperty.cpp
    ${PROJECT_SOURCE_DIR}/modules/WeakEntityRef.cpp
    ${PROJECT_SOURCE_DIR}/modules/DateTime.cpp
    ${PROJECT_SOURCE_DIR}/modules/TerrainContext.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp
    ${PROJECT_SOURCE_DIR}/common/Inheritance.cpp
    ${PROJECT_SOURCE_DIR}/common/custom.cpp
    ${PROJECT_SOURCE_DIR}/common/operations.cpp
    ${PROJECT_SOURCE_DIR}/physics/Shape.cpp
    ${PROJECT_SOURCE_DIR}/physics/Course.cpp)
wf_add_test(ScriptTest.cpp ../rules/Script.cpp)
wf_add_test(AreaPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/AreaProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(BBoxPropertyTest.cpp PropertyCoverage.cpp ../rules/BBoxProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(CalendarPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/CalendarProperty.cpp
    ${PROJECT_SOURCE_DIR}/modules/DateTime.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(LinePropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/LineProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(MindPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/MindProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
target_link_libraries(MindPropertyTest pycxx)

wf_add_test(ModePropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/ModeProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(SolidPropertyTest.cpp PropertyCoverage.cpp ../rules/SolidProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(StatisticsPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/StatisticsProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(StatusPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/StatusProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(SuspendedPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/SuspendedProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(TerrainModPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/TerrainModProperty.cpp
    ../rules/simulation/TerrainProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(PythonClassTest.cpp python_testers.cpp ../rules/python/PythonClass.cpp)
target_link_libraries(PythonClassTest pycxx)
wf_add_test(TerrainPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/TerrainProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(TransientPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/TransientProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(TasksPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/TasksProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(InternalPropertiesTest.cpp PropertyCoverage.cpp ../rules/simulation/InternalProperties.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(EntityPropertiesTest.cpp PropertyCoverage.cpp ../rules/EntityProperties.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(AtlasPropertiesTest.cpp PropertyCoverage.cpp ../rules/AtlasProperties.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(SpawnPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/SpawnProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(SpawnerPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/SpawnerProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(VisibilityPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/VisibilityProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(ExternalPropertyTest.cpp ../rules/simulation/ExternalProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(BurnSpeedPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/BurnSpeedProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(BiomassPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/BiomassProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(DecaysPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/DecaysProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(BaseMindTest.cpp ../rules/ai/BaseMind.cpp ../rules/ai/MemMap.cpp)
wf_add_test(MemEntityTest.cpp ../rules/ai/MemEntity.cpp)
wf_add_test(MemMapTest.cpp ../rules/ai/MemMap.cpp)
wf_add_test(MovementTest.cpp ../rules/simulation/Movement.cpp)
wf_add_test(PedestrianTest.cpp ../rules/simulation/Pedestrian.cpp ../rules/simulation/Movement.cpp)
wf_add_test(ExternalMindTest.cpp ../rules/simulation/ExternalMind.cpp)
wf_add_test(PythonContextTest.cpp ../rules/python/PythonContext.cpp)
wf_add_test(ArithmeticScriptTest.cpp)
wf_add_test(PythonArithmeticScriptTest.cpp python_testers.cpp ../rules/python/PythonArithmeticScript.cpp)
target_link_libraries(PythonArithmeticScriptTest pycxx)

wf_add_test(ArithmeticFactoryTest.cpp)
wf_add_test(PythonArithmeticFactoryTest.cpp python_testers.cpp ../rules/python/PythonArithmeticFactory.cpp
    ../rules/python/PythonClass.cpp)
target_link_libraries(PythonArithmeticFactoryTest pycxx)
wf_add_test(TerrainModTest.cpp ../rules/simulation/TerrainModTranslator.cpp)
wf_add_test(TerrainEffectorPropertyTest.cpp ../rules/simulation/TerrainEffectorProperty.cpp)
wf_add_test(GeometryPropertyTest.cpp PropertyCoverage.cpp ../rules/simulation/GeometryProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)

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
    ../rules/simulation/EntityProperty.cpp
    ../rules/simulation/Entity.cpp
    ../rules/BBoxProperty.cpp
    ../rules/LocatedEntity.cpp
    ../rules/simulation/PlantedOnProperty.cpp
    ${PROJECT_SOURCE_DIR}/modules/WeakEntityRef.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp
    ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp
    ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp)
target_link_libraries(EntityFilterTest entityfilter)
target_compile_definitions(EntityFilterTest PRIVATE -DBOOST_SPIRIT_DEBUG)

wf_add_test(EntityFilterParserTest.cpp
    ../rules/simulation/EntityProperty.cpp
    ../rules/simulation/Entity.cpp
    ../rules/BBoxProperty.cpp
    ../rules/LocatedEntity.cpp
    ${PROJECT_SOURCE_DIR}/modules/WeakEntityRef.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp
    ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp
    ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp)
target_link_libraries(EntityFilterParserTest entityfilter)

wf_add_test(EntityFilterProvidersTest.cpp ../rules/simulation/EntityProperty.cpp
    ../rules/simulation/Entity.cpp
    ../rules/BBoxProperty.cpp
    ../rules/LocatedEntity.cpp
    ../rules/simulation/PlantedOnProperty.cpp
    ${PROJECT_SOURCE_DIR}/modules/WeakEntityRef.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp
    ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp
    ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp)
target_link_libraries(EntityFilterProvidersTest entityfilter)


# RULESETS_INTEGRATION

wf_add_test(GeometryPropertyIntegration.cpp ../rules/simulation/GeometryProperty.cpp)
target_link_libraries(GeometryPropertyIntegration rulessimulation rulesbase physics modules common)

wf_add_benchmark(PhysicalDomainBenchmark.cpp ../rules/simulation/PhysicalDomain.cpp)
target_link_libraries(PhysicalDomainBenchmark rulessimulation rulesbase physics modules common)

wf_add_test(PhysicalDomainIntegrationTest.cpp ../rules/simulation/PhysicalDomain.cpp)
target_link_libraries(PhysicalDomainIntegrationTest rulessimulation rulesbase physics modules common)

wf_add_test(TerrainPropertyIntegration.cpp ../rules/simulation/TerrainModProperty.cpp
    ../rules/simulation/TerrainEffectorProperty.cpp
    ../rules/simulation/TerrainProperty.cpp
    ../rules/simulation/TerrainModTranslator.cpp
    ../rules/simulation/Entity.cpp
    ../rules/LocatedEntity.cpp
    ../rules/Domain.cpp
    ../rules/AtlasProperties.cpp
    ../rules/simulation/BaseWorld.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp
    ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp
    ${PROJECT_SOURCE_DIR}/common/Router.cpp
    ${PROJECT_SOURCE_DIR}/common/operations.cpp
    ${PROJECT_SOURCE_DIR}/common/id.cpp)

target_link_libraries(TerrainPropertyIntegration modules physics)

wf_add_test(PropertyEntityIntegration.cpp ../rules/simulation/Entity.cpp
    ../rules/LocatedEntity.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp
    ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp)
wf_add_test(DecaysPropertyIntegration.cpp ../rules/simulation/Entity.cpp
    ../rules/simulation/DecaysProperty.cpp
    ../rules/LocatedEntity.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp
    ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp)
wf_add_test(BiomassPropertyIntegration.cpp ../rules/simulation/Entity.cpp
    ../rules/simulation/BiomassProperty.cpp
    ../rules/LocatedEntity.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp
    ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp)
wf_add_test(TerrainModPropertyIntegration.cpp ../rules/simulation/Entity.cpp
    ../rules/LocatedEntity.cpp
    ../rules/simulation/TerrainEffectorProperty.cpp
    ../rules/simulation/TerrainModProperty.cpp
    ../rules/simulation/TerrainModTranslator.cpp
    ../rules/simulation/TerrainProperty.cpp
    ${PROJECT_SOURCE_DIR}/modules/WeakEntityRef.cpp
    ${PROJECT_SOURCE_DIR}/modules/TerrainContext.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp
    ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp)
wf_add_test(StatisticsPropertyIntegration.cpp ../rules/simulation/StatisticsProperty.cpp
    ../rules/simulation/Entity.cpp
    ../rules/LocatedEntity.cpp)
wf_add_test(BaseMindMapEntityIntegration.cpp ../rules/ai/BaseMind.cpp
    ../rules/ai/MemEntity.cpp
    ../rules/ai/MemMap.cpp
    ../rules/BBoxProperty.cpp
    ../rules/SolidProperty.cpp
    ../rules/SimpleProperty.cpp
    ../rules/simulation/InternalProperties.cpp
    ../rules/Location.cpp
    ../rules/EntityLocation.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(AreaPropertyIntegration.cpp ../rules/simulation/AreaProperty.cpp
    ../rules/simulation/Entity.cpp
    ../rules/LocatedEntity.cpp
    ${PROJECT_SOURCE_DIR}/physics/Course.cpp
    ${PROJECT_SOURCE_DIR}/physics/Shape.cpp)
wf_add_test(SuspendedPropertyIntegration.cpp ../rules/simulation/SuspendedProperty.cpp
    ../rules/simulation/Entity.cpp
    ../rules/LocatedEntity.cpp)
target_link_libraries(SuspendedPropertyIntegration common)
wf_add_test(ThingIntegration.cpp)
target_link_libraries(ThingIntegration rulessimulation rulesbase modules common physics)


# CLIENT_TESTS
wf_add_test(Py_CreatorClientTest.cpp python_testers.cpp ${PROJECT_SOURCE_DIR}/client/CyPy_CreatorClient.cpp
    ${PROJECT_SOURCE_DIR}/client/CyPy_ObserverClient.cpp
    ${PROJECT_SOURCE_DIR}/client/Python_ClientAPI.cpp)
target_link_libraries(Py_CreatorClientTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_ObserverClientTest.cpp python_testers.cpp ${PROJECT_SOURCE_DIR}/client/CyPy_CreatorClient.cpp
    ${PROJECT_SOURCE_DIR}/client/CyPy_ObserverClient.cpp
    ${PROJECT_SOURCE_DIR}/client/Python_ClientAPI.cpp)
target_link_libraries(Py_ObserverClientTest ${PYTHON_TESTS_LIBS})

wf_add_test(ClientConnectionTest.cpp ${PROJECT_SOURCE_DIR}/client/ClientConnection.cpp)
wf_add_test(BaseClientTest.cpp ${PROJECT_SOURCE_DIR}/client/BaseClient.cpp)
wf_add_test(ClientPropertyManagerTest.cpp ${PROJECT_SOURCE_DIR}/client/ClientPropertyManager.cpp
    ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp)


# CLIENT_INTEGRATION_TESTS
wf_add_test(ClientConnectionIntegration.cpp ${PROJECT_SOURCE_DIR}/client/ClientConnection.cpp
    ${PROJECT_SOURCE_DIR}/common/AtlasStreamClient.cpp)

# SERVER_TESTS

wf_add_test(RulesetTest.cpp ${PROJECT_SOURCE_DIR}/server/Ruleset.cpp)
wf_add_test(EntityBuilderTest.cpp ${PROJECT_SOURCE_DIR}/server/EntityBuilder.cpp)
wf_add_test(PropertyFlagTest.cpp TestPropertyManager.cpp ../rules/simulation/Thing.cpp
    ../rules/simulation/Entity.cpp
    ../rules/LocatedEntity.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(AccountTest.cpp ${PROJECT_SOURCE_DIR}/server/Account.cpp
    ${PROJECT_SOURCE_DIR}/common/debug.cpp)
wf_add_test(AdminTest.cpp ${PROJECT_SOURCE_DIR}/server/Admin.cpp
    ${PROJECT_SOURCE_DIR}/common/debug.cpp)
wf_add_test(ServerAccountTest.cpp ${PROJECT_SOURCE_DIR}/server/ServerAccount.cpp)
wf_add_test(PlayerTest.cpp ${PROJECT_SOURCE_DIR}/server/Player.cpp
    ${PROJECT_SOURCE_DIR}/common/debug.cpp)
wf_add_test(EntityFactoryTest.cpp ${PROJECT_SOURCE_DIR}/server/EntityFactory.cpp
    ${PROJECT_SOURCE_DIR}/common/EntityKit.cpp)
wf_add_test(buildidTest.cpp ${PROJECT_BINARY_DIR}/server/buildid.cpp)
wf_add_test(ConnectionTest.cpp ${PROJECT_SOURCE_DIR}/server/Connection.cpp)
wf_add_test(TrustedConnectionTest.cpp ${PROJECT_SOURCE_DIR}/server/TrustedConnection.cpp)
wf_add_test(WorldRouterTest.cpp ${PROJECT_SOURCE_DIR}/server/WorldRouter.cpp)
wf_add_test(PeerTest.cpp ${PROJECT_SOURCE_DIR}/server/Peer.cpp)
wf_add_test(LobbyTest.cpp ${PROJECT_SOURCE_DIR}/server/Lobby.cpp)
wf_add_test(SpawnTest.cpp)
wf_add_test(SimplePropertyTest.cpp PropertyCoverage.cpp ../rules/SimpleProperty.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(SpawnEntityTest.cpp ${PROJECT_SOURCE_DIR}/server/SpawnEntity.cpp)
wf_add_test(ArithmeticBuilderTest.cpp ${PROJECT_SOURCE_DIR}/server/ArithmeticBuilder.cpp)
target_link_libraries(ArithmeticBuilderTest pycxx)

wf_add_test(ServerRoutingTest.cpp ${PROJECT_SOURCE_DIR}/server/ServerRouting.cpp)
wf_add_test(StorageManagerTest.cpp ${PROJECT_SOURCE_DIR}/server/StorageManager.cpp)
wf_add_test(HttpCacheTest.cpp ${PROJECT_SOURCE_DIR}/server/HttpCache.cpp)

# SERVER_COMM_TESTS
wf_add_test(CommPeerTest.cpp ${PROJECT_SOURCE_DIR}/server/CommPeer.cpp)
wf_add_test(CommMDNSPublisherTest.cpp ${PROJECT_SOURCE_DIR}/server/CommMDNSPublisher.cpp)
wf_add_test(TeleportAuthenticatorTest.cpp ${PROJECT_SOURCE_DIR}/server/PossessionAuthenticator.cpp)
wf_add_test(TeleportStateTest.cpp ${PROJECT_SOURCE_DIR}/server/TeleportState.cpp)
wf_add_test(PendingTeleportTest.cpp ${PROJECT_SOURCE_DIR}/server/PendingPossession.cpp)
wf_add_test(JunctureTest.cpp ${PROJECT_SOURCE_DIR}/server/Juncture.cpp)
wf_add_test(ConnectableRouterTest.cpp)
wf_add_test(OpRuleHandlerTest.cpp ${PROJECT_SOURCE_DIR}/server/OpRuleHandler.cpp)
wf_add_test(EntityRuleHandlerTest.cpp ${PROJECT_SOURCE_DIR}/server/EntityRuleHandler.cpp)

wf_add_test(PropertyRuleHandlerTest.cpp ${PROJECT_SOURCE_DIR}/server/PropertyRuleHandler.cpp)
wf_add_test(IdleConnectorTest.cpp ${PROJECT_SOURCE_DIR}/server/IdleConnector.cpp)
wf_add_test(PersistenceTest.cpp ${PROJECT_SOURCE_DIR}/server/Persistence.cpp)
wf_add_test(SystemAccountTest.cpp ${PROJECT_SOURCE_DIR}/server/SystemAccount.cpp)
wf_add_test(CorePropertyManagerTest.cpp ${PROJECT_SOURCE_DIR}/server/CorePropertyManager.cpp)

# SERVER_INTEGRATION_TESTS

wf_add_test(WorldRouterIntegration.cpp ${PROJECT_SOURCE_DIR}/server/WorldRouter.cpp
    ${PROJECT_SOURCE_DIR}/server/EntityBuilder.cpp
    ${PROJECT_SOURCE_DIR}/server/EntityFactory.cpp
    ${PROJECT_SOURCE_DIR}/server/EntityRuleHandler.cpp
    ${PROJECT_SOURCE_DIR}/server/SpawnEntity.cpp
    ${PROJECT_SOURCE_DIR}/server/ArithmeticBuilder.cpp
    ../rules/Domain.cpp
    ../rules/LocatedEntity.cpp
    ../rules/simulation/Entity.cpp
    ../rules/simulation/Thing.cpp
    ../rules/simulation/World.cpp
    ../rules/simulation/BaseWorld.cpp)
target_link_libraries(WorldRouterIntegration modules physics common pycxx)
wf_add_test(RulesetIntegration.cpp ${PROJECT_SOURCE_DIR}/server/Ruleset.cpp
    ${PROJECT_SOURCE_DIR}/server/EntityBuilder.cpp
    ${PROJECT_SOURCE_DIR}/server/EntityFactory.cpp
    ${PROJECT_SOURCE_DIR}/server/OpRuleHandler.cpp
    ${PROJECT_SOURCE_DIR}/server/EntityRuleHandler.cpp
    ${PROJECT_SOURCE_DIR}/server/ArchetypeRuleHandler.cpp
    ${PROJECT_SOURCE_DIR}/server/PropertyRuleHandler.cpp
    ${PROJECT_SOURCE_DIR}/server/Persistence.cpp
    ../rules/simulation/BaseWorld.cpp)
target_link_libraries(RulesetIntegration common pycxx)
#Rulesetintegration_LDFLAGS = $(PYTHON_LINKER_FLAGS)
wf_add_test(ConnectionShakerIntegration.cpp ${PROJECT_SOURCE_DIR}/server/Connection.cpp
    ${PROJECT_SOURCE_DIR}/common/Shaker.cpp)
wf_add_test(ConnectionCreatorIntegration.cpp
    ${PROJECT_SOURCE_DIR}/server/Connection.cpp)
target_link_libraries(ConnectionCreatorIntegration rulessimulation rulesbase modules physics common pycxx)
wf_add_test(TrustedConnectionCreatorIntegration.cpp
    ${PROJECT_SOURCE_DIR}/server/Connection.cpp)
target_link_libraries(TrustedConnectionCreatorIntegration rulessimulation rulesbase modules physics common pycxx)
wf_add_test(AccountConnectionCharacterIntegration.cpp
    ${PROJECT_SOURCE_DIR}/server/Account.cpp
    ${PROJECT_SOURCE_DIR}/server/Connection.cpp)
target_link_libraries(AccountConnectionCharacterIntegration rulessimulation rulesbase modules physics common pycxx)
wf_add_test(AccountIntegration.cpp)
target_link_libraries(AccountIntegration server ${PYTHON_TESTS_LIBS})

wf_add_test(EntityFactoryTypeNodeIntegration.cpp TestPropertyManager.cpp ${PROJECT_SOURCE_DIR}/server/EntityFactory.cpp
    ${PROJECT_SOURCE_DIR}/common/EntityKit.cpp
    ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp
    ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(AccountConnectionIntegration.cpp ${PROJECT_SOURCE_DIR}/server/Account.cpp
    ${PROJECT_SOURCE_DIR}/server/Admin.cpp
    ${PROJECT_SOURCE_DIR}/server/Connection.cpp
    ${PROJECT_SOURCE_DIR}/server/Lobby.cpp
    ${PROJECT_SOURCE_DIR}/server/Player.cpp
    ${PROJECT_SOURCE_DIR}/server/ServerAccount.cpp
    ${PROJECT_SOURCE_DIR}/server/ServerRouting.cpp
    ${PROJECT_SOURCE_DIR}/server/SystemAccount.cpp
    ../rules/simulation/BaseWorld.cpp
    ${PROJECT_SOURCE_DIR}/common/Inheritance.cpp
    ${PROJECT_SOURCE_DIR}/common/Property.cpp
    ${PROJECT_SOURCE_DIR}/common/Router.cpp
    ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp
    ${PROJECT_SOURCE_DIR}/common/custom.cpp
    ${PROJECT_SOURCE_DIR}/common/operations.cpp)
target_link_libraries(AccountConnectionIntegration physics)
wf_add_test(AccountServerLobbyIntegration.cpp ${PROJECT_SOURCE_DIR}/server/Account.cpp
    ${PROJECT_SOURCE_DIR}/server/ServerRouting.cpp
    ${PROJECT_SOURCE_DIR}/server/Lobby.cpp)


# Other TESTS
#wf_add_test(MasterTest.cpp ${PROJECT_SOURCE_DIR}/server/Master.cpp)
#target_link_libraries(MasterTest common)


# TOOLS_TESTS
wf_add_test(AdminClientTest.cpp ${PROJECT_SOURCE_DIR}/tools/AdminClient.cpp)
wf_add_test(FlusherTest.cpp ${PROJECT_SOURCE_DIR}/tools/Flusher.cpp
    ${PROJECT_SOURCE_DIR}/common/ClientTask.cpp)
wf_add_test(OperationMonitorTest.cpp ${PROJECT_SOURCE_DIR}/tools/OperationMonitor.cpp
    ${PROJECT_SOURCE_DIR}/common/ClientTask.cpp)


# PYTHON_TESTS
#wf_add_test(python_class.cpp)
#target_link_libraries(python_class scriptpython rulessimulation rulesetmind rulesbase modules physics common)
