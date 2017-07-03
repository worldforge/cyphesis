add_definitions(-DTESTDATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

add_library(stubnavigation stubs/navigation/libstub.cpp)

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
endif(SEGFAULT_LIBRARY)

set(PYTHON_TESTS_LIBS
        scriptpython
        rulesetmind
        rulesetentity
        rulesetbase
        entityfilter
        stubnavigation
        modules
        physics
        common)


#Macro for adding a test
macro(wf_add_test TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ${PROJECT_SOURCE_DIR}/common/debug.cpp ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(check ${TEST_NAME})

endmacro()

macro(wf_add_benchmark TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ${PROJECT_SOURCE_DIR}/common/debug.cpp ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(benchmark ${TEST_NAME})

endmacro()


#### Common tests #####
wf_add_test(TestBaseTest.cpp)
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
wf_add_test(BaseWorldTest.cpp ${PROJECT_SOURCE_DIR}/common/BaseWorld.cpp)
wf_add_test(DatabaseTest.cpp ${PROJECT_SOURCE_DIR}/common/Database.cpp)
wf_add_test(idTest.cpp ${PROJECT_SOURCE_DIR}/common/id.cpp)
wf_add_test(StorageTest.cpp ${PROJECT_SOURCE_DIR}/common/Storage.cpp)
wf_add_test(debugTest.cpp ${PROJECT_SOURCE_DIR}/common/debug.cpp)
wf_add_test(globalsTest.cpp ${PROJECT_SOURCE_DIR}/common/globals.cpp)
target_compile_definitions(globalsTest PUBLIC -DBINDIR="${CMAKE_INSTALL_FULL_BINDIR}" -DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}" -DSYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}" -DLOCALSTATEDIR="${CMAKE_INSTALL_FULL_LOCALSTATEDIR}")
wf_add_test(OperationRouterTest.cpp ${PROJECT_SOURCE_DIR}/common/OperationRouter.cpp)
wf_add_test(RouterTest.cpp ${PROJECT_SOURCE_DIR}/common/Router.cpp)
wf_add_test(client_socketTest.cpp ${PROJECT_SOURCE_DIR}/common/client_socket.cpp)
wf_add_test(customTest.cpp ${PROJECT_SOURCE_DIR}/common/custom.cpp)
wf_add_test(MonitorsTest.cpp ${PROJECT_SOURCE_DIR}/common/Monitors.cpp ${PROJECT_SOURCE_DIR}/common/Variable.cpp)
wf_add_test(operationsTest.cpp ${PROJECT_SOURCE_DIR}/common/const.cpp)
wf_add_test(serialnoTest.cpp)
wf_add_test(newidTest.cpp ${PROJECT_SOURCE_DIR}/common/newid.cpp)
wf_add_test(TypeNodeTest.cpp ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(FormattedXMLWriterTest.cpp ${PROJECT_SOURCE_DIR}/common/FormattedXMLWriter.cpp)
wf_add_test(PropertyFactoryTest.cpp ${PROJECT_SOURCE_DIR}/common/PropertyFactory.cpp ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(PropertyManagerTest.cpp ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp)
wf_add_test(VariableTest.cpp ${PROJECT_SOURCE_DIR}/common/Variable.cpp)
wf_add_test(AtlasStreamClientTest.cpp ${PROJECT_SOURCE_DIR}/common/AtlasStreamClient.cpp)
wf_add_test(ClientTaskTest.cpp ${PROJECT_SOURCE_DIR}/common/ClientTask.cpp)
wf_add_test(utilsTest.cpp ${PROJECT_SOURCE_DIR}/common/utils.cpp)
wf_add_test(SystemTimeTest.cpp ${PROJECT_SOURCE_DIR}/common/SystemTime.cpp)
wf_add_test(TaskKitTest.cpp ${PROJECT_SOURCE_DIR}/common/TaskKit.cpp)
wf_add_test(ShakerTest.cpp ${PROJECT_SOURCE_DIR}/common/Shaker.cpp)
wf_add_test(ScriptKitTest.cpp ${PROJECT_SOURCE_DIR}/common/ScriptKit.cpp)
wf_add_test(EntityKitTest.cpp ${PROJECT_SOURCE_DIR}/common/EntityKit.cpp)
wf_add_test(atlas_helpersTest.cpp ${PROJECT_SOURCE_DIR}/common/atlas_helpers.cpp)
wf_add_test(LinkTest.cpp ${PROJECT_SOURCE_DIR}/common/Link.cpp)
wf_add_test(CommSocketTest.cpp ${PROJECT_SOURCE_DIR}/common/CommSocket.cpp)
wf_add_test(composeTest.cpp)

# PHYSICS_TESTS
wf_add_test(BBoxTest.cpp ${PROJECT_SOURCE_DIR}/physics/BBox.cpp ${PROJECT_SOURCE_DIR}/common/const.cpp)
wf_add_test(Vector3DTest.cpp ${PROJECT_SOURCE_DIR}/physics/Vector3D.cpp)
wf_add_test(QuaternionTest.cpp ${PROJECT_SOURCE_DIR}/physics/Quaternion.cpp)
wf_add_test(transformTest.cpp)
wf_add_test(distanceTest.cpp ${PROJECT_SOURCE_DIR}/physics/Vector3D ${PROJECT_SOURCE_DIR}/modules/Location)
wf_add_test(ShapeTest.cpp ${PROJECT_SOURCE_DIR}/physics/Shape.cpp ${PROJECT_SOURCE_DIR}/physics/Course.cpp)
wf_add_test(CourseTest.cpp ${PROJECT_SOURCE_DIR}/physics/Course.cpp)

# MODULE_TESTS

wf_add_test(DateTimeTest.cpp ${PROJECT_SOURCE_DIR}/modules/DateTime.cpp)
wf_add_test(LocationTest.cpp ${PROJECT_SOURCE_DIR}/modules/Location.cpp)
target_link_libraries(LocationTest physics)
wf_add_test(WorldTimeTest.cpp ${PROJECT_SOURCE_DIR}/modules/WorldTime.cpp ${PROJECT_SOURCE_DIR}/modules/DateTime.cpp)
wf_add_test(EntityRefTest.cpp ${PROJECT_SOURCE_DIR}/modules/EntityRef.cpp)
wf_add_test(TerrainContextTest.cpp ${PROJECT_SOURCE_DIR}/modules/TerrainContext.cpp)

# RULESETS_TESTS

set(ENTITYEXERCISE TestPropertyManager.cpp IGEntityExerciser.cpp EntityExerciser.cpp)

wf_add_test(LocatedEntityTest.cpp EntityExerciser.cpp ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp)
wf_add_test(EntityTest.cpp ${ENTITYEXERCISE} ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp)
wf_add_test(PlantTest.cpp ${ENTITYEXERCISE} ${PROJECT_SOURCE_DIR}/rulesets/Plant.cpp ${PROJECT_SOURCE_DIR}/rulesets/DensityProperty.cpp)
target_link_libraries(PlantTest physics)
wf_add_test(StackableTest.cpp ${ENTITYEXERCISE} ${PROJECT_SOURCE_DIR}/rulesets/Stackable.cpp)
wf_add_test(ThingTest.cpp ${ENTITYEXERCISE} ${PROJECT_SOURCE_DIR}/rulesets/Thing.cpp)
wf_add_test(WorldTest.cpp ${ENTITYEXERCISE} ${PROJECT_SOURCE_DIR}/rulesets/World.cpp)
wf_add_test(CharacterTest.cpp ${ENTITYEXERCISE} ${PROJECT_SOURCE_DIR}/rulesets/Character.cpp ${PROJECT_SOURCE_DIR}/physics/Quaternion.cpp)
wf_add_test(CreatorTest.cpp ${ENTITYEXERCISE} ${PROJECT_SOURCE_DIR}/rulesets/Creator.cpp)
wf_add_test(ThingupdatePropertiesTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/Thing.cpp ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(ContainerTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/Container.cpp)
wf_add_test(TaskTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/Task.cpp)
wf_add_test(EntityPropertyTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/EntityProperty.cpp ${PROJECT_SOURCE_DIR}/modules/EntityRef.cpp ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(AllPropertyTest.cpp PropertyExerciser.cpp ${PROJECT_SOURCE_DIR}/rulesets/AreaProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/AtlasProperties.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/CalendarProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/EntityProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LineProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/OutfitProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/StatisticsProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/TerrainProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/TerrainEffectorProperty.cpp
        ${PROJECT_SOURCE_DIR}/modules/EntityRef.cpp
        ${PROJECT_SOURCE_DIR}/modules/DateTime.cpp
        ${PROJECT_SOURCE_DIR}/modules/TerrainContext.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp
        ${PROJECT_SOURCE_DIR}/common/Inheritance.cpp
        ${PROJECT_SOURCE_DIR}/common/custom.cpp
        ${PROJECT_SOURCE_DIR}/common/operations.cpp
        ${PROJECT_SOURCE_DIR}/physics/Shape.cpp
        ${PROJECT_SOURCE_DIR}/physics/Course.cpp)
wf_add_test(ScriptTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/Script.cpp)
wf_add_test(AreaPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/AreaProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(BBoxPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/BBoxProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(CalendarPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/CalendarProperty.cpp
        ${PROJECT_SOURCE_DIR}/modules/DateTime.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(LinePropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/LineProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(MindPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/MindProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(ModePropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/ModeProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(OutfitPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/OutfitProperty.cpp
        ${PROJECT_SOURCE_DIR}/modules/EntityRef.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(SolidPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/SolidProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(StatisticsPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/StatisticsProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(StatusPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/StatusProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(SuspendedPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/SuspendedProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(TerrainModPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/TerrainModProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/TerrainProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(PythonClassTest.cpp python_testers.cpp ${PROJECT_SOURCE_DIR}/rulesets/PythonClass.cpp)
wf_add_test(TerrainPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/TerrainProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(TransientPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/TransientProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(TasksPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/TasksProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(InternalPropertiesTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/InternalProperties.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(EntityPropertiesTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/EntityProperties.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(AtlasPropertiesTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/AtlasProperties.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(SpawnPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/SpawnProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(SpawnerPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/SpawnerProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(VisibilityPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/VisibilityProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(ExternalPropertyTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/ExternalProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(BurnSpeedPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/BurnSpeedProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(BiomassPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/BiomassProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(DecaysPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/DecaysProperty.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(BaseMindTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/BaseMind.cpp ${PROJECT_SOURCE_DIR}/rulesets/MemMap.cpp)
wf_add_test(MemEntityTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/MemEntity.cpp)
wf_add_test(MemMapTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/MemMap.cpp)
wf_add_test(MovementTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/Movement.cpp)
wf_add_test(PedestrianTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/Pedestrian.cpp ${PROJECT_SOURCE_DIR}/rulesets/Movement.cpp)
wf_add_test(ExternalMindTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/ExternalMind.cpp)
wf_add_test(MindFactoryTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/MindFactory.cpp)
wf_add_test(PythonContextTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/PythonContext.cpp)
wf_add_test(ArithmeticScriptTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/ArithmeticScript.cpp)
wf_add_test(PythonArithmeticScriptTest.cpp python_testers.cpp ${PROJECT_SOURCE_DIR}/rulesets/PythonArithmeticScript.cpp)
wf_add_test(ArithmeticFactoryTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/ArithmeticFactory.cpp)
wf_add_test(PythonArithmeticFactoryTest.cpp python_testers.cpp ${PROJECT_SOURCE_DIR}/rulesets/PythonArithmeticFactory.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/PythonClass.cpp)
wf_add_test(TerrainModTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/TerrainModTranslator.cpp)
wf_add_test(TerrainEffectorPropertyTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/TerrainEffectorProperty.cpp)
wf_add_test(GeometryPropertyTest.cpp PropertyCoverage.cpp ${PROJECT_SOURCE_DIR}/rulesets/GeometryProperty.cpp
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

wf_add_test(Py_TerrainModPropertyTest.cpp python_testers.cpp)
target_link_libraries(Py_TerrainModPropertyTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_ShapeTest.cpp python_testers.cpp)
target_link_libraries(Py_ShapeTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_FilterTest.cpp python_testers.cpp)
target_link_libraries(Py_FilterTest ${PYTHON_TESTS_LIBS})

wf_add_test(PythonWrapperTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/PythonWrapper.cpp)

wf_add_test(PythonEntityScriptTest.cpp python_testers.cpp)
target_link_libraries(PythonEntityScriptTest ${PYTHON_TESTS_LIBS})

#Entity filter tests

wf_add_test(EntityFilterTest.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/EntityProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/OutfitProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/BBoxProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
        ${PROJECT_SOURCE_DIR}/modules/EntityRef.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp
        ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp
        ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp)
target_link_libraries(EntityFilterTest entityfilter)

wf_add_test(EntityFilterParserTest.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/EntityProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/OutfitProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/BBoxProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
        ${PROJECT_SOURCE_DIR}/modules/EntityRef.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp
        ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp
        ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp)
target_link_libraries(EntityFilterParserTest entityfilter)

wf_add_test(EntityFilterProvidersTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/EntityProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/OutfitProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/BBoxProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
        ${PROJECT_SOURCE_DIR}/modules/EntityRef.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp
        ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp
        ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp)
target_link_libraries(EntityFilterProvidersTest entityfilter)


# RULESETS_INTEGRATION

wf_add_test(GeometryPropertyIntegration.cpp ${PROJECT_SOURCE_DIR}/rulesets/GeometryProperty.cpp)
target_link_libraries(GeometryPropertyIntegration rulesetentity rulesetbase physics modules common)

wf_add_benchmark(PhysicalDomainBenchmark.cpp ${PROJECT_SOURCE_DIR}/rulesets/PhysicalDomain.cpp)
target_link_libraries(PhysicalDomainBenchmark rulesetentity rulesetbase physics modules common)

wf_add_test(PhysicalDomainIntegrationTest.cpp ${PROJECT_SOURCE_DIR}/rulesets/PhysicalDomain.cpp)
target_link_libraries(PhysicalDomainIntegrationTest rulesetentity rulesetbase physics modules common)

wf_add_test(TerrainPropertyIntegration.cpp ${PROJECT_SOURCE_DIR}/rulesets/TerrainModProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/TerrainEffectorProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/TerrainProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/TerrainModTranslator.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Domain.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/AtlasProperties.cpp
        ${PROJECT_SOURCE_DIR}/common/BaseWorld.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp
        ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp
        ${PROJECT_SOURCE_DIR}/common/Router.cpp
        ${PROJECT_SOURCE_DIR}/common/operations.cpp
        ${PROJECT_SOURCE_DIR}/common/id.cpp)

target_link_libraries(TerrainPropertyIntegration modules physics)

wf_add_test(PropertyEntityIntegration.cpp ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp
        ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp)
wf_add_test(DecaysPropertyIntegration.cpp ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/DecaysProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp
        ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp)
wf_add_test(BiomassPropertyIntegration.cpp ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/BiomassProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp
        ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp)
wf_add_test(TerrainModPropertyIntegration.cpp ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/TerrainEffectorProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/TerrainModProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/TerrainModTranslator.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/TerrainProperty.cpp
        ${PROJECT_SOURCE_DIR}/modules/EntityRef.cpp
        ${PROJECT_SOURCE_DIR}/modules/TerrainContext.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp
        ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp)
wf_add_test(StatisticsPropertyIntegration.cpp ${PROJECT_SOURCE_DIR}/rulesets/StatisticsProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp)
wf_add_test(BaseMindMapEntityIntegration.cpp ${PROJECT_SOURCE_DIR}/rulesets/BaseMind.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/MemEntity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/MemMap.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/BBoxProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/SolidProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/InternalProperties.cpp
        ${PROJECT_SOURCE_DIR}/modules/Location.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(AreaPropertyIntegration.cpp ${PROJECT_SOURCE_DIR}/rulesets/AreaProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
        ${PROJECT_SOURCE_DIR}/physics/Course.cpp
        ${PROJECT_SOURCE_DIR}/physics/Shape.cpp)
wf_add_test(SuspendedPropertyIntegration.cpp ${PROJECT_SOURCE_DIR}/rulesets/SuspendedProperty.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp)
target_link_libraries(SuspendedPropertyIntegration common)
wf_add_test(ThingIntegration.cpp)
target_link_libraries(ThingIntegration rulesetentity rulesetbase modules common physics)


# CLIENT_TESTS
wf_add_test(Py_CreatorClientTest.cpp python_testers.cpp ${PROJECT_SOURCE_DIR}/client/Py_CreatorClient.cpp
        ${PROJECT_SOURCE_DIR}/client/Py_ObserverClient.cpp
        ${PROJECT_SOURCE_DIR}/client/Python_ClientAPI.cpp)
target_link_libraries(Py_CreatorClientTest ${PYTHON_TESTS_LIBS})

wf_add_test(Py_ObserverClientTest.cpp python_testers.cpp ${PROJECT_SOURCE_DIR}/client/Py_CreatorClient.cpp
        ${PROJECT_SOURCE_DIR}/client/Py_ObserverClient.cpp
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
wf_add_test(PropertyFlagTest.cpp TestPropertyManager.cpp ${PROJECT_SOURCE_DIR}/rulesets/Thing.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
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
wf_add_test(TaskFactoryTest.cpp ${PROJECT_SOURCE_DIR}/server/TaskFactory.cpp)
wf_add_test(buildidTest.cpp ${PROJECT_BINARY_DIR}/server/buildid.cpp)
wf_add_test(ConnectionTest.cpp ${PROJECT_SOURCE_DIR}/server/Connection.cpp)
wf_add_test(TrustedConnectionTest.cpp ${PROJECT_SOURCE_DIR}/server/TrustedConnection.cpp)
wf_add_test(WorldRouterTest.cpp ${PROJECT_SOURCE_DIR}/server/WorldRouter.cpp)
wf_add_test(PeerTest.cpp ${PROJECT_SOURCE_DIR}/server/Peer.cpp)
wf_add_test(LobbyTest.cpp ${PROJECT_SOURCE_DIR}/server/Lobby.cpp)
wf_add_test(SpawnTest.cpp)
wf_add_test(SpawnEntityTest.cpp ${PROJECT_SOURCE_DIR}/server/SpawnEntity.cpp)
wf_add_test(ArithmeticBuilderTest.cpp ${PROJECT_SOURCE_DIR}/server/ArithmeticBuilder.cpp)
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
wf_add_test(ConnectableRouterTest.cpp ${PROJECT_SOURCE_DIR}/server/ConnectableRouter.cpp)
wf_add_test(RuleHandlerTest.cpp ${PROJECT_SOURCE_DIR}/server/RuleHandler.cpp)
wf_add_test(OpRuleHandlerTest.cpp ${PROJECT_SOURCE_DIR}/server/OpRuleHandler.cpp)
wf_add_test(EntityRuleHandlerTest.cpp ${PROJECT_SOURCE_DIR}/server/EntityRuleHandler.cpp
        ${PROJECT_SOURCE_DIR}/server/RuleHandler.cpp)
wf_add_test(TaskRuleHandlerTest.cpp ${PROJECT_SOURCE_DIR}/server/TaskRuleHandler.cpp
        ${PROJECT_SOURCE_DIR}/server/RuleHandler.cpp)
wf_add_test(PropertyRuleHandlerTest.cpp ${PROJECT_SOURCE_DIR}/server/PropertyRuleHandler.cpp)
wf_add_test(IdleConnectorTest.cpp ${PROJECT_SOURCE_DIR}/server/IdleConnector.cpp)
wf_add_test(CommPSQLSocketTest.cpp ${PROJECT_SOURCE_DIR}/server/CommPSQLSocket.cpp)
wf_add_test(PersistenceTest.cpp ${PROJECT_SOURCE_DIR}/server/Persistence.cpp)
wf_add_test(SystemAccountTest.cpp ${PROJECT_SOURCE_DIR}/server/SystemAccount.cpp)
wf_add_test(CorePropertyManagerTest.cpp ${PROJECT_SOURCE_DIR}/server/CorePropertyManager.cpp)

# SERVER_INTEGRATION_TESTS


wf_add_test(WorldRouterIntegration.cpp ${PROJECT_SOURCE_DIR}/server/WorldRouter.cpp
        ${PROJECT_SOURCE_DIR}/server/EntityBuilder.cpp
        ${PROJECT_SOURCE_DIR}/server/EntityFactory.cpp
        ${PROJECT_SOURCE_DIR}/server/TaskFactory.cpp
        ${PROJECT_SOURCE_DIR}/server/SpawnEntity.cpp
        ${PROJECT_SOURCE_DIR}/server/ArithmeticBuilder.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Character.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Domain.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Thing.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/World.cpp)
target_link_libraries(WorldRouterIntegration modules physics common)
wf_add_test(RulesetIntegration.cpp ${PROJECT_SOURCE_DIR}/server/Ruleset.cpp
        ${PROJECT_SOURCE_DIR}/server/EntityBuilder.cpp
        ${PROJECT_SOURCE_DIR}/server/EntityFactory.cpp
        ${PROJECT_SOURCE_DIR}/server/TaskRuleHandler.cpp
        ${PROJECT_SOURCE_DIR}/server/OpRuleHandler.cpp
        ${PROJECT_SOURCE_DIR}/server/EntityRuleHandler.cpp
        ${PROJECT_SOURCE_DIR}/server/ArchetypeRuleHandler.cpp
        ${PROJECT_SOURCE_DIR}/server/PropertyRuleHandler.cpp
        ${PROJECT_SOURCE_DIR}/server/RuleHandler.cpp
        ${PROJECT_SOURCE_DIR}/server/Persistence.cpp
        ${PROJECT_SOURCE_DIR}/server/TaskFactory.cpp)
target_link_libraries(RulesetIntegration common)
#Rulesetintegration_LDFLAGS = $(PYTHON_LINKER_FLAGS)
wf_add_test(ConnectionShakerIntegration.cpp ${PROJECT_SOURCE_DIR}/server/Connection.cpp
        ${PROJECT_SOURCE_DIR}/common/Shaker.cpp)
wf_add_test(ConnectionCharacterIntegration.cpp ${PROJECT_SOURCE_DIR}/physics/Quaternion.cpp
        ${PROJECT_SOURCE_DIR}/server/Connection.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Character.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/ExternalMind.cpp)
wf_add_test(ConnectionCreatorIntegration.cpp ${PROJECT_SOURCE_DIR}/physics/Quaternion.cpp
        ${PROJECT_SOURCE_DIR}/server/Connection.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Creator.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Character.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/ExternalMind.cpp)
wf_add_test(TrustedConnectionCreatorIntegration.cpp ${PROJECT_SOURCE_DIR}/physics/Quaternion.cpp
        ${PROJECT_SOURCE_DIR}/server/Connection.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Creator.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Character.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/ExternalMind.cpp)
wf_add_test(AccountConnectionCharacterIntegration.cpp ${PROJECT_SOURCE_DIR}/physics/Quaternion.cpp
        ${PROJECT_SOURCE_DIR}/server/Account.cpp
        ${PROJECT_SOURCE_DIR}/server/Connection.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Character.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/ExternalMind.cpp)
wf_add_test(AccountIntegration.cpp ${PROJECT_SOURCE_DIR}/server/EntityBuilder.cpp
        ${PROJECT_SOURCE_DIR}/server/EntityFactory.cpp
        ${PROJECT_SOURCE_DIR}/server/CorePropertyManager.cpp
        ${PROJECT_SOURCE_DIR}/server/ServerRouting.cpp
        ${PROJECT_SOURCE_DIR}/server/Lobby.cpp
        ${PROJECT_SOURCE_DIR}/server/Account.cpp
        ${PROJECT_SOURCE_DIR}/server/Connection.cpp
        ${PROJECT_SOURCE_DIR}/server/PossessionAuthenticator.cpp
        ${PROJECT_SOURCE_DIR}/server/PendingPossession.cpp
        ${PROJECT_SOURCE_DIR}/server/WorldRouter.cpp
        ${PROJECT_SOURCE_DIR}/server/SpawnEntity.cpp
        ${PROJECT_SOURCE_DIR}/server/ConnectableRouter.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Domain.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Entity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/ExternalMind.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Character.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/LocatedEntity.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Task.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/Thing.cpp
        ${PROJECT_SOURCE_DIR}/rulesets/World.cpp
        ${PROJECT_SOURCE_DIR}/modules/DateTime.cpp
        ${PROJECT_SOURCE_DIR}/modules/EntityRef.cpp
        ${PROJECT_SOURCE_DIR}/modules/Location.cpp
        ${PROJECT_SOURCE_DIR}/common/BaseWorld.cpp
        ${PROJECT_SOURCE_DIR}/common/const.cpp
        ${PROJECT_SOURCE_DIR}/common/id.cpp
        ${PROJECT_SOURCE_DIR}/common/Link.cpp
        ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp
        ${PROJECT_SOURCE_DIR}/common/Router.cpp)
target_link_libraries(AccountIntegration modules physics)

wf_add_test(EntityFactoryTypeNodeIntegration.cpp TestPropertyManager.cpp ${PROJECT_SOURCE_DIR}/server/EntityFactory.cpp
        ${PROJECT_SOURCE_DIR}/common/EntityKit.cpp
        ${PROJECT_SOURCE_DIR}/common/TypeNode.cpp
        ${PROJECT_SOURCE_DIR}/common/PropertyManager.cpp
        ${PROJECT_SOURCE_DIR}/common/Property.cpp)
wf_add_test(AccountConnectionIntegration.cpp ${PROJECT_SOURCE_DIR}/server/Account.cpp
        ${PROJECT_SOURCE_DIR}/server/Admin.cpp
        ${PROJECT_SOURCE_DIR}/server/ConnectableRouter.cpp
        ${PROJECT_SOURCE_DIR}/server/Connection.cpp
        ${PROJECT_SOURCE_DIR}/server/Lobby.cpp
        ${PROJECT_SOURCE_DIR}/server/Player.cpp
        ${PROJECT_SOURCE_DIR}/server/ServerAccount.cpp
        ${PROJECT_SOURCE_DIR}/server/ServerRouting.cpp
        ${PROJECT_SOURCE_DIR}/server/SystemAccount.cpp
        ${PROJECT_SOURCE_DIR}/common/BaseWorld.cpp
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
#target_link_libraries(python_class scriptpython rulesetentity rulesetmind rulesetbase modules physics common)
