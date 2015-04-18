TEST_TESTS = TestBasetest

COMMON_TESTS = logtest Inheritancetest Propertytest \
               systemtest system_nettest system_uidtest \
               type_utilstest \
               consttest randomtest Addtest Attacktest Burntest \
               Connecttest Droptest Eattest \
               Monitortest Nourishtest Pickuptest Setuptest \
               Ticktest Unseentest Updatetest AtlasFileLoadertest \
               BaseWorldtest Databasetest idtest Storagetest \
               debugtest globalstest OperationRoutertest Routertest \
               client_sockettest customtest Monitorstest \
               operationstest serialnotest newidtest TypeNodetest \
               FormattedXMLWritertest PropertyFactorytest \
               PropertyManagertest Variabletest AtlasStreamClienttest \
               ClientTasktest utilstest SystemTimetest \
               TaskKittest EntityKittest ScriptKittest atlas_helperstest \
               Shakertest CommSockettest Linktest composetest

PHYSICS_TESTS = BBoxtest Vector3Dtest Quaterniontest \
                transformtest Collisiontest emergencetest distancetest \
                Shapetest Coursetest

MODULE_TESTS = Locationtest WorldTimetest EntityReftest DateTimetest \
               TerrainContexttest

RULESETS_TESTS = LocatedEntitytest Entitytest Planttest \
                 Stackabletest Thingtest Worldtest \
                 Charactertest Creatortest ThingupdatePropertiestest \
                 Containertest Tasktest EntityPropertytest \
                 AllPropertytest Scripttest Motiontest AreaPropertytest \
                 BBoxPropertytest CalendarPropertytest \
                 LinePropertytest MindPropertytest \
                 OutfitPropertytest SolidPropertytest \
                 StatisticsPropertytest StatusPropertytest \
                 TerrainModPropertytest TerrainPropertytest \
                 TransientPropertytest TasksPropertytest \
                 InternalPropertiestest EntityPropertiestest \
                 SpawnPropertytest VisibilityPropertytest \
                 ExternalPropertytest BurnSpeedPropertytest \
                 BiomassPropertytest DecaysPropertytest \
                 BulletDomaintest AtlasPropertiestest \
                 SpawnerPropertytest \
                 BaseMindtest MemEntitytest MemMaptest Movementtest \
                 Pedestriantest \
                 ExternalMindtest \
                 Python_APItest Py_Quaterniontest Py_Vector3Dtest \
                 Py_Point3Dtest Py_BBoxtest Py_Locationtest \
                 Py_RootEntitytest Py_Operationtest Py_Oplisttest \
                 Py_Thingtest Py_Maptest Py_Tasktest \
                 Py_Worldtest Py_Messagetest Py_WorldTimetest \
                 Py_Propertytest Py_TerrainPropertytest \
                 Py_TerrainModPropertytest Py_Shapetest \
                 Py_Filtertest \
                 PythonWrappertest PythonEntityScripttest \
                 MindFactorytest PythonContexttest \
                 ArithmeticScripttest PythonArithmeticScripttest \
                 ArithmeticFactorytest PythonArithmeticFactorytest \
                 TerrainModtest PythonClasstest \
                 TerrainEffectorPropertytest SuspendedPropertytest \
                 EntityFiltertest

RULESETS_INTEGRATION_TESTS = MindPropertyintegration \
                             TerrainPropertyintegration \
                             PropertyEntityintegration \
                             DecaysPropertyintegration \
                             BiomassPropertyintegration \
                             TerrainModPropertyintegration \
                             StatisticsPropertyintegration \
                             BaseMindMapEntityintegration \
                             AreaPropertyintegration \
                             SuspendedPropertyintegration

CLIENT_TESTS = Py_CreatorClienttest Py_ObserverClienttest \
               ClientConnectiontest BaseClienttest \
               ClientPropertyManagertest

CLIENT_INTEGRATION_TESTS = ClientConnectionintegration

SERVER_TESTS = Rulesettest EntityBuildertest PropertyFlagtest \
               Accounttest Admintest Playertest buildidtest \
               EntityFactorytest TaskFactorytest Connectiontest \
               TrustedConnectiontest WorldRoutertest Peertest Lobbytest \
               Spawntest SpawnEntitytest ArithmeticBuildertest \
               ServerRoutingtest \
               StorageManagertest HttpCachetest \
               ServerAccounttest TeleportAuthenticatortest \
               TeleportStatetest PendingTeleporttest Juncturetest \
               ConnectableRoutertest RuleHandlertest OpRuleHandlertest \
               EntityRuleHandlertest TaskRuleHandlertest \
               PropertyRuleHandlertest \
               IdleConnectortest CommPSQLSockettest \
               Persistencetest \
               SystemAccounttest CorePropertyManagertest

SERVER_COMM_TESTS = CommPeertest \
                    CommMDNSPublishertest

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

TOOLS_TESTS = AdminClienttest \
              Flushertest OperationMonitortest \
              MultiLineListFormattertest

PYTHON_TESTS = python_class

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir) \
           -DTESTDATADIR=\"$(abs_top_srcdir)/tests/data\"

LIBS = $(COMMON_LIBS) $(PGSQL_LIBS) $(PYTHON_LIBS) $(PYTHON_UTIL_LIBS) $(MATH_LIBS) pthreadforce.o

TESTS = $(TEST_TESTS) \
        $(COMMON_TESTS) $(PHYSICS_TESTS) $(MODULE_TESTS) $(RULESETS_TESTS) \
        $(SERVER_TESTS) $(SERVER_COMM_TESTS) $(CLIENT_TESTS) $(TOOLS_TESTS) \
        $(RULESETS_INTEGRATION_TESTS) $(SERVER_INTEGRATION_TESTS) \
        $(CLIENT_INTEGRATION_TESTS)

RECHECK_LOGS =

EXTRA_PROGRAMS = $(PYTHON_TESTS) Mastertest

check_PROGRAMS = $(TESTS)

noinst_HEADERS = TestBase.h null_stream.h \
                 OperationExerciser.h \
                 allOperations.h TestWorld.h \
                 Sink.h Property_stub_impl.h
dist-hook:
	(cd $(top_srcdir)/tests && tar cf - data) | (cd $(distdir) && tar xf -)

#We want to build our object file which forces inclusion of pthreads
noinst_LIBRARIES = libpthreadforce.a
libpthreadforce_a_SOURCES = pthreadforce.cpp

# TEST_TESTS

TestBasetest_SOURCES = TestBasetest.cpp

# COMMON_TESTS

logtest_SOURCES = logtest.cpp
logtest_LDADD = \
        $(top_builddir)/common/log.o

Inheritancetest_SOURCES = Inheritancetest.cpp
Inheritancetest_LDADD = \
        $(top_builddir)/common/Inheritance.o \
        $(top_builddir)/common/custom.o

Propertytest_SOURCES = Propertytest.cpp
Propertytest_LDADD = $(top_builddir)/common/Property.o

systemtest_SOURCES = systemtest.cpp
systemtest_LDADD = \
        $(top_builddir)/common/system.o

system_nettest_SOURCES = system_nettest.cpp
system_nettest_LDADD = \
        $(top_builddir)/common/system_net.o

system_uidtest_SOURCES = system_uidtest.cpp
system_uidtest_LDADD = \
        $(top_builddir)/common/system_uid.o

type_utilstest_SOURCES = type_utilstest.cpp
type_utilstest_LDADD = $(top_builddir)/common/type_utils.o

consttest_SOURCES = consttest.cpp
consttest_LDADD = $(top_builddir)/common/const.o

randomtest_SOURCES = randomtest.cpp

Addtest_SOURCES = Addtest.cpp OperationExerciser.cpp

Attacktest_SOURCES = Attacktest.cpp

Burntest_SOURCES = Burntest.cpp

Connecttest_SOURCES = Connecttest.cpp

Droptest_SOURCES = Droptest.cpp

Eattest_SOURCES = Eattest.cpp

Monitortest_SOURCES = Monitortest.cpp

Nourishtest_SOURCES = Nourishtest.cpp

Pickuptest_SOURCES = Pickuptest.cpp

Setuptest_SOURCES = Setuptest.cpp

Ticktest_SOURCES = Ticktest.cpp

Unseentest_SOURCES = Unseentest.cpp

Updatetest_SOURCES = Updatetest.cpp

AtlasFileLoadertest_SOURCES = AtlasFileLoadertest.cpp
AtlasFileLoadertest_LDADD = \
        $(top_builddir)/common/AtlasFileLoader.o

BaseWorldtest_SOURCES = BaseWorldtest.cpp
BaseWorldtest_LDADD = \
        $(top_builddir)/common/BaseWorld.o

Databasetest_SOURCES = Databasetest.cpp
Databasetest_LDADD = \
        $(top_builddir)/common/Database.o

idtest_SOURCES = idtest.cpp
idtest_LDADD = \
        $(top_builddir)/common/id.o

Storagetest_SOURCES = Storagetest.cpp
Storagetest_LDADD = \
        $(top_builddir)/common/Storage.o

debugtest_SOURCES = debugtest.cpp
debugtest_LDADD = $(top_builddir)/common/debug.o

globalstest_SOURCES = globalstest.cpp
globalstest_LDADD = \
        $(top_builddir)/common/globals.o

OperationRoutertest_SOURCES = OperationRoutertest.cpp
OperationRoutertest_LDADD = $(top_builddir)/common/OperationRouter.o

Routertest_SOURCES = Routertest.cpp
Routertest_LDADD = \
        $(top_builddir)/common/Router.o

client_sockettest_SOURCES = client_sockettest.cpp
client_sockettest_LDADD = \
        $(top_builddir)/common/client_socket.o

customtest_SOURCES = customtest.cpp
customtest_LDADD = \
        $(top_builddir)/common/custom.o

Monitorstest_SOURCES = Monitorstest.cpp
Monitorstest_LDADD = \
        $(top_builddir)/common/Monitors.o \
        $(top_builddir)/common/Variable.o

operationstest_SOURCES = operationstest.cpp
operationstest_LDADD = $(top_builddir)/common/const.o

serialnotest_SOURCES = serialnotest.cpp

newidtest_SOURCES = newidtest.cpp
newidtest_LDADD = \
        $(top_builddir)/common/newid.o

TypeNodetest_SOURCES = TypeNodetest.cpp
TypeNodetest_LDADD = \
        $(top_builddir)/common/TypeNode.o \
        $(top_builddir)/common/Property.o

FormattedXMLWritertest_SOURCES = FormattedXMLWritertest.cpp
FormattedXMLWritertest_LDADD = $(top_builddir)/common/FormattedXMLWriter.o

PropertyFactorytest_SOURCES = PropertyFactorytest.cpp
PropertyFactorytest_LDADD = \
        $(top_builddir)/common/PropertyFactory.o \
        $(top_builddir)/common/Property.o

PropertyManagertest_SOURCES = PropertyManagertest.cpp
PropertyManagertest_LDADD = $(top_builddir)/common/PropertyManager.o

Variabletest_SOURCES = Variabletest.cpp
Variabletest_LDADD = \
        $(top_builddir)/common/Variable.o

AtlasStreamClienttest_SOURCES = AtlasStreamClienttest.cpp
AtlasStreamClienttest_LDADD = \
        $(top_builddir)/common/AtlasStreamClient.o \
        $(NETWORK_LIBS)

ClientTasktest_SOURCES = ClientTasktest.cpp
ClientTasktest_LDADD = \
        $(top_builddir)/common/ClientTask.o

utilstest_SOURCES = utilstest.cpp
utilstest_LDADD = \
        $(top_builddir)/common/utils.o

SystemTimetest_SOURCES = SystemTimetest.cpp
SystemTimetest_LDADD = \
        $(top_builddir)/common/SystemTime.o

TaskKittest_SOURCES = TaskKittest.cpp
TaskKittest_LDADD = \
        $(top_builddir)/common/TaskKit.o
        
Shakertest_SOURCES = Shakertest.cpp
Shakertest_LDADD = \
        $(top_builddir)/common/Shaker.o

ScriptKittest_SOURCES = ScriptKittest.cpp
ScriptKittest_LDADD = \
        $(top_builddir)/common/ScriptKit.o

EntityKittest_SOURCES = EntityKittest.cpp
EntityKittest_LDADD = \
        $(top_builddir)/common/EntityKit.o

atlas_helperstest_SOURCES = atlas_helperstest.cpp
atlas_helperstest_LDADD = \
        $(top_builddir)/common/atlas_helpers.o

Linktest_SOURCES = Linktest.cpp
Linktest_LDADD = \
        $(top_builddir)/common/Link.o

CommSockettest_SOURCES = CommSockettest.cpp
CommSockettest_LDADD = \
        $(top_builddir)/common/CommSocket.o

composetest_SOURCES = composetest.cpp

# PHYSICS_TESTS

BBoxtest_SOURCES = BBoxtest.cpp
BBoxtest_LDADD = \
        $(top_builddir)/physics/BBox.o \
        $(top_builddir)/common/const.o

Vector3Dtest_SOURCES = Vector3Dtest.cpp
Vector3Dtest_LDADD = $(top_builddir)/physics/Vector3D.o

Quaterniontest_SOURCES = Quaterniontest.cpp
Quaterniontest_LDADD = $(top_builddir)/physics/Quaternion.o

Collisiontest_SOURCES = Collisiontest.cpp
Collisiontest_LDADD = \
        $(top_builddir)/physics/Collision.o \
        $(top_builddir)/physics/BBox.o \
        $(top_builddir)/physics/Vector3D.o \
        $(top_builddir)/modules/Location.o

emergencetest_SOURCES = emergencetest.cpp
emergencetest_LDADD = \
        $(top_builddir)/physics/Collision.o \
        $(top_builddir)/physics/BBox.o \
        $(top_builddir)/physics/Vector3D.o \
        $(top_builddir)/modules/Location.o

transformtest_SOURCES = transformtest.cpp

distancetest_SOURCES = distancetest.cpp
distancetest_LDADD = \
        $(top_builddir)/physics/Vector3D.o \
        $(top_builddir)/modules/Location.o

Shapetest_SOURCES = Shapetest.cpp
Shapetest_LDADD = \
        $(top_builddir)/physics/Shape.o \
        $(top_builddir)/physics/Course.o

Coursetest_SOURCES = Coursetest.cpp
Coursetest_LDADD = \
        $(top_builddir)/physics/Course.o

# MODULE_TESTS

DateTimetest_SOURCES = DateTimetest.cpp
DateTimetest_LDADD = $(top_builddir)/modules/DateTime.o

Locationtest_SOURCES = Locationtest.cpp
Locationtest_LDADD = \
        $(top_builddir)/modules/Location.o \
        $(top_builddir)/physics/libphysics.a

WorldTimetest_SOURCES = WorldTimetest.cpp
WorldTimetest_LDADD = \
        $(top_builddir)/modules/WorldTime.o \
        $(top_builddir)/modules/DateTime.o

EntityReftest_SOURCES = EntityReftest.cpp
EntityReftest_LDADD = \
        $(top_builddir)/modules/EntityRef.o

TerrainContexttest_SOURCES = TerrainContexttest.cpp
TerrainContexttest_LDADD = \
        $(top_builddir)/modules/TerrainContext.o \
        $(TERRAIN_LIBS)

# RULESETS_TESTS

LocatedEntitytest_SOURCES = LocatedEntitytest.cpp \
                            EntityExerciser.cpp EntityExerciser.h
LocatedEntitytest_LDADD = \
        $(top_builddir)/rulesets/LocatedEntity.o

Entitytest_SOURCES = Entitytest.cpp \
                     TestPropertyManager.cpp TestPropertyManager.h \
                     IGEntityExerciser.cpp IGEntityExerciser.h \
                     EntityExerciser.cpp EntityExerciser.h
Entitytest_LDADD = \
        $(top_builddir)/rulesets/Entity.o

Planttest_SOURCES = Planttest.cpp \
                    TestPropertyManager.cpp TestPropertyManager.h \
                    IGEntityExerciser.cpp IGEntityExerciser.h \
                    EntityExerciser.cpp EntityExerciser.h
Planttest_LDADD = \
        $(top_builddir)/rulesets/Plant.o

Stackabletest_SOURCES = Stackabletest.cpp \
                        TestPropertyManager.cpp TestPropertyManager.h \
                        IGEntityExerciser.cpp IGEntityExerciser.h \
                        EntityExerciser.cpp EntityExerciser.h
Stackabletest_LDADD = \
        $(top_builddir)/rulesets/Stackable.o

Thingtest_SOURCES = Thingtest.cpp \
                    TestPropertyManager.cpp TestPropertyManager.h \
                    IGEntityExerciser.cpp IGEntityExerciser.h \
                    EntityExerciser.cpp EntityExerciser.h
Thingtest_LDADD = \
        $(top_builddir)/rulesets/Thing.o

Worldtest_SOURCES = Worldtest.cpp \
                    TestPropertyManager.cpp TestPropertyManager.h \
                    IGEntityExerciser.cpp IGEntityExerciser.h \
                    EntityExerciser.cpp EntityExerciser.h
Worldtest_LDADD = \
        $(top_builddir)/rulesets/World.o

Charactertest_SOURCES = Charactertest.cpp \
                        TestPropertyManager.cpp TestPropertyManager.h \
                        IGEntityExerciser.cpp IGEntityExerciser.h \
                        EntityExerciser.cpp EntityExerciser.h
Charactertest_LDADD = \
        $(top_builddir)/rulesets/Character.o

Creatortest_SOURCES = Creatortest.cpp \
                      TestPropertyManager.cpp TestPropertyManager.h \
                      IGEntityExerciser.cpp IGEntityExerciser.h \
                      EntityExerciser.cpp EntityExerciser.h
Creatortest_LDADD = \
        $(top_builddir)/rulesets/Creator.o

ThingupdatePropertiestest_SOURCES = ThingupdatePropertiestest.cpp
ThingupdatePropertiestest_LDADD = \
        $(top_builddir)/rulesets/Thing.o \
        $(top_builddir)/common/Property.o

Containertest_SOURCES = Containertest.cpp
Containertest_LDADD = $(top_builddir)/rulesets/Container.o

Tasktest_SOURCES = Tasktest.cpp
Tasktest_LDADD = \
        $(top_builddir)/rulesets/Task.o

EntityPropertytest_SOURCES = EntityPropertytest.cpp
EntityPropertytest_LDADD = \
        $(top_builddir)/rulesets/EntityProperty.o \
        $(top_builddir)/modules/EntityRef.o \
        $(top_builddir)/common/Property.o

AllPropertytest_SOURCES = AllPropertytest.cpp \
                          PropertyExerciser.cpp PropertyExerciser.h
AllPropertytest_LDADD = \
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

Scripttest_SOURCES = Scripttest.cpp
Scripttest_LDADD = $(top_builddir)/rulesets/Script.o

Motiontest_SOURCES = Motiontest.cpp
Motiontest_LDADD = \
        $(top_builddir)/rulesets/Motion.o \
        $(top_builddir)/rulesets/PhysicalDomain.o \
        $(top_builddir)/physics/BBox.o \
        $(top_builddir)/physics/Collision.o

AreaPropertytest_SOURCES = AreaPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
AreaPropertytest_LDADD = \
        $(top_builddir)/rulesets/AreaProperty.o \
        $(top_builddir)/common/Property.o

BBoxPropertytest_SOURCES = BBoxPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
BBoxPropertytest_LDADD = \
        $(top_builddir)/rulesets/BBoxProperty.o \
        $(top_builddir)/common/Property.o

CalendarPropertytest_SOURCES = CalendarPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
CalendarPropertytest_LDADD = \
        $(top_builddir)/rulesets/CalendarProperty.o \
        $(top_builddir)/modules/DateTime.o \
        $(top_builddir)/common/Property.o

LinePropertytest_SOURCES = LinePropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
LinePropertytest_LDADD = \
        $(top_builddir)/rulesets/LineProperty.o \
        $(top_builddir)/common/Property.o

MindPropertytest_SOURCES = MindPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
MindPropertytest_LDADD = \
        $(top_builddir)/rulesets/MindProperty.o \
        $(top_builddir)/common/Property.o

OutfitPropertytest_SOURCES = OutfitPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
OutfitPropertytest_LDADD = \
        $(top_builddir)/rulesets/OutfitProperty.o \
        $(top_builddir)/modules/EntityRef.o \
        $(top_builddir)/common/Property.o

SolidPropertytest_SOURCES = SolidPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
SolidPropertytest_LDADD = \
        $(top_builddir)/rulesets/SolidProperty.o \
        $(top_builddir)/common/Property.o

StatisticsPropertytest_SOURCES = StatisticsPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
StatisticsPropertytest_LDADD = \
        $(top_builddir)/rulesets/StatisticsProperty.o \
        $(top_builddir)/common/Property.o

StatusPropertytest_SOURCES = StatusPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
StatusPropertytest_LDADD = \
        $(top_builddir)/rulesets/StatusProperty.o \
        $(top_builddir)/common/Property.o

SuspendedPropertytest_SOURCES = SuspendedPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
SuspendedPropertytest_LDADD = \
        $(top_builddir)/rulesets/SuspendedProperty.o \
        $(top_builddir)/common/Property.o
        
TerrainModPropertytest_SOURCES = TerrainModPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TerrainModPropertytest_LDADD = \
        $(top_builddir)/rulesets/TerrainModProperty.o \
        $(top_builddir)/rulesets/TerrainProperty.o \
        $(top_builddir)/common/Property.o \
        $(TERRAIN_LIBS)

PythonClasstest_SOURCES = PythonClasstest.cpp \
        python_testers.cpp python_testers.h
PythonClasstest_LDADD = \
        $(top_builddir)/rulesets/PythonClass.o

TerrainPropertytest_SOURCES = TerrainPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TerrainPropertytest_LDADD = \
        $(top_builddir)/rulesets/TerrainProperty.o \
        $(top_builddir)/common/Property.o \
        $(TERRAIN_LIBS)

TransientPropertytest_SOURCES = TransientPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TransientPropertytest_LDADD = \
        $(top_builddir)/rulesets/TransientProperty.o \
        $(top_builddir)/common/Property.o

TasksPropertytest_SOURCES = TasksPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TasksPropertytest_LDADD = \
        $(top_builddir)/rulesets/TasksProperty.o \
        $(top_builddir)/common/Property.o

InternalPropertiestest_SOURCES = InternalPropertiestest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
InternalPropertiestest_LDADD = \
        $(top_builddir)/rulesets/InternalProperties.o \
        $(top_builddir)/common/Property.o

EntityPropertiestest_SOURCES = EntityPropertiestest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
EntityPropertiestest_LDADD = \
        $(top_builddir)/rulesets/EntityProperties.o \
        $(top_builddir)/common/Property.o

AtlasPropertiestest_SOURCES = AtlasPropertiestest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
AtlasPropertiestest_LDADD = \
        $(top_builddir)/rulesets/AtlasProperties.o \
        $(top_builddir)/common/Property.o

SpawnPropertytest_SOURCES = SpawnPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
SpawnPropertytest_LDADD = \
        $(top_builddir)/rulesets/SpawnProperty.o \
        $(top_builddir)/common/Property.o

SpawnerPropertytest_SOURCES = SpawnerPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
SpawnerPropertytest_LDADD = \
        $(top_builddir)/rulesets/SpawnerProperty.o \
        $(top_builddir)/common/Property.o

VisibilityPropertytest_SOURCES = VisibilityPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
VisibilityPropertytest_LDADD = \
        $(top_builddir)/rulesets/VisibilityProperty.o \
        $(top_builddir)/common/Property.o

ExternalPropertytest_SOURCES = ExternalPropertytest.cpp
ExternalPropertytest_LDADD = \
        $(top_builddir)/rulesets/ExternalProperty.o \
        $(top_builddir)/common/Property.o

BurnSpeedPropertytest_SOURCES = BurnSpeedPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
BurnSpeedPropertytest_LDADD = \
        $(top_builddir)/rulesets/BurnSpeedProperty.o \
        $(top_builddir)/common/Property.o

BiomassPropertytest_SOURCES = BiomassPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
BiomassPropertytest_LDADD = \
        $(top_builddir)/rulesets/BiomassProperty.o \
        $(top_builddir)/common/Property.o

DecaysPropertytest_SOURCES = DecaysPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
DecaysPropertytest_LDADD = \
        $(top_builddir)/rulesets/DecaysProperty.o \
        $(top_builddir)/common/Property.o

BulletDomaintest_SOURCES = BulletDomaintest.cpp
BulletDomaintest_LDADD = \
        $(top_builddir)/rulesets/BulletDomain.o \
        $(TERRAIN_LIBS)

BaseMindtest_SOURCES = BaseMindtest.cpp
BaseMindtest_LDADD = \
        $(top_builddir)/rulesets/BaseMind.o \
        $(top_builddir)/rulesets/MemMap.o

MemEntitytest_SOURCES = MemEntitytest.cpp
MemEntitytest_LDADD = \
        $(top_builddir)/rulesets/MemEntity.o

MemMaptest_SOURCES = MemMaptest.cpp
MemMaptest_LDADD = \
        $(top_builddir)/rulesets/MemMap.o

Movementtest_SOURCES = Movementtest.cpp
Movementtest_LDADD = \
        $(top_builddir)/rulesets/Movement.o

Pedestriantest_SOURCES = Pedestriantest.cpp
Pedestriantest_LDADD = \
        $(top_builddir)/rulesets/Pedestrian.o \
        $(top_builddir)/rulesets/Movement.o

ExternalMindtest_SOURCES = ExternalMindtest.cpp
ExternalMindtest_LDADD = \
        $(top_builddir)/rulesets/ExternalMind.o

Python_APItest_SOURCES = Python_APItest.cpp \
        python_testers.cpp python_testers.h
Python_APItest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Quaterniontest_SOURCES = Py_Quaterniontest.cpp \
        python_testers.cpp python_testers.h
Py_Quaterniontest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Vector3Dtest_SOURCES = Py_Vector3Dtest.cpp \
        python_testers.cpp python_testers.h
Py_Vector3Dtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Point3Dtest_SOURCES = Py_Point3Dtest.cpp \
        python_testers.cpp python_testers.h
Py_Point3Dtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_BBoxtest_SOURCES = Py_BBoxtest.cpp \
        python_testers.cpp python_testers.h
Py_BBoxtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Locationtest_SOURCES = Py_Locationtest.cpp \
        python_testers.cpp python_testers.h
Py_Locationtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_RootEntitytest_SOURCES = Py_RootEntitytest.cpp \
        python_testers.cpp python_testers.h
Py_RootEntitytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Operationtest_SOURCES = Py_Operationtest.cpp \
        python_testers.cpp python_testers.h
Py_Operationtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Oplisttest_SOURCES = Py_Oplisttest.cpp \
        python_testers.cpp python_testers.h
Py_Oplisttest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Thingtest_SOURCES = Py_Thingtest.cpp \
        TestPropertyManager.cpp TestPropertyManager.h \
        python_testers.cpp python_testers.h
Py_Thingtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Maptest_SOURCES = Py_Maptest.cpp \
        python_testers.cpp python_testers.h
Py_Maptest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Tasktest_SOURCES = Py_Tasktest.cpp \
        python_testers.cpp python_testers.h
Py_Tasktest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Worldtest_SOURCES = Py_Worldtest.cpp \
        python_testers.cpp python_testers.h
Py_Worldtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Messagetest_SOURCES = Py_Messagetest.cpp \
        python_testers.cpp python_testers.h
Py_Messagetest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_WorldTimetest_SOURCES = Py_WorldTimetest.cpp \
        python_testers.cpp python_testers.h
Py_WorldTimetest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Propertytest_SOURCES = Py_Propertytest.cpp \
        python_testers.cpp python_testers.h
Py_Propertytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_TerrainPropertytest_SOURCES = Py_TerrainPropertytest.cpp \
        python_testers.cpp python_testers.h
Py_TerrainPropertytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_TerrainModPropertytest_SOURCES = Py_TerrainModPropertytest.cpp \
        python_testers.cpp python_testers.h
Py_TerrainModPropertytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Shapetest_SOURCES = Py_Shapetest.cpp \
        python_testers.cpp python_testers.h
Py_Shapetest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Filtertest_SOURCES = Py_Filtertest.cpp \
		python_testers.cpp python_testers.h
Py_Filtertest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

PythonWrappertest_SOURCES = PythonWrappertest.cpp
PythonWrappertest_LDADD = \
        $(top_builddir)/rulesets/PythonWrapper.o

PythonEntityScripttest_SOURCES = PythonEntityScripttest.cpp \
        python_testers.cpp python_testers.h
PythonEntityScripttest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

MindFactorytest_SOURCES = MindFactorytest.cpp
MindFactorytest_LDADD = \
        $(top_builddir)/rulesets/MindFactory.o

PythonContexttest_SOURCES = PythonContexttest.cpp
PythonContexttest_LDADD = \
        $(top_builddir)/rulesets/PythonContext.o

ArithmeticScripttest_SOURCES = ArithmeticScripttest.cpp
ArithmeticScripttest_LDADD = \
        $(top_builddir)/rulesets/ArithmeticScript.o

PythonArithmeticScripttest_SOURCES = PythonArithmeticScripttest.cpp \
        python_testers.cpp python_testers.h
PythonArithmeticScripttest_LDADD = \
        $(top_builddir)/rulesets/PythonArithmeticScript.o

ArithmeticFactorytest_SOURCES = ArithmeticFactorytest.cpp
ArithmeticFactorytest_LDADD = \
        $(top_builddir)/rulesets/ArithmeticFactory.o

PythonArithmeticFactorytest_SOURCES = PythonArithmeticFactorytest.cpp \
        python_testers.cpp python_testers.h
PythonArithmeticFactorytest_LDADD = \
        $(top_builddir)/rulesets/PythonArithmeticFactory.o \
        $(top_builddir)/rulesets/PythonClass.o

TerrainModtest_SOURCES = TerrainModtest.cpp
TerrainModtest_LDADD = \
        $(top_builddir)/rulesets/TerrainModTranslator.o \
        $(TERRAIN_LIBS)


TerrainEffectorPropertytest_SOURCES = TerrainEffectorPropertytest.cpp
TerrainEffectorPropertytest_LDADD = \
        $(top_builddir)/rulesets/TerrainEffectorProperty.o \
        $(TERRAIN_LIBS)

EntityFiltertest_SOURCES = EntityFiltertest.cpp
EntityFiltertest_LDADD = \
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

MindPropertyintegration_SOURCES = MindPropertyintegration.cpp \
        python_testers.cpp python_testers.h
MindPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/PythonScriptFactory.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)

TerrainPropertyintegration_SOURCES = TerrainPropertyintegration.cpp
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

PropertyEntityintegration_SOURCES = PropertyEntityintegration.cpp
PropertyEntityintegration_LDADD = \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/TypeNode.o

DecaysPropertyintegration_SOURCES = DecaysPropertyintegration.cpp
DecaysPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/DecaysProperty.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/TypeNode.o

BiomassPropertyintegration_SOURCES = BiomassPropertyintegration.cpp
BiomassPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/BiomassProperty.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/TypeNode.o

TerrainModPropertyintegration_SOURCES = TerrainModPropertyintegration.cpp
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

StatisticsPropertyintegration_SOURCES = StatisticsPropertyintegration.cpp
StatisticsPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/StatisticsProperty.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o

BaseMindMapEntityintegration_SOURCES = BaseMindMapEntityintegration.cpp
BaseMindMapEntityintegration_LDADD = \
        $(top_builddir)/rulesets/BaseMind.o \
        $(top_builddir)/rulesets/MemEntity.o \
        $(top_builddir)/rulesets/MemMap.o \
        $(top_builddir)/rulesets/BBoxProperty.o \
        $(top_builddir)/rulesets/SolidProperty.o \
        $(top_builddir)/rulesets/InternalProperties.o \
        $(top_builddir)/modules/Location.o \
        $(top_builddir)/common/Property.o
        
        

AreaPropertyintegration_SOURCES = AreaPropertyintegration.cpp
AreaPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/AreaProperty.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/physics/Course.o \
        $(top_builddir)/physics/Shape.o

SuspendedPropertyintegration_SOURCES = SuspendedPropertyintegration.cpp
SuspendedPropertyintegration_LDADD = \
        $(top_builddir)/rulesets/SuspendedProperty.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/common/libcommon.a
# CLIENT_TESTS

Py_CreatorClienttest_SOURCES = Py_CreatorClienttest.cpp \
        python_testers.cpp python_testers.h
Py_CreatorClienttest_LDADD = \
        $(top_builddir)/client/Py_CreatorClient.o \
        $(top_builddir)/client/Py_ObserverClient.o \
        $(top_builddir)/client/Python_ClientAPI.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)


Py_ObserverClienttest_SOURCES = Py_ObserverClienttest.cpp \
        python_testers.cpp python_testers.h
Py_ObserverClienttest_LDADD = \
        $(top_builddir)/client/Py_CreatorClient.o \
        $(top_builddir)/client/Py_ObserverClient.o \
        $(top_builddir)/client/Python_ClientAPI.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libentityfilter.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)

ClientConnectiontest_SOURCES = ClientConnectiontest.cpp
ClientConnectiontest_LDADD = \
        $(top_builddir)/client/ClientConnection.o $(NETWORK_LIBS)

BaseClienttest_SOURCES = BaseClienttest.cpp
BaseClienttest_LDADD = \
        $(top_builddir)/client/BaseClient.o $(NETWORK_LIBS)

ClientPropertyManagertest_SOURCES = ClientPropertyManagertest.cpp
ClientPropertyManagertest_LDADD = \
        $(top_builddir)/client/ClientPropertyManager.o \
        $(top_builddir)/common/PropertyManager.o

# CLIENT_INTEGRATION_TESTS

ClientConnectionintegration_SOURCES = ClientConnectionintegration.cpp
ClientConnectionintegration_LDADD = \
        $(top_builddir)/client/ClientConnection.o \
        $(top_builddir)/common/AtlasStreamClient.o \
        $(NETWORK_LIBS)

# SERVER_TESTS

Rulesettest_SOURCES = Rulesettest.cpp
Rulesettest_LDADD = \
        $(top_builddir)/server/Ruleset.o

EntityBuildertest_SOURCES = EntityBuildertest.cpp
EntityBuildertest_LDADD = \
        $(top_builddir)/server/EntityBuilder.o

PropertyFlagtest_SOURCES = \
        PropertyFlagtest.cpp \
        TestPropertyManager.cpp
PropertyFlagtest_LDADD = \
        $(top_builddir)/rulesets/Thing.o \
        $(top_builddir)/rulesets/Entity.o \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/common/Property.o

Accounttest_SOURCES = Accounttest.cpp
Accounttest_LDADD = \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/common/debug.o

Admintest_SOURCES = Admintest.cpp
Admintest_LDADD = \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/common/debug.o

ServerAccounttest_SOURCES = ServerAccounttest.cpp
ServerAccounttest_LDADD = \
        $(top_builddir)/server/ServerAccount.o

Playertest_SOURCES = Playertest.cpp
Playertest_LDADD = \
        $(top_builddir)/server/Player.o \
        $(top_builddir)/common/debug.o

EntityFactorytest_SOURCES = EntityFactorytest.cpp
EntityFactorytest_LDADD = \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/common/EntityKit.o

TaskFactorytest_SOURCES = TaskFactorytest.cpp
TaskFactorytest_LDADD = \
        $(top_builddir)/server/TaskFactory.o

buildidtest_SOURCES = buildidtest.cpp
buildidtest_LDADD = \
        $(top_builddir)/server/buildid.o

Connectiontest_SOURCES = \
        Connectiontest.cpp        
Connectiontest_LDADD = \
        $(top_builddir)/server/Connection.o \
        $(NETWORK_LIBS)

TrustedConnectiontest_SOURCES = \
        TrustedConnectiontest.cpp
TrustedConnectiontest_LDADD = \
        $(top_builddir)/server/TrustedConnection.o \
        $(NETWORK_LIBS)

WorldRoutertest_SOURCES = WorldRoutertest.cpp
WorldRoutertest_LDADD = \
        $(top_builddir)/server/WorldRouter.o

Peertest_SOURCES = \
        Peertest.cpp 
Peertest_LDADD = \
        $(top_builddir)/server/Peer.o \
        $(NETWORK_LIBS)

Lobbytest_SOURCES = Lobbytest.cpp
Lobbytest_LDADD = \
        $(top_builddir)/server/Lobby.o

Spawntest_SOURCES = Spawntest.cpp

SpawnEntitytest_SOURCES = SpawnEntitytest.cpp
SpawnEntitytest_LDADD = \
        $(top_builddir)/server/SpawnEntity.o

ArithmeticBuildertest_SOURCES = ArithmeticBuildertest.cpp
ArithmeticBuildertest_LDADD = \
        $(top_builddir)/server/ArithmeticBuilder.o

ServerRoutingtest_SOURCES = ServerRoutingtest.cpp
ServerRoutingtest_LDADD = \
        $(top_builddir)/server/ServerRouting.o

StorageManagertest_SOURCES = StorageManagertest.cpp
StorageManagertest_LDADD = \
        $(top_builddir)/server/StorageManager.o

HttpCachetest_SOURCES = HttpCachetest.cpp
HttpCachetest_LDADD = \
        $(top_builddir)/server/HttpCache.o

# SERVER_COMM_TESTS

CommPeertest_SOURCES = CommPeertest.cpp
CommPeertest_LDADD = \
        $(top_builddir)/server/CommPeer.o \
        $(NETWORK_LIBS)

CommMDNSPublishertest_SOURCES = CommMDNSPublishertest.cpp
CommMDNSPublishertest_LDADD = \
        $(top_builddir)/server/CommMDNSPublisher.o $(NETWORK_LIBS)

TeleportAuthenticatortest_SOURCES = TeleportAuthenticatortest.cpp
TeleportAuthenticatortest_LDADD = \
        $(top_builddir)/server/TeleportAuthenticator.o

TeleportStatetest_SOURCES = TeleportStatetest.cpp
TeleportStatetest_LDADD = \
        $(top_builddir)/server/TeleportState.o

PendingTeleporttest_SOURCES = PendingTeleporttest.cpp
PendingTeleporttest_LDADD = \
        $(top_builddir)/server/PendingTeleport.o

Juncturetest_SOURCES = Juncturetest.cpp
Juncturetest_LDADD = \
        $(top_builddir)/server/Juncture.o \
        $(NETWORK_LIBS)

ConnectableRoutertest_SOURCES = ConnectableRoutertest.cpp
ConnectableRoutertest_LDADD = \
        $(top_builddir)/server/ConnectableRouter.o

RuleHandlertest_SOURCES = RuleHandlertest.cpp
RuleHandlertest_LDADD = \
        $(top_builddir)/server/RuleHandler.o

OpRuleHandlertest_SOURCES = OpRuleHandlertest.cpp
OpRuleHandlertest_LDADD = \
        $(top_builddir)/server/OpRuleHandler.o

EntityRuleHandlertest_SOURCES = EntityRuleHandlertest.cpp
EntityRuleHandlertest_LDADD = \
        $(top_builddir)/server/EntityRuleHandler.o \
        $(top_builddir)/server/RuleHandler.o

TaskRuleHandlertest_SOURCES = TaskRuleHandlertest.cpp
TaskRuleHandlertest_LDADD = \
        $(top_builddir)/server/TaskRuleHandler.o \
        $(top_builddir)/server/RuleHandler.o

PropertyRuleHandlertest_SOURCES = PropertyRuleHandlertest.cpp
PropertyRuleHandlertest_LDADD = \
        $(top_builddir)/server/PropertyRuleHandler.o

IdleConnectortest_SOURCES = IdleConnectortest.cpp
IdleConnectortest_LDADD = \
        $(top_builddir)/server/IdleConnector.o $(NETWORK_LIBS)

CommPSQLSockettest_SOURCES = CommPSQLSockettest.cpp
CommPSQLSockettest_LDADD = \
        $(top_builddir)/server/CommPSQLSocket.o $(NETWORK_LIBS)

Persistencetest_SOURCES = Persistencetest.cpp
Persistencetest_LDADD = \
        $(top_builddir)/server/Persistence.o

SystemAccounttest_SOURCES = SystemAccounttest.cpp
SystemAccounttest_LDADD = \
        $(top_builddir)/server/SystemAccount.o

CorePropertyManagertest_SOURCES = CorePropertyManagertest.cpp
CorePropertyManagertest_LDADD = \
        $(top_builddir)/server/CorePropertyManager.o

# SERVER_INTEGRATION_TESTS

WorldRouterintegration_SOURCES = WorldRouterintegration.cpp
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

Rulesetintegration_SOURCES = Rulesetintegration.cpp
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
        ConnectionShakerintegration.cpp
ConnectionShakerintegration_LDADD = \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/common/Shaker.o
        
ConnectionCharacterintegration_SOURCES = \
        ConnectionCharacterintegration.cpp
ConnectionCharacterintegration_LDADD = \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/rulesets/Character.o \
        $(top_builddir)/rulesets/ExternalMind.o
        
ConnectionCreatorintegration_SOURCES = \
        ConnectionCreatorintegration.cpp
ConnectionCreatorintegration_LDADD = \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/rulesets/Creator.o \
        $(top_builddir)/rulesets/Character.o \
        $(top_builddir)/rulesets/ExternalMind.o
        
TrustedConnectionCreatorintegration_SOURCES = \
        TrustedConnectionCreatorintegration.cpp
TrustedConnectionCreatorintegration_LDADD = \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/rulesets/Creator.o \
        $(top_builddir)/rulesets/Character.o \
        $(top_builddir)/rulesets/ExternalMind.o
        
AccountConnectionCharacterintegration_SOURCES = \
        AccountConnectionCharacterintegration.cpp
AccountConnectionCharacterintegration_LDADD = \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/rulesets/Character.o \
        $(top_builddir)/rulesets/ExternalMind.o
        
Accountintegration_SOURCES = \
        Accountintegration.cpp
Accountintegration_LDADD = \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/server/TeleportAuthenticator.o \
        $(top_builddir)/server/PendingTeleport.o \
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
        EntityFactoryTypeNodeintegration.cpp \
        TestPropertyManager.cpp
EntityFactoryTypeNodeintegration_LDADD = \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/common/EntityKit.o \
        $(top_builddir)/common/TypeNode.o \
        $(top_builddir)/common/PropertyManager.o \
        $(top_builddir)/common/Property.o

AccountConnectionintegration_SOURCES = \
        AccountConnectionintegration.cpp
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
        AccountServerLobbyintegration.cpp
AccountServerLobbyintegration_LDADD = \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/Lobby.o

# Other TESTS

Mastertest_SOURCES = Mastertest.cpp
Mastertest_LDADD = \
        $(top_builddir)/server/Master.o \
        $(top_builddir)/common/libcommon.a


# TOOLS_TESTS

AdminClienttest_SOURCES = AdminClienttest.cpp
AdminClienttest_LDADD = \
        $(top_builddir)/tools/AdminClient.o $(NETWORK_LIBS)

Flushertest_SOURCES = Flushertest.cpp
Flushertest_LDADD = \
        $(top_builddir)/tools/Flusher.o \
        $(top_builddir)/common/ClientTask.o

OperationMonitortest_SOURCES = OperationMonitortest.cpp
OperationMonitortest_LDADD = \
        $(top_builddir)/tools/OperationMonitor.o \
        $(top_builddir)/common/ClientTask.o

MultiLineListFormattertest_SOURCES = MultiLineListFormattertest.cpp
MultiLineListFormattertest_LDADD = \
        $(top_builddir)/tools/MultiLineListFormatter.o


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
