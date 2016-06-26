TEST_TESTS = TestBaseTest

COMMON_TESTS = logTest InheritanceTest PropertyTest \
               systemTest system_netTest system_uidTest \
               type_utilsTest \
               constTest randomTest AddTest AttackTest BurnTest \
               ConnectTest DropTest EatTest \
               MonitorTest NourishTest PickupTest SetupTest \
               TickTest UnseenTest UpdateTest AtlasFileLoaderTest \
               BaseWorldTest DatabaseTest idTest StorageTest \
               debugTest globalsTest OperationRouterTest RouterTest \
               client_socketTest customTest MonitorsTest \
               operationsTest serialnoTest newidTest TypeNodeTest \
               FormattedXMLWriterTest PropertyFactoryTest \
               PropertyManagerTest VariableTest AtlasStreamClientTest \
               ClientTaskTest utilsTest SystemTimeTest \
               TaskKitTest EntityKitTest ScriptKitTest atlas_helpersTest \
               ShakerTest CommSocketTest LinkTest composeTest

PHYSICS_TESTS = BBoxTest Vector3DTest QuaternionTest \
                transformTest CollisionTest emergenceTest distanceTest \
                ShapeTest CourseTest

MODULE_TESTS = LocationTest WorldTimeTest EntityRefTest DateTimeTest \
               TerrainContextTest

RULESETS_TESTS = LocatedEntityTest EntityTest PlantTest \
                 StackableTest ThingTest WorldTest \
                 CharacterTest CreatorTest ThingupdatePropertiesTest \
                 ContainerTest TaskTest EntityPropertyTest \
                 AllPropertyTest ScriptTest MotionTest AreaPropertyTest \
                 BBoxPropertyTest CalendarPropertyTest \
                 LinePropertyTest MindPropertyTest \
                 OutfitPropertyTest SolidPropertyTest \
                 StatisticsPropertyTest StatusPropertyTest \
                 TerrainModPropertyTest TerrainPropertyTest \
                 TransientPropertyTest TasksPropertyTest \
                 InternalPropertiesTest EntityPropertiesTest \
                 SpawnPropertyTest VisibilityPropertyTest \
                 ExternalPropertyTest BurnSpeedPropertyTest \
                 BiomassPropertyTest DecaysPropertyTest \
                 BulletDomainTest AtlasPropertiesTest \
                 SpawnerPropertyTest \
                 BaseMindTest MemEntityTest MemMapTest MovementTest \
                 PedestrianTest \
                 ExternalMindTest \
                 Python_APITest Py_QuaternionTest Py_Vector3DTest \
                 Py_Point3DTest Py_BBoxTest Py_LocationTest \
                 Py_RootEntityTest Py_OperationTest Py_OplistTest \
                 Py_ThingTest Py_MapTest Py_TaskTest \
                 Py_WorldTest Py_MessageTest Py_WorldTimeTest \
                 Py_PropertyTest Py_TerrainPropertyTest \
                 Py_TerrainModPropertyTest Py_ShapeTest \
                 Py_FilterTest \
                 PythonWrapperTest PythonEntityScriptTest \
                 MindFactoryTest PythonContextTest \
                 ArithmeticScriptTest PythonArithmeticScriptTest \
                 ArithmeticFactoryTest PythonArithmeticFactoryTest \
                 TerrainModTest PythonClassTest \
                 TerrainEffectorPropertyTest SuspendedPropertyTest \
                 EntityFilterTest EntityFilterParserTest \
                 EntityFilterProvidersTest TransformsPropertyTest \
                 ModePropertyTest ModeSpecPropertyTest

RULESETS_INTEGRATION_TESTS = TerrainPropertyintegration \
                             PropertyEntityintegration \
                             DecaysPropertyintegration \
                             BiomassPropertyintegration \
                             TerrainModPropertyintegration \
                             StatisticsPropertyintegration \
                             BaseMindMapEntityintegration \
                             AreaPropertyintegration \
                             SuspendedPropertyintegration \
                             ThingIntegration

CLIENT_TESTS = Py_CreatorClientTest Py_ObserverClientTest \
               ClientConnectionTest BaseClientTest \
               ClientPropertyManagerTest

CLIENT_INTEGRATION_TESTS = ClientConnectionintegration

SERVER_TESTS = RulesetTest EntityBuilderTest PropertyFlagTest \
               AccountTest AdminTest PlayerTest buildidTest \
               EntityFactoryTest TaskFactoryTest ConnectionTest \
               TrustedConnectionTest WorldRouterTest PeerTest LobbyTest \
               SpawnTest SpawnEntityTest ArithmeticBuilderTest \
               ServerRoutingTest \
               StorageManagerTest HttpCacheTest \
               ServerAccountTest TeleportAuthenticatorTest \
               TeleportStateTest PendingTeleportTest JunctureTest \
               ConnectableRouterTest RuleHandlerTest OpRuleHandlerTest \
               EntityRuleHandlerTest TaskRuleHandlerTest \
               PropertyRuleHandlerTest \
               IdleConnectorTest CommPSQLSocketTest \
               PersistenceTest \
               SystemAccountTest CorePropertyManagerTest

SERVER_COMM_TESTS = CommPeerTest \
                    CommMDNSPublisherTest

SERVER_INTEGRATION_TESTS = WorldRouterintegration Rulesetintegration \
                           Accountintegration \
                           EntityFactoryTypeNodeintegration \
                           AccountConnectionintegration \
                           AccountServerLobbyintegration \
                           ConnectionShakerintegration \
                           ConnectionCharacterintegration \
                           ConnectionCreatorintegration \
                           TrustedConnectionCreatorintegration \
                           AccountConnectionCharacterintegration

TOOLS_TESTS = AdminClientTest \
              FlusherTest OperationMonitorTest

PYTHON_TESTS = python_class

AM_CPPFLAGS = -w -I$(top_srcdir) -I$(top_builddir) \
           -DTESTDATADIR=\"$(abs_top_srcdir)/tests/data\"

LIBS = $(COMMON_LIBS) $(PGSQL_LIBS) $(PYTHON_LIBS) $(PYTHON_UTIL_LIBS) $(MATH_LIBS) pthreadforce.o

TESTS = $(TEST_TESTS) \
        $(COMMON_TESTS) $(PHYSICS_TESTS) $(MODULE_TESTS) $(RULESETS_TESTS) \
        $(SERVER_TESTS) $(SERVER_COMM_TESTS) $(CLIENT_TESTS) $(TOOLS_TESTS) \
        $(RULESETS_INTEGRATION_TESTS) $(SERVER_INTEGRATION_TESTS) \
        $(CLIENT_INTEGRATION_TESTS)

RECHECK_LOGS =

EXTRA_PROGRAMS = $(PYTHON_TESTS) MasterTest

check_PROGRAMS = $(TESTS)

noinst_HEADERS = TestBase.h null_stream.h \
                 OperationExerciser.h \
                 allOperations.h TestWorld.h \
                 Sink.h Property_stub_impl.h
dist-hook:
	(cd $(top_srcdir)/Tests && tar cf - data) | (cd $(distdir) && tar xf -)

#We want to build our object file which forces inclusion of pthreads
noinst_LIBRARIES = libpthreadforce.a libstubnavigation.a
libpthreadforce_a_SOURCES = pthreadforce.cpp

libstubnavigation_a_SOURCES = stubs/navigation/libstub.cpp

PYTHON_TESTS_LIBS = $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        libstubnavigation.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)


# TEST_TESTS

TestBaseTest_SOURCES = TestBaseTest.cpp

# COMMON_TESTS

logTest_SOURCES = logTest.cpp
logTest_LDADD = \
        $(top_builddir)/common/log.o

InheritanceTest_SOURCES = InheritanceTest.cpp
InheritanceTest_LDADD = \
        $(top_builddir)/common/Inheritance.o \
        $(top_builddir)/common/custom.o

PropertyTest_SOURCES = PropertyTest.cpp
PropertyTest_LDADD = $(top_builddir)/common/Property.o

systemTest_SOURCES = systemTest.cpp
systemTest_LDADD = \
        $(top_builddir)/common/system.o

system_netTest_SOURCES = system_netTest.cpp
system_netTest_LDADD = \
        $(top_builddir)/common/system_net.o

system_uidTest_SOURCES = system_uidTest.cpp
system_uidTest_LDADD = \
        $(top_builddir)/common/system_uid.o

type_utilsTest_SOURCES = type_utilsTest.cpp
type_utilsTest_LDADD = $(top_builddir)/common/type_utils.o

constTest_SOURCES = constTest.cpp
constTest_LDADD = $(top_builddir)/common/const.o

randomTest_SOURCES = randomTest.cpp

AddTest_SOURCES = AddTest.cpp OperationExerciser.cpp

AttackTest_SOURCES = AttackTest.cpp

BurnTest_SOURCES = BurnTest.cpp

ConnectTest_SOURCES = ConnectTest.cpp

DropTest_SOURCES = DropTest.cpp

EatTest_SOURCES = EatTest.cpp

MonitorTest_SOURCES = MonitorTest.cpp

NourishTest_SOURCES = NourishTest.cpp

PickupTest_SOURCES = PickupTest.cpp

SetupTest_SOURCES = SetupTest.cpp

TickTest_SOURCES = TickTest.cpp

UnseenTest_SOURCES = UnseenTest.cpp

UpdateTest_SOURCES = UpdateTest.cpp

AtlasFileLoaderTest_SOURCES = AtlasFileLoaderTest.cpp
AtlasFileLoaderTest_LDADD = \
        $(top_builddir)/common/AtlasFileLoader.o

BaseWorldTest_SOURCES = BaseWorldTest.cpp
BaseWorldTest_LDADD = \
        $(top_builddir)/common/BaseWorld.o

DatabaseTest_SOURCES = DatabaseTest.cpp
DatabaseTest_LDADD = \
        $(top_builddir)/common/Database.o

idTest_SOURCES = idTest.cpp
idTest_LDADD = \
        $(top_builddir)/common/id.o

StorageTest_SOURCES = StorageTest.cpp
StorageTest_LDADD = \
        $(top_builddir)/common/Storage.o

debugTest_SOURCES = debugTest.cpp
debugTest_LDADD = $(top_builddir)/common/debug.o

globalsTest_SOURCES = globalsTest.cpp
globalsTest_LDADD = \
        $(top_builddir)/common/globals.o

OperationRouterTest_SOURCES = OperationRouterTest.cpp
OperationRouterTest_LDADD = $(top_builddir)/common/OperationRouter.o

RouterTest_SOURCES = RouterTest.cpp
RouterTest_LDADD = \
        $(top_builddir)/common/Router.o

client_socketTest_SOURCES = client_socketTest.cpp
client_socketTest_LDADD = \
        $(top_builddir)/common/client_socket.o

customTest_SOURCES = customTest.cpp
customTest_LDADD = \
        $(top_builddir)/common/custom.o

MonitorsTest_SOURCES = MonitorsTest.cpp
MonitorsTest_LDADD = \
        $(top_builddir)/common/Monitors.o \
        $(top_builddir)/common/Variable.o

operationsTest_SOURCES = operationsTest.cpp
operationsTest_LDADD = $(top_builddir)/common/const.o

serialnoTest_SOURCES = serialnoTest.cpp

newidTest_SOURCES = newidTest.cpp
newidTest_LDADD = \
        $(top_builddir)/common/newid.o

TypeNodeTest_SOURCES = TypeNodeTest.cpp
TypeNodeTest_LDADD = \
        $(top_builddir)/common/TypeNode.o \
        $(top_builddir)/common/Property.o

FormattedXMLWriterTest_SOURCES = FormattedXMLWriterTest.cpp
FormattedXMLWriterTest_LDADD = $(top_builddir)/common/FormattedXMLWriter.o

PropertyFactoryTest_SOURCES = PropertyFactoryTest.cpp
PropertyFactoryTest_LDADD = \
        $(top_builddir)/common/PropertyFactory.o \
        $(top_builddir)/common/Property.o

PropertyManagerTest_SOURCES = PropertyManagerTest.cpp
PropertyManagerTest_LDADD = $(top_builddir)/common/PropertyManager.o

VariableTest_SOURCES = VariableTest.cpp
VariableTest_LDADD = \
        $(top_builddir)/common/Variable.o

AtlasStreamClientTest_SOURCES = AtlasStreamClientTest.cpp
AtlasStreamClientTest_LDADD = \
        $(top_builddir)/common/AtlasStreamClient.o \
        $(NETWORK_LIBS)

ClientTaskTest_SOURCES = ClientTaskTest.cpp
ClientTaskTest_LDADD = \
        $(top_builddir)/common/ClientTask.o

utilsTest_SOURCES = utilsTest.cpp
utilsTest_LDADD = \
        $(top_builddir)/common/utils.o

SystemTimeTest_SOURCES = SystemTimeTest.cpp
SystemTimeTest_LDADD = \
        $(top_builddir)/common/SystemTime.o

TaskKitTest_SOURCES = TaskKitTest.cpp
TaskKitTest_LDADD = \
        $(top_builddir)/common/TaskKit.o
        
ShakerTest_SOURCES = ShakerTest.cpp
ShakerTest_LDADD = \
        $(top_builddir)/common/Shaker.o

ScriptKitTest_SOURCES = ScriptKitTest.cpp
ScriptKitTest_LDADD = \
        $(top_builddir)/common/ScriptKit.o

EntityKitTest_SOURCES = EntityKitTest.cpp
EntityKitTest_LDADD = \
        $(top_builddir)/common/EntityKit.o

atlas_helpersTest_SOURCES = atlas_helpersTest.cpp
atlas_helpersTest_LDADD = \
        $(top_builddir)/common/atlas_helpers.o

LinkTest_SOURCES = LinkTest.cpp
LinkTest_LDADD = \
        $(top_builddir)/common/Link.o

CommSocketTest_SOURCES = CommSocketTest.cpp
CommSocketTest_LDADD = \
        $(top_builddir)/common/CommSocket.o

composeTest_SOURCES = composeTest.cpp

# PHYSICS_TESTS

BBoxTest_SOURCES = BBoxTest.cpp
BBoxTest_LDADD = \
        $(top_builddir)/physics/BBox.o \
        $(top_builddir)/common/const.o

Vector3DTest_SOURCES = Vector3DTest.cpp
Vector3DTest_LDADD = $(top_builddir)/physics/Vector3D.o

QuaternionTest_SOURCES = QuaternionTest.cpp
QuaternionTest_LDADD = $(top_builddir)/physics/Quaternion.o

CollisionTest_SOURCES = CollisionTest.cpp
CollisionTest_LDADD = \
        $(top_builddir)/physics/Collision.o \
        $(top_builddir)/physics/BBox.o \
        $(top_builddir)/physics/Vector3D.o \
        $(top_builddir)/modules/Location.o

emergenceTest_SOURCES = emergenceTest.cpp
emergenceTest_LDADD = \
        $(top_builddir)/physics/Collision.o \
        $(top_builddir)/physics/BBox.o \
        $(top_builddir)/physics/Vector3D.o \
        $(top_builddir)/modules/Location.o

transformTest_SOURCES = transformTest.cpp

distanceTest_SOURCES = distanceTest.cpp
distanceTest_LDADD = \
        $(top_builddir)/physics/Vector3D.o \
        $(top_builddir)/modules/Location.o

ShapeTest_SOURCES = ShapeTest.cpp
ShapeTest_LDADD = \
        $(top_builddir)/physics/Shape.o \
        $(top_builddir)/physics/Course.o

CourseTest_SOURCES = CourseTest.cpp
CourseTest_LDADD = \
        $(top_builddir)/physics/Course.o

# MODULE_TESTS

DateTimeTest_SOURCES = DateTimeTest.cpp
DateTimeTest_LDADD = $(top_builddir)/modules/DateTime.o

LocationTest_SOURCES = LocationTest.cpp
LocationTest_LDADD = \
        $(top_builddir)/modules/Location.o \
        $(top_builddir)/physics/libphysics.a

WorldTimeTest_SOURCES = WorldTimeTest.cpp
WorldTimeTest_LDADD = \
        $(top_builddir)/modules/WorldTime.o \
        $(top_builddir)/modules/DateTime.o

EntityRefTest_SOURCES = EntityRefTest.cpp
EntityRefTest_LDADD = \
        $(top_builddir)/modules/EntityRef.o

TerrainContextTest_SOURCES = TerrainContextTest.cpp
TerrainContextTest_LDADD = \
        $(top_builddir)/modules/TerrainContext.o \
        $(TERRAIN_LIBS)

# RULESETS_TESTS

LocatedEntityTest_SOURCES = LocatedEntityTest.cpp \
                            EntityExerciser.cpp EntityExerciser.h
LocatedEntityTest_LDADD = \
        $(top_builddir)/rulesets/LocatedEntity.o

EntityTest_SOURCES = EntityTest.cpp \
                     TestPropertyManager.cpp TestPropertyManager.h \
                     IGEntityExerciser.cpp IGEntityExerciser.h \
                     EntityExerciser.cpp EntityExerciser.h
EntityTest_LDADD = \
        $(top_builddir)/rulesets/Entity.o

PlantTest_SOURCES = PlantTest.cpp \
                    TestPropertyManager.cpp TestPropertyManager.h \
                    IGEntityExerciser.cpp IGEntityExerciser.h \
                    EntityExerciser.cpp EntityExerciser.h
PlantTest_LDADD = \
        $(top_builddir)/rulesets/Plant.o

StackableTest_SOURCES = StackableTest.cpp \
                        TestPropertyManager.cpp TestPropertyManager.h \
                        IGEntityExerciser.cpp IGEntityExerciser.h \
                        EntityExerciser.cpp EntityExerciser.h
StackableTest_LDADD = \
        $(top_builddir)/rulesets/Stackable.o

ThingTest_SOURCES = ThingTest.cpp \
                    TestPropertyManager.cpp TestPropertyManager.h \
                    IGEntityExerciser.cpp IGEntityExerciser.h \
                    EntityExerciser.cpp EntityExerciser.h
ThingTest_LDADD = \
        $(top_builddir)/rulesets/Thing.o

WorldTest_SOURCES = WorldTest.cpp \
                    TestPropertyManager.cpp TestPropertyManager.h \
                    IGEntityExerciser.cpp IGEntityExerciser.h \
                    EntityExerciser.cpp EntityExerciser.h
WorldTest_LDADD = \
        $(top_builddir)/rulesets/World.o

CharacterTest_SOURCES = CharacterTest.cpp \
                        TestPropertyManager.cpp TestPropertyManager.h \
                        IGEntityExerciser.cpp IGEntityExerciser.h \
                        EntityExerciser.cpp EntityExerciser.h
CharacterTest_LDADD = \
        $(top_builddir)/rulesets/Character.o \
        $(top_builddir)/physics/Quaternion.o

CreatorTest_SOURCES = CreatorTest.cpp \
                      TestPropertyManager.cpp TestPropertyManager.h \
                      IGEntityExerciser.cpp IGEntityExerciser.h \
                      EntityExerciser.cpp EntityExerciser.h
CreatorTest_LDADD = \
        $(top_builddir)/rulesets/Creator.o

ThingupdatePropertiesTest_SOURCES = ThingupdatePropertiesTest.cpp
ThingupdatePropertiesTest_LDADD = \
        $(top_builddir)/rulesets/Thing.o \
        $(top_builddir)/common/Property.o

ContainerTest_SOURCES = ContainerTest.cpp
ContainerTest_LDADD = $(top_builddir)/rulesets/Container.o

TaskTest_SOURCES = TaskTest.cpp
TaskTest_LDADD = \
        $(top_builddir)/rulesets/Task.o

EntityPropertyTest_SOURCES = EntityPropertyTest.cpp
EntityPropertyTest_LDADD = \
        $(top_builddir)/rulesets/EntityProperty.o \
        $(top_builddir)/modules/EntityRef.o \
        $(top_builddir)/common/Property.o

AllPropertyTest_SOURCES = AllPropertyTest.cpp \
                          PropertyExerciser.cpp PropertyExerciser.h
AllPropertyTest_LDADD = \
        $(top_builddir)/rulesets/AreaProperty.o \
        $(top_builddir)/rulesets/AtlasProperties.o \
        $(top_builddir)/rulesets/CalendarProperty.o \
        $(top_builddir)/rulesets/EntityProperty.o \
        $(top_builddir)/rulesets/LineProperty.o \
        $(top_builddir)/rulesets/OutfitProperty.o \
        $(top_builddir)/rulesets/StatisticsProperty.o \
        $(top_builddir)/rulesets/TerrainProperty.o \
        $(top_builddir)/rulesets/TerrainEffectorProperty.o \
        $(top_builddir)/modules/EntityRef.o \
        $(top_builddir)/modules/DateTime.o \
        $(top_builddir)/modules/TerrainContext.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/Inheritance.o \
        $(top_builddir)/common/custom.o \
        $(top_builddir)/common/operations.o \
        $(top_builddir)/physics/Shape.o \
        $(top_builddir)/physics/Course.o \
        $(TERRAIN_LIBS)

ScriptTest_SOURCES = ScriptTest.cpp
ScriptTest_LDADD = $(top_builddir)/rulesets/Script.o

MotionTest_SOURCES = MotionTest.cpp
MotionTest_LDADD = \
        $(top_builddir)/rulesets/Motion.o \
        $(top_builddir)/rulesets/PhysicalDomain.o \
        $(top_builddir)/physics/BBox.o \
        $(top_builddir)/physics/Collision.o

AreaPropertyTest_SOURCES = AreaPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
AreaPropertyTest_LDADD = \
        $(top_builddir)/rulesets/AreaProperty.o \
        $(top_builddir)/common/Property.o

BBoxPropertyTest_SOURCES = BBoxPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
BBoxPropertyTest_LDADD = \
        $(top_builddir)/rulesets/BBoxProperty.o \
        $(top_builddir)/common/Property.o

CalendarPropertyTest_SOURCES = CalendarPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
CalendarPropertyTest_LDADD = \
        $(top_builddir)/rulesets/CalendarProperty.o \
        $(top_builddir)/modules/DateTime.o \
        $(top_builddir)/common/Property.o

LinePropertyTest_SOURCES = LinePropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
LinePropertyTest_LDADD = \
        $(top_builddir)/rulesets/LineProperty.o \
        $(top_builddir)/common/Property.o

MindPropertyTest_SOURCES = MindPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
MindPropertyTest_LDADD = \
        $(top_builddir)/rulesets/MindProperty.o \
        $(top_builddir)/common/Property.o

ModePropertyTest_SOURCES = ModePropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
ModePropertyTest_LDADD = \
        $(top_builddir)/rulesets/ModeProperty.o \
        $(top_builddir)/common/Property.o
        
ModeSpecPropertyTest_SOURCES = ModeSpecPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
ModeSpecPropertyTest_LDADD = \
        $(top_builddir)/rulesets/ModeSpecProperty.o \
        $(top_builddir)/common/Property.o
                
OutfitPropertyTest_SOURCES = OutfitPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
OutfitPropertyTest_LDADD = \
        $(top_builddir)/rulesets/OutfitProperty.o \
        $(top_builddir)/modules/EntityRef.o \
        $(top_builddir)/common/Property.o

SolidPropertyTest_SOURCES = SolidPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
SolidPropertyTest_LDADD = \
        $(top_builddir)/rulesets/SolidProperty.o \
        $(top_builddir)/common/Property.o

StatisticsPropertyTest_SOURCES = StatisticsPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
StatisticsPropertyTest_LDADD = \
        $(top_builddir)/rulesets/StatisticsProperty.o \
        $(top_builddir)/common/Property.o

StatusPropertyTest_SOURCES = StatusPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
StatusPropertyTest_LDADD = \
        $(top_builddir)/rulesets/StatusProperty.o \
        $(top_builddir)/common/Property.o

SuspendedPropertyTest_SOURCES = SuspendedPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
SuspendedPropertyTest_LDADD = \
        $(top_builddir)/rulesets/SuspendedProperty.o \
        $(top_builddir)/common/Property.o
        
TerrainModPropertyTest_SOURCES = TerrainModPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TerrainModPropertyTest_LDADD = \
        $(top_builddir)/rulesets/TerrainModProperty.o \
        $(top_builddir)/rulesets/TerrainProperty.o \
        $(top_builddir)/common/Property.o \
        $(TERRAIN_LIBS)

TransformsPropertyTest_SOURCES = TransformsPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TransformsPropertyTest_LDADD = \
        $(top_builddir)/rulesets/TransformsProperty.o \
        $(top_builddir)/common/Property.o

PythonClassTest_SOURCES = PythonClassTest.cpp \
        python_testers.cpp python_testers.h
PythonClassTest_LDADD = \
        $(top_builddir)/rulesets/PythonClass.o

TerrainPropertyTest_SOURCES = TerrainPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TerrainPropertyTest_LDADD = \
        $(top_builddir)/rulesets/TerrainProperty.o \
        $(top_builddir)/common/Property.o \
        $(TERRAIN_LIBS)

TransientPropertyTest_SOURCES = TransientPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TransientPropertyTest_LDADD = \
        $(top_builddir)/rulesets/TransientProperty.o \
        $(top_builddir)/common/Property.o

TasksPropertyTest_SOURCES = TasksPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TasksPropertyTest_LDADD = \
        $(top_builddir)/rulesets/TasksProperty.o \
        $(top_builddir)/common/Property.o

InternalPropertiesTest_SOURCES = InternalPropertiesTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
InternalPropertiesTest_LDADD = \
        $(top_builddir)/rulesets/InternalProperties.o \
        $(top_builddir)/common/Property.o

EntityPropertiesTest_SOURCES = EntityPropertiesTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
EntityPropertiesTest_LDADD = \
        $(top_builddir)/rulesets/EntityProperties.o \
        $(top_builddir)/common/Property.o

AtlasPropertiesTest_SOURCES = AtlasPropertiesTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
AtlasPropertiesTest_LDADD = \
        $(top_builddir)/rulesets/AtlasProperties.o \
        $(top_builddir)/common/Property.o

SpawnPropertyTest_SOURCES = SpawnPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
SpawnPropertyTest_LDADD = \
        $(top_builddir)/rulesets/SpawnProperty.o \
        $(top_builddir)/common/Property.o

SpawnerPropertyTest_SOURCES = SpawnerPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
SpawnerPropertyTest_LDADD = \
        $(top_builddir)/rulesets/SpawnerProperty.o \
        $(top_builddir)/common/Property.o

VisibilityPropertyTest_SOURCES = VisibilityPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
VisibilityPropertyTest_LDADD = \
        $(top_builddir)/rulesets/VisibilityProperty.o \
        $(top_builddir)/common/Property.o

ExternalPropertyTest_SOURCES = ExternalPropertyTest.cpp
ExternalPropertyTest_LDADD = \
        $(top_builddir)/rulesets/ExternalProperty.o \
        $(top_builddir)/common/Property.o

BurnSpeedPropertyTest_SOURCES = BurnSpeedPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
BurnSpeedPropertyTest_LDADD = \
        $(top_builddir)/rulesets/BurnSpeedProperty.o \
        $(top_builddir)/common/Property.o

BiomassPropertyTest_SOURCES = BiomassPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
BiomassPropertyTest_LDADD = \
        $(top_builddir)/rulesets/BiomassProperty.o \
        $(top_builddir)/common/Property.o

DecaysPropertyTest_SOURCES = DecaysPropertyTest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
DecaysPropertyTest_LDADD = \
        $(top_builddir)/rulesets/DecaysProperty.o \
        $(top_builddir)/common/Property.o

BulletDomainTest_SOURCES = BulletDomainTest.cpp
BulletDomainTest_LDADD = \
        $(top_builddir)/rulesets/BulletDomain.o \
        $(TERRAIN_LIBS)

BaseMindTest_SOURCES = BaseMindTest.cpp
BaseMindTest_LDADD = \
        $(top_builddir)/rulesets/BaseMind.o \
        $(top_builddir)/rulesets/MemMap.o

MemEntityTest_SOURCES = MemEntityTest.cpp
MemEntityTest_LDADD = \
        $(top_builddir)/rulesets/MemEntity.o

MemMapTest_SOURCES = MemMapTest.cpp
MemMapTest_LDADD = \
        $(top_builddir)/rulesets/MemMap.o

MovementTest_SOURCES = MovementTest.cpp
MovementTest_LDADD = \
        $(top_builddir)/rulesets/Movement.o

PedestrianTest_SOURCES = PedestrianTest.cpp
PedestrianTest_LDADD = \
        $(top_builddir)/rulesets/Pedestrian.o \
        $(top_builddir)/rulesets/Movement.o

ExternalMindTest_SOURCES = ExternalMindTest.cpp
ExternalMindTest_LDADD = \
        $(top_builddir)/rulesets/ExternalMind.o

Python_APITest_SOURCES = Python_APITest.cpp \
        python_testers.cpp python_testers.h
Python_APITest_LDADD = $(PYTHON_TESTS_LIBS)

Py_QuaternionTest_SOURCES = Py_QuaternionTest.cpp \
        python_testers.cpp python_testers.h
Py_QuaternionTest_LDADD = $(PYTHON_TESTS_LIBS)
Py_Vector3DTest_SOURCES = Py_Vector3DTest.cpp \
        python_testers.cpp python_testers.h
Py_Vector3DTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_Point3DTest_SOURCES = Py_Point3DTest.cpp \
        python_testers.cpp python_testers.h
Py_Point3DTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_BBoxTest_SOURCES = Py_BBoxTest.cpp \
        python_testers.cpp python_testers.h
Py_BBoxTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_LocationTest_SOURCES = Py_LocationTest.cpp \
        python_testers.cpp python_testers.h
Py_LocationTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_RootEntityTest_SOURCES = Py_RootEntityTest.cpp \
        python_testers.cpp python_testers.h
Py_RootEntityTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_OperationTest_SOURCES = Py_OperationTest.cpp \
        python_testers.cpp python_testers.h
Py_OperationTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_OplistTest_SOURCES = Py_OplistTest.cpp \
        python_testers.cpp python_testers.h
Py_OplistTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_ThingTest_SOURCES = Py_ThingTest.cpp \
        TestPropertyManager.cpp TestPropertyManager.h \
        python_testers.cpp python_testers.h
Py_ThingTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_MapTest_SOURCES = Py_MapTest.cpp \
        python_testers.cpp python_testers.h
Py_MapTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_TaskTest_SOURCES = Py_TaskTest.cpp \
        python_testers.cpp python_testers.h
Py_TaskTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_WorldTest_SOURCES = Py_WorldTest.cpp \
        python_testers.cpp python_testers.h
Py_WorldTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_MessageTest_SOURCES = Py_MessageTest.cpp \
        python_testers.cpp python_testers.h
Py_MessageTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_WorldTimeTest_SOURCES = Py_WorldTimeTest.cpp \
        python_testers.cpp python_testers.h
Py_WorldTimeTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_PropertyTest_SOURCES = Py_PropertyTest.cpp \
        python_testers.cpp python_testers.h
Py_PropertyTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_TerrainPropertyTest_SOURCES = Py_TerrainPropertyTest.cpp \
        python_testers.cpp python_testers.h
Py_TerrainPropertyTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_TerrainModPropertyTest_SOURCES = Py_TerrainModPropertyTest.cpp \
        python_testers.cpp python_testers.h
Py_TerrainModPropertyTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_ShapeTest_SOURCES = Py_ShapeTest.cpp \
        python_testers.cpp python_testers.h
Py_ShapeTest_LDADD = $(PYTHON_TESTS_LIBS)

Py_FilterTest_SOURCES = Py_FilterTest.cpp \
		python_testers.cpp python_testers.h
Py_FilterTest_LDADD = $(PYTHON_TESTS_LIBS)

PythonWrapperTest_SOURCES = PythonWrapperTest.cpp
PythonWrapperTest_LDADD = \
        $(top_builddir)/rulesets/PythonWrapper.o

PythonEntityScriptTest_SOURCES = PythonEntityScriptTest.cpp \
        python_testers.cpp python_testers.h
PythonEntityScriptTest_LDADD = $(PYTHON_TESTS_LIBS)

MindFactoryTest_SOURCES = MindFactoryTest.cpp
MindFactoryTest_LDADD = \
        $(top_builddir)/rulesets/MindFactory.o

PythonContextTest_SOURCES = PythonContextTest.cpp
PythonContextTest_LDADD = \
        $(top_builddir)/rulesets/PythonContext.o

ArithmeticScriptTest_SOURCES = ArithmeticScriptTest.cpp
ArithmeticScriptTest_LDADD = \
        $(top_builddir)/rulesets/ArithmeticScript.o

PythonArithmeticScriptTest_SOURCES = PythonArithmeticScriptTest.cpp \
        python_testers.cpp python_testers.h
PythonArithmeticScriptTest_LDADD = \
        $(top_builddir)/rulesets/PythonArithmeticScript.o

ArithmeticFactoryTest_SOURCES = ArithmeticFactoryTest.cpp
ArithmeticFactoryTest_LDADD = \
        $(top_builddir)/rulesets/ArithmeticFactory.o

PythonArithmeticFactoryTest_SOURCES = PythonArithmeticFactoryTest.cpp \
        python_testers.cpp python_testers.h
PythonArithmeticFactoryTest_LDADD = \
        $(top_builddir)/rulesets/PythonArithmeticFactory.o \
        $(top_builddir)/rulesets/PythonClass.o

TerrainModTest_SOURCES = TerrainModTest.cpp
TerrainModTest_LDADD = \
        $(top_builddir)/rulesets/TerrainModTranslator.o \
        $(TERRAIN_LIBS)


TerrainEffectorPropertyTest_SOURCES = TerrainEffectorPropertyTest.cpp
TerrainEffectorPropertyTest_LDADD = \
        $(top_builddir)/rulesets/TerrainEffectorProperty.o \
        $(TERRAIN_LIBS)

EntityFilterTest_SOURCES = EntityFilterTest.cpp
EntityFilterTest_LDADD = \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/rulesets/EntityProperty.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/OutfitProperty.o \
        $(top_builddir)/rulesets/BBoxProperty.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/modules/EntityRef.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/TypeNode.o \
        $(top_builddir)/common/PropertyManager.o
        
EntityFilterParserTest_SOURCES = EntityFilterParserTest.cpp
EntityFilterParserTest_LDADD = \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/rulesets/EntityProperty.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/OutfitProperty.o \
        $(top_builddir)/rulesets/BBoxProperty.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/modules/EntityRef.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/TypeNode.o \
        $(top_builddir)/common/PropertyManager.o
        
EntityFilterProvidersTest_SOURCES = EntityFilterProvidersTest.cpp
EntityFilterProvidersTest_LDADD = \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/rulesets/EntityProperty.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/OutfitProperty.o \
        $(top_builddir)/rulesets/BBoxProperty.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/modules/EntityRef.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/TypeNode.o \
        $(top_builddir)/common/PropertyManager.o

# RULESETS_INTEGRATION

TerrainPropertyintegration_SOURCES = TerrainPropertyIntegration.cpp
TerrainPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/TerrainModProperty.o \
        $(top_builddir)/rulesets/TerrainEffectorProperty.o \
        $(top_builddir)/rulesets/TerrainProperty.o \
        $(top_builddir)/rulesets/TerrainModTranslator.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/rulesets/Domain.o \
        $(top_builddir)/rulesets/AtlasProperties.o \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/BaseWorld.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/PropertyManager.o \
        $(top_builddir)/common/Router.o \
        $(top_builddir)/common/operations.o \
        $(top_builddir)/common/id.o \
        $(TERRAIN_LIBS)

PropertyEntityintegration_SOURCES = PropertyEntityIntegration.cpp
PropertyEntityintegration_LDADD = \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/TypeNode.o

DecaysPropertyintegration_SOURCES = DecaysPropertyIntegration.cpp
DecaysPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/DecaysProperty.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/TypeNode.o

BiomassPropertyintegration_SOURCES = BiomassPropertyIntegration.cpp
BiomassPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/BiomassProperty.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/TypeNode.o

TerrainModPropertyintegration_SOURCES = TerrainModPropertyIntegration.cpp
TerrainModPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/rulesets/TerrainEffectorProperty.o \
        $(top_builddir)/rulesets/TerrainModProperty.o \
        $(top_builddir)/rulesets/TerrainModTranslator.o \
        $(top_builddir)/rulesets/TerrainProperty.o \
        $(top_builddir)/modules/EntityRef.o \
        $(top_builddir)/modules/TerrainContext.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/TypeNode.o \
        $(TERRAIN_LIBS)

StatisticsPropertyintegration_SOURCES = StatisticsPropertyIntegration.cpp
StatisticsPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/StatisticsProperty.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o

BaseMindMapEntityintegration_SOURCES = BaseMindMapEntityIntegration.cpp
BaseMindMapEntityintegration_LDADD = \
        $(top_builddir)/rulesets/BaseMind.o \
        $(top_builddir)/rulesets/MemEntity.o \
        $(top_builddir)/rulesets/MemMap.o \
        $(top_builddir)/rulesets/BBoxProperty.o \
        $(top_builddir)/rulesets/SolidProperty.o \
        $(top_builddir)/rulesets/InternalProperties.o \
        $(top_builddir)/modules/Location.o \
        $(top_builddir)/common/Property.o
        
        

AreaPropertyintegration_SOURCES = AreaPropertyIntegration.cpp
AreaPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/AreaProperty.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/physics/Course.o \
        $(top_builddir)/physics/Shape.o

SuspendedPropertyintegration_SOURCES = SuspendedPropertyIntegration.cpp
SuspendedPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/SuspendedProperty.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/common/libcommon.a
        
        
ThingIntegration_SOURCES = ThingIntegration.cpp
ThingIntegration_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/common/libcommon.a \
        $(top_builddir)/physics/libphysics.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)       
        
# CLIENT_TESTS

Py_CreatorClientTest_SOURCES = Py_CreatorClientTest.cpp \
        python_testers.cpp python_testers.h
Py_CreatorClientTest_LDADD = \
        $(top_builddir)/client/Py_CreatorClient.o \
        $(top_builddir)/client/Py_ObserverClient.o \
        $(top_builddir)/client/Python_ClientAPI.o \
        $(PYTHON_TESTS_LIBS) \
        $(NETWORK_LIBS)


Py_ObserverClientTest_SOURCES = Py_ObserverClientTest.cpp \
        python_testers.cpp python_testers.h
Py_ObserverClientTest_LDADD = \
        $(top_builddir)/client/Py_CreatorClient.o \
        $(top_builddir)/client/Py_ObserverClient.o \
        $(top_builddir)/client/Python_ClientAPI.o \
        $(PYTHON_TESTS_LIBS) \
         $(NETWORK_LIBS)

ClientConnectionTest_SOURCES = ClientConnectionTest.cpp
ClientConnectionTest_LDADD = \
        $(top_builddir)/client/ClientConnection.o $(NETWORK_LIBS)

BaseClientTest_SOURCES = BaseClientTest.cpp
BaseClientTest_LDADD = \
        $(top_builddir)/client/BaseClient.o $(NETWORK_LIBS)

ClientPropertyManagerTest_SOURCES = ClientPropertyManagerTest.cpp
ClientPropertyManagerTest_LDADD = \
        $(top_builddir)/client/ClientPropertyManager.o \
        $(top_builddir)/common/PropertyManager.o

# CLIENT_INTEGRATION_TESTS

ClientConnectionintegration_SOURCES = ClientConnectionIntegration.cpp
ClientConnectionintegration_LDADD = \
        $(top_builddir)/client/ClientConnection.o \
        $(top_builddir)/common/AtlasStreamClient.o \
        $(NETWORK_LIBS)

# SERVER_TESTS

RulesetTest_SOURCES = RulesetTest.cpp
RulesetTest_LDADD = \
        $(top_builddir)/server/Ruleset.o

EntityBuilderTest_SOURCES = EntityBuilderTest.cpp
EntityBuilderTest_LDADD = \
        $(top_builddir)/server/EntityBuilder.o

PropertyFlagTest_SOURCES = \
        PropertyFlagTest.cpp \
        TestPropertyManager.cpp
PropertyFlagTest_LDADD = \
        $(top_builddir)/rulesets/Thing.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/common/Property.o

AccountTest_SOURCES = AccountTest.cpp
AccountTest_LDADD = \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/common/debug.o

AdminTest_SOURCES = AdminTest.cpp
AdminTest_LDADD = \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/common/debug.o

ServerAccountTest_SOURCES = ServerAccountTest.cpp
ServerAccountTest_LDADD = \
        $(top_builddir)/server/ServerAccount.o

PlayerTest_SOURCES = PlayerTest.cpp
PlayerTest_LDADD = \
        $(top_builddir)/server/Player.o \
        $(top_builddir)/common/debug.o

EntityFactoryTest_SOURCES = EntityFactoryTest.cpp
EntityFactoryTest_LDADD = \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/common/EntityKit.o

TaskFactoryTest_SOURCES = TaskFactoryTest.cpp
TaskFactoryTest_LDADD = \
        $(top_builddir)/server/TaskFactory.o

buildidTest_SOURCES = buildidTest.cpp
buildidTest_LDADD = \
        $(top_builddir)/server/buildid.o

ConnectionTest_SOURCES = \
        ConnectionTest.cpp        
ConnectionTest_LDADD = \
        $(top_builddir)/server/Connection.o \
        $(NETWORK_LIBS)

TrustedConnectionTest_SOURCES = \
        TrustedConnectionTest.cpp
TrustedConnectionTest_LDADD = \
        $(top_builddir)/server/TrustedConnection.o \
        $(NETWORK_LIBS)

WorldRouterTest_SOURCES = WorldRouterTest.cpp
WorldRouterTest_LDADD = \
        $(top_builddir)/server/WorldRouter.o

PeerTest_SOURCES = \
        PeerTest.cpp 
PeerTest_LDADD = \
        $(top_builddir)/server/Peer.o \
        $(NETWORK_LIBS)

LobbyTest_SOURCES = LobbyTest.cpp
LobbyTest_LDADD = \
        $(top_builddir)/server/Lobby.o

SpawnTest_SOURCES = SpawnTest.cpp

SpawnEntityTest_SOURCES = SpawnEntityTest.cpp
SpawnEntityTest_LDADD = \
        $(top_builddir)/server/SpawnEntity.o

ArithmeticBuilderTest_SOURCES = ArithmeticBuilderTest.cpp
ArithmeticBuilderTest_LDADD = \
        $(top_builddir)/server/ArithmeticBuilder.o

ServerRoutingTest_SOURCES = ServerRoutingTest.cpp
ServerRoutingTest_LDADD = \
        $(top_builddir)/server/ServerRouting.o

StorageManagerTest_SOURCES = StorageManagerTest.cpp
StorageManagerTest_LDADD = \
        $(top_builddir)/server/StorageManager.o

HttpCacheTest_SOURCES = HttpCacheTest.cpp
HttpCacheTest_LDADD = \
        $(top_builddir)/server/HttpCache.o

# SERVER_COMM_TESTS

CommPeerTest_SOURCES = CommPeerTest.cpp
CommPeerTest_LDADD = \
        $(top_builddir)/server/CommPeer.o \
        $(NETWORK_LIBS)

CommMDNSPublisherTest_SOURCES = CommMDNSPublisherTest.cpp
CommMDNSPublisherTest_LDADD = \
        $(top_builddir)/server/CommMDNSPublisher.o $(NETWORK_LIBS)

TeleportAuthenticatorTest_SOURCES = TeleportAuthenticatorTest.cpp
TeleportAuthenticatorTest_LDADD = \
        $(top_builddir)/server/PossessionAuthenticator.o

TeleportStateTest_SOURCES = TeleportStateTest.cpp
TeleportStateTest_LDADD = \
        $(top_builddir)/server/TeleportState.o

PendingTeleportTest_SOURCES = PendingTeleportTest.cpp
PendingTeleportTest_LDADD = \
        $(top_builddir)/server/PendingPossession.o

JunctureTest_SOURCES = JunctureTest.cpp
JunctureTest_LDADD = \
        $(top_builddir)/server/Juncture.o \
        $(NETWORK_LIBS)

ConnectableRouterTest_SOURCES = ConnectableRouterTest.cpp
ConnectableRouterTest_LDADD = \
        $(top_builddir)/server/ConnectableRouter.o

RuleHandlerTest_SOURCES = RuleHandlerTest.cpp
RuleHandlerTest_LDADD = \
        $(top_builddir)/server/RuleHandler.o

OpRuleHandlerTest_SOURCES = OpRuleHandlerTest.cpp
OpRuleHandlerTest_LDADD = \
        $(top_builddir)/server/OpRuleHandler.o

EntityRuleHandlerTest_SOURCES = EntityRuleHandlerTest.cpp
EntityRuleHandlerTest_LDADD = \
        $(top_builddir)/server/EntityRuleHandler.o \
        $(top_builddir)/server/RuleHandler.o

TaskRuleHandlerTest_SOURCES = TaskRuleHandlerTest.cpp
TaskRuleHandlerTest_LDADD = \
        $(top_builddir)/server/TaskRuleHandler.o \
        $(top_builddir)/server/RuleHandler.o

PropertyRuleHandlerTest_SOURCES = PropertyRuleHandlerTest.cpp
PropertyRuleHandlerTest_LDADD = \
        $(top_builddir)/server/PropertyRuleHandler.o

IdleConnectorTest_SOURCES = IdleConnectorTest.cpp
IdleConnectorTest_LDADD = \
        $(top_builddir)/server/IdleConnector.o $(NETWORK_LIBS)

CommPSQLSocketTest_SOURCES = CommPSQLSocketTest.cpp
CommPSQLSocketTest_LDADD = \
        $(top_builddir)/server/CommPSQLSocket.o $(NETWORK_LIBS)

PersistenceTest_SOURCES = PersistenceTest.cpp
PersistenceTest_LDADD = \
        $(top_builddir)/server/Persistence.o

SystemAccountTest_SOURCES = SystemAccountTest.cpp
SystemAccountTest_LDADD = \
        $(top_builddir)/server/SystemAccount.o

CorePropertyManagerTest_SOURCES = CorePropertyManagerTest.cpp
CorePropertyManagerTest_LDADD = \
        $(top_builddir)/server/CorePropertyManager.o

# SERVER_INTEGRATION_TESTS

WorldRouterintegration_SOURCES = WorldRouterIntegration.cpp
WorldRouterintegration_LDADD = \
        $(top_builddir)/server/WorldRouter.o \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/TaskFactory.o \
        $(top_builddir)/server/SpawnEntity.o \
        $(top_builddir)/server/ArithmeticBuilder.o \
        $(top_builddir)/rulesets/Character.o \
        $(top_builddir)/rulesets/Domain.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/Thing.o \
        $(top_builddir)/rulesets/World.o \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a

Rulesetintegration_SOURCES = RulesetIntegration.cpp
Rulesetintegration_LDADD = \
        $(top_builddir)/server/Ruleset.o \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/TaskRuleHandler.o \
        $(top_builddir)/server/OpRuleHandler.o \
        $(top_builddir)/server/EntityRuleHandler.o \
        $(top_builddir)/server/ArchetypeRuleHandler.o \
        $(top_builddir)/server/PropertyRuleHandler.o \
        $(top_builddir)/server/RuleHandler.o \
        $(top_builddir)/server/Persistence.o \
        $(top_builddir)/server/TaskFactory.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)
Rulesetintegration_LDFLAGS = $(PYTHON_LINKER_FLAGS)

ConnectionShakerintegration_SOURCES = \
        ConnectionShakerIntegration.cpp
ConnectionShakerintegration_LDADD = \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/common/Shaker.o
        
ConnectionCharacterintegration_SOURCES = \
        ConnectionCharacterIntegration.cpp
ConnectionCharacterintegration_LDADD = \
        $(top_builddir)/physics/Quaternion.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/rulesets/Character.o \
        $(top_builddir)/rulesets/ExternalMind.o
        
ConnectionCreatorintegration_SOURCES = \
        ConnectionCreatorIntegration.cpp
ConnectionCreatorintegration_LDADD = \
        $(top_builddir)/physics/Quaternion.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/rulesets/Creator.o \
        $(top_builddir)/rulesets/Character.o \
        $(top_builddir)/rulesets/ExternalMind.o
        
TrustedConnectionCreatorintegration_SOURCES = \
        TrustedConnectionCreatorIntegration.cpp
TrustedConnectionCreatorintegration_LDADD = \
        $(top_builddir)/physics/Quaternion.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/rulesets/Creator.o \
        $(top_builddir)/rulesets/Character.o \
        $(top_builddir)/rulesets/ExternalMind.o
        
AccountConnectionCharacterintegration_SOURCES = \
        AccountConnectionCharacterIntegration.cpp
AccountConnectionCharacterintegration_LDADD = \
        $(top_builddir)/physics/Quaternion.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/rulesets/Character.o \
        $(top_builddir)/rulesets/ExternalMind.o
        
Accountintegration_SOURCES = \
        AccountIntegration.cpp
Accountintegration_LDADD = \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/server/PossessionAuthenticator.o \
        $(top_builddir)/server/PendingPossession.o \
        $(top_builddir)/server/WorldRouter.o \
        $(top_builddir)/server/SpawnEntity.o \
        $(top_builddir)/server/ConnectableRouter.o \
        $(top_builddir)/rulesets/Domain.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/ExternalMind.o \
        $(top_builddir)/rulesets/Character.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/rulesets/Task.o \
        $(top_builddir)/rulesets/Thing.o \
        $(top_builddir)/rulesets/World.o \
        $(top_builddir)/modules/DateTime.o \
        $(top_builddir)/modules/EntityRef.o \
        $(top_builddir)/modules/Location.o \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/BaseWorld.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/id.o \
        $(top_builddir)/common/Link.o \
        $(top_builddir)/common/PropertyManager.o \
        $(top_builddir)/common/Router.o \
        $(TERRAIN_LIBS)

EntityFactoryTypeNodeintegration_SOURCES = \
        EntityFactoryTypeNodeIntegration.cpp \
        TestPropertyManager.cpp
EntityFactoryTypeNodeintegration_LDADD = \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/common/EntityKit.o \
        $(top_builddir)/common/TypeNode.o \
        $(top_builddir)/common/PropertyManager.o \
        $(top_builddir)/common/Property.o

AccountConnectionintegration_SOURCES = \
        AccountConnectionIntegration.cpp
AccountConnectionintegration_LDADD = \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/server/ConnectableRouter.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/server/Player.o \
        $(top_builddir)/server/ServerAccount.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/SystemAccount.o \
        $(top_builddir)/common/BaseWorld.o \
        $(top_builddir)/common/Inheritance.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/Router.o \
        $(top_builddir)/common/TypeNode.o \
        $(top_builddir)/common/custom.o \
        $(top_builddir)/common/operations.o \
        $(top_builddir)/physics/libphysics.a \
        $(NETWORK_LIBS)

AccountServerLobbyintegration_SOURCES = \
        AccountServerLobbyIntegration.cpp
AccountServerLobbyintegration_LDADD = \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/Lobby.o

# Other TESTS

MasterTest_SOURCES = MasterTest.cpp
MasterTest_LDADD = \
        $(top_builddir)/server/Master.o \
        $(top_builddir)/common/libcommon.a


# TOOLS_TESTS

AdminClientTest_SOURCES = AdminClientTest.cpp
AdminClientTest_LDADD = \
        $(top_builddir)/tools/AdminClient.o $(NETWORK_LIBS)

FlusherTest_SOURCES = FlusherTest.cpp
FlusherTest_LDADD = \
        $(top_builddir)/tools/Flusher.o \
        $(top_builddir)/common/ClientTask.o

OperationMonitorTest_SOURCES = OperationMonitorTest.cpp
OperationMonitorTest_LDADD = \
        $(top_builddir)/tools/OperationMonitor.o \
        $(top_builddir)/common/ClientTask.o

# PYTHON_TESTS

python_class_SOURCES = python_class.cpp
python_class_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a
