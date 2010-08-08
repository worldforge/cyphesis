COMMON_TESTS = logtest Inheritancetest Propertytest \
               systemtest system_nettest system_uidtest \
               type_utilstest \
               consttest randomtest Addtest Attacktest Burntest \
               Connecttest Droptest Eattest \
               Monitortest Nourishtest Pickuptest Setuptest \
               Ticktest Unseentest Updatetest AtlasFileLoadertest \
               BaseWorldtest Databasetest idtest AccountBasetest \
               debugtest globalstest OperationRoutertest Routertest \
               client_sockettest customtest Monitorstest \
               operationstest serialnotest newidtest TypeNodetest \
               FormattedXMLWritertest PropertyFactorytest \
               PropertyManagertest Variabletest AtlasStreamClienttest \
               ClientTasktest

PHYSICS_TESTS = BBoxtest Vector3Dtest Quaterniontest \
                transformtest Collisiontest emergencetest distancetest

MODULE_TESTS = Locationtest WorldTimetest EntityReftest DateTimetest

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
                 HandlerPropertytest MultiHandlerPropertytest \
                 InternalPropertiestest EntityPropertiestest \
                 SpawnPropertytest \
                 Domaintest AtlasPropertiestest \
                 BaseMindtest MemEntitytest MemMaptest Movementtest \
                 Pedestriantest ActivePropertyFactorytest \
                 Python_APItest Py_Quaterniontest Py_Vector3Dtest \
                 Py_Point3Dtest Py_BBoxtest Py_Locationtest \
                 Py_RootEntitytest Py_Operationtest Py_Oplisttest \
                 Py_Thingtest Py_Maptest Py_Mindtest Py_Tasktest \
                 Py_Worldtest Py_Messagetest Py_WorldTimetest \
                 Py_Propertytest Py_TerrainPropertytest \
                 PythonWrappertest PythonEntityScripttest \
                 MindFactorytest TaskScripttest PythonContexttest \
                 ArithmeticScripttest PythonArithmeticScripttest \
                 ArithmeticFactorytest PythonArithmeticFactorytest \
                 MindFactoryintegration

CLIENT_TESTS = Py_CreatorClienttest Py_ObserverClienttest \
               ClientConnectiontest

SERVER_TESTS = Rulesettest EntityBuildertest PropertyFlagtest \
               CommServertest Accounttest Admintest Playertest buildidtest \
               EntityFactorytest TaskFactorytest Connectiontest \
               TrustedConnectiontest WorldRoutertest Peertest Lobbytest \
               Spawntest SpawnEntitytest ArithmeticBuildertest \
               CommClientFactorytest ServerRoutingtest Idletest \
               StorageManagertest HttpCachetest UpdateTestertest \
               ExternalMindtest

SERVER_COMM_TESTS = CommSockettest CommStreamClienttest CommClienttest \
                    CommHttpClienttest CommStreamListenertest

SERVER_INTEGRATION_TESTS = WorldRouterintegration Rulesetintegration

TOOLS_TESTS = WorldLoadertest AdminClienttest WorldDumpertest

PYTHON_TESTS = python_class

INCLUDES = -I$(top_srcdir) -I$(top_builddir) \
           -DTESTDATADIR=\"$(abs_top_srcdir)/tests/data\"

LIBS = $(COMMON_LIBS) $(PGSQL_LIBS) $(PYTHON_LIBS) $(PYTHON_UTIL_LIBS) $(MATH_LIBS)

TESTS = $(COMMON_TESTS) $(PHYSICS_TESTS) $(MODULE_TESTS) $(RULESETS_TESTS) \
        $(SERVER_TESTS) $(SERVER_COMM_TESTS) $(CLIENT_TESTS) $(TOOLS_TESTS) \
        $(SERVER_INTEGRATION_TESTS)

EXTRA_PROGRAMS = $(PYTHON_TESTS) Mastertest

check_PROGRAMS = $(TESTS)

dist-hook:
	(cd $(top_srcdir)/tests && tar cf - data) | (cd $(distdir) && tar xf -)

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

Addtest_SOURCES = Addtest.cpp OperationExerciser.cpp OperationExerciser.h

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

AccountBasetest_SOURCES = AccountBasetest.cpp
AccountBasetest_LDADD = \
        $(top_builddir)/common/AccountBase.o

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
        $(top_builddir)/common/client_socket.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/globals.o

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

# RULESETS_TESTS

LocatedEntitytest_SOURCES = LocatedEntitytest.cpp \
                            EntityExerciser.cpp EntityExerciser.h \
                            allOperations.h
LocatedEntitytest_LDADD = \
        $(top_builddir)/rulesets/LocatedEntity.o

Entitytest_SOURCES = Entitytest.cpp \
                     TestPropertyManager.cpp TestPropertyManager.h \
                     IGEntityExerciser.cpp IGEntityExerciser.h \
                     EntityExerciser.cpp EntityExerciser.h allOperations.h
Entitytest_LDADD = \
        $(top_builddir)/rulesets/Entity.o

Planttest_SOURCES = Planttest.cpp \
                    TestPropertyManager.cpp TestPropertyManager.h \
                    IGEntityExerciser.cpp IGEntityExerciser.h \
                    EntityExerciser.cpp EntityExerciser.h allOperations.h \
                    TestWorld.h
Planttest_LDADD = \
        $(top_builddir)/rulesets/Plant.o

Stackabletest_SOURCES = Stackabletest.cpp \
                        TestPropertyManager.cpp TestPropertyManager.h \
                        IGEntityExerciser.cpp IGEntityExerciser.h \
                        EntityExerciser.cpp EntityExerciser.h allOperations.h
Stackabletest_LDADD = \
        $(top_builddir)/rulesets/Stackable.o

Thingtest_SOURCES = Thingtest.cpp \
                    TestPropertyManager.cpp TestPropertyManager.h \
                    IGEntityExerciser.cpp IGEntityExerciser.h \
                    EntityExerciser.cpp EntityExerciser.h allOperations.h
Thingtest_LDADD = \
        $(top_builddir)/rulesets/Thing.o

Worldtest_SOURCES = Worldtest.cpp \
                    TestPropertyManager.cpp TestPropertyManager.h \
                    IGEntityExerciser.cpp IGEntityExerciser.h \
                    EntityExerciser.cpp EntityExerciser.h allOperations.h
Worldtest_LDADD = \
        $(top_builddir)/rulesets/World.o

Charactertest_SOURCES = Charactertest.cpp \
                        TestPropertyManager.cpp TestPropertyManager.h \
                        IGEntityExerciser.cpp IGEntityExerciser.h \
                        EntityExerciser.cpp EntityExerciser.h allOperations.h
Charactertest_LDADD = \
        $(top_builddir)/rulesets/Character.o

Creatortest_SOURCES = Creatortest.cpp \
                      TestPropertyManager.cpp TestPropertyManager.h \
                      IGEntityExerciser.cpp IGEntityExerciser.h \
                      EntityExerciser.cpp EntityExerciser.h allOperations.h
Creatortest_LDADD = \
        $(top_builddir)/rulesets/Creator.o

ThingupdatePropertiestest_SOURCES = ThingupdatePropertiestest.cpp TestPropertyManager.cpp allOperations.h
ThingupdatePropertiestest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(MATH_LIBS)

Containertest_SOURCES = Containertest.cpp
Containertest_LDADD = $(top_builddir)/rulesets/Container.o

Tasktest_SOURCES = Tasktest.cpp Fell.cpp Fell.h
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
        $(top_builddir)/modules/EntityRef.o \
        $(top_builddir)/modules/DateTime.o \
        $(top_builddir)/common/Property.o \
        $(top_builddir)/common/Inheritance.o \
        $(top_builddir)/common/custom.o \
        $(top_builddir)/common/operations.o \
        $(TERRAIN_LIBS)

Scripttest_SOURCES = Scripttest.cpp
Scripttest_LDADD = $(top_builddir)/rulesets/Script.o

Motiontest_SOURCES = Motiontest.cpp
Motiontest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

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

TerrainModPropertytest_SOURCES = TerrainModPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TerrainModPropertytest_LDADD = \
        $(top_builddir)/rulesets/TerrainModProperty.o \
        $(top_builddir)/rulesets/TerrainProperty.o \
        $(top_builddir)/common/Property.o \
        $(TERRAIN_LIBS)

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

HandlerPropertytest_SOURCES = HandlerPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
HandlerPropertytest_LDADD = \
        $(top_builddir)/rulesets/HandlerProperty.o \
        $(top_builddir)/common/Property.o

MultiHandlerPropertytest_SOURCES = MultiHandlerPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
MultiHandlerPropertytest_LDADD = \
        $(top_builddir)/rulesets/MultiHandlerProperty.o \
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

Domaintest_SOURCES = Domaintest.cpp
Domaintest_LDADD = \
        $(top_builddir)/rulesets/Domain.o

BaseMindtest_SOURCES = BaseMindtest.cpp
BaseMindtest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

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

ActivePropertyFactorytest_SOURCES = ActivePropertyFactorytest.cpp
ActivePropertyFactorytest_LDADD = \
        $(top_builddir)/rulesets/ActivePropertyFactory.o

Python_APItest_SOURCES = Python_APItest.cpp python_testers.h
Python_APItest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Quaterniontest_SOURCES = Py_Quaterniontest.cpp
Py_Quaterniontest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Vector3Dtest_SOURCES = Py_Vector3Dtest.cpp
Py_Vector3Dtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Point3Dtest_SOURCES = Py_Point3Dtest.cpp
Py_Point3Dtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_BBoxtest_SOURCES = Py_BBoxtest.cpp python_testers.h
Py_BBoxtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Locationtest_SOURCES = Py_Locationtest.cpp python_testers.h
Py_Locationtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_RootEntitytest_SOURCES = Py_RootEntitytest.cpp python_testers.h
Py_RootEntitytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Operationtest_SOURCES = Py_Operationtest.cpp python_testers.h
Py_Operationtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Oplisttest_SOURCES = Py_Oplisttest.cpp python_testers.h
Py_Oplisttest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Thingtest_SOURCES = Py_Thingtest.cpp TestPropertyManager.cpp TestPropertyManager.h python_testers.h
Py_Thingtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Maptest_SOURCES = Py_Maptest.cpp python_testers.h
Py_Maptest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Mindtest_SOURCES = Py_Mindtest.cpp python_testers.h
Py_Mindtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Tasktest_SOURCES = Py_Tasktest.cpp python_testers.h
Py_Tasktest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Worldtest_SOURCES = Py_Worldtest.cpp python_testers.h
Py_Worldtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Messagetest_SOURCES = Py_Messagetest.cpp python_testers.h
Py_Messagetest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_WorldTimetest_SOURCES = Py_WorldTimetest.cpp python_testers.h
Py_WorldTimetest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Propertytest_SOURCES = Py_Propertytest.cpp python_testers.h
Py_Propertytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_TerrainPropertytest_SOURCES = Py_TerrainPropertytest.cpp python_testers.h
Py_TerrainPropertytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

PythonWrappertest_SOURCES = PythonWrappertest.cpp
PythonWrappertest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

PythonEntityScripttest_SOURCES = PythonEntityScripttest.cpp python_testers.h
PythonEntityScripttest_LDADD = \
        $(top_builddir)/server/ScriptFactory.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

MindFactorytest_SOURCES = MindFactorytest.cpp
MindFactorytest_LDADD = \
        $(top_builddir)/rulesets/MindFactory.o

TaskScripttest_SOURCES = TaskScripttest.cpp
TaskScripttest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

PythonContexttest_SOURCES = PythonContexttest.cpp
PythonContexttest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

ArithmeticScripttest_SOURCES = ArithmeticScripttest.cpp
ArithmeticScripttest_LDADD = \
        $(top_builddir)/rulesets/ArithmeticScript.o \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

PythonArithmeticScripttest_SOURCES = PythonArithmeticScripttest.cpp python_testers.h
PythonArithmeticScripttest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

ArithmeticFactorytest_SOURCES = ArithmeticFactorytest.cpp
ArithmeticFactorytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

PythonArithmeticFactorytest_SOURCES = PythonArithmeticFactorytest.cpp
PythonArithmeticFactorytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

MindFactoryintegration_SOURCES = MindFactoryintegration.cpp
MindFactoryintegration_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

# CLIENT_TESTS

Py_CreatorClienttest_SOURCES = Py_CreatorClienttest.cpp python_testers.h
Py_CreatorClienttest_LDADD = \
        $(top_builddir)/client/Py_CreatorClient.o \
        $(top_builddir)/client/Py_ObserverClient.o \
        $(top_builddir)/client/Python_ClientAPI.o \
        $(top_builddir)/client/CreatorClient.o \
        $(top_builddir)/client/CharacterClient.o \
        $(top_builddir)/client/ClientConnection.o \
        $(top_builddir)/client/ObserverClient.o \
        $(top_builddir)/client/BaseClient.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)


Py_ObserverClienttest_SOURCES = Py_ObserverClienttest.cpp python_testers.h
Py_ObserverClienttest_LDADD = \
        $(top_builddir)/client/Py_CreatorClient.o \
        $(top_builddir)/client/Py_ObserverClient.o \
        $(top_builddir)/client/Python_ClientAPI.o \
        $(top_builddir)/client/CreatorClient.o \
        $(top_builddir)/client/CharacterClient.o \
        $(top_builddir)/client/ClientConnection.o \
        $(top_builddir)/client/ObserverClient.o \
        $(top_builddir)/client/BaseClient.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)

ClientConnectiontest_SOURCES = ClientConnectiontest.cpp python_testers.h
ClientConnectiontest_LDADD = \
        $(top_builddir)/client/ClientConnection.o \
        $(top_builddir)/common/libcommon.a \
        $(NETWORK_LIBS)


# SERVER_TESTS

Rulesettest_SOURCES = Rulesettest.cpp
Rulesettest_LDADD = \
        $(top_builddir)/server/Ruleset.o \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/server/Player.o \
        $(top_builddir)/server/ServerAccount.o \
        $(top_builddir)/server/Persistence.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/buildid.o \
        $(top_builddir)/server/ExternalMind.o \
        $(top_builddir)/server/ExternalProperty.o \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/server/CommStreamClient.o \
        $(top_builddir)/server/CommClient.o \
        $(top_builddir)/server/Idle.o \
        $(top_builddir)/server/CommSocket.o \
        $(top_builddir)/server/CommPeer.o \
        $(top_builddir)/server/Peer.o \
        $(top_builddir)/server/PendingTeleport.o \
        $(top_builddir)/server/CommServer.o \
        $(top_builddir)/server/ScriptFactory.o \
        $(top_builddir)/server/TaskFactory.o \
        $(top_builddir)/server/TaskScriptFactory.o \
        $(top_builddir)/server/TeleportAuthenticator.o \
        $(top_builddir)/server/TeleportState.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)
Rulesettest_LDFLAGS = $(PYTHON_LINKER_FLAGS)

EntityBuildertest_SOURCES = EntityBuildertest.cpp
EntityBuildertest_LDADD = \
        $(top_builddir)/server/EntityBuilder.o

PropertyFlagtest_SOURCES = PropertyFlagtest.cpp
PropertyFlagtest_LDADD = \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/Ruleset.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/Persistence.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/server/Player.o \
        $(top_builddir)/server/ServerAccount.o \
        $(top_builddir)/server/TeleportAuthenticator.o \
        $(top_builddir)/server/PendingTeleport.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/server/buildid.o \
        $(top_builddir)/server/ExternalMind.o \
        $(top_builddir)/server/ExternalProperty.o \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/server/CommStreamClient.o \
        $(top_builddir)/server/CommClient.o \
        $(top_builddir)/server/Idle.o \
        $(top_builddir)/server/CommSocket.o \
        $(top_builddir)/server/CommPeer.o \
        $(top_builddir)/server/Peer.o \
        $(top_builddir)/server/CommServer.o \
        $(top_builddir)/server/ScriptFactory.o \
        $(top_builddir)/server/TaskFactory.o \
        $(top_builddir)/server/TaskScriptFactory.o \
        $(top_builddir)/server/TeleportState.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)
PropertyFlagtest_LDFLAGS = $(PYTHON_LINKER_FLAGS)

CommServertest_SOURCES = CommServertest.cpp
CommServertest_LDADD = \
        $(top_builddir)/server/CommServer.o

Accounttest_SOURCES = Accounttest.cpp
Accounttest_LDADD = \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/buildid.o \
        $(top_builddir)/server/ExternalMind.o \
        $(top_builddir)/server/ExternalProperty.o \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/server/CommStreamClient.o \
        $(top_builddir)/server/CommClient.o \
        $(top_builddir)/server/Idle.o \
        $(top_builddir)/server/CommSocket.o \
        $(top_builddir)/server/CommPeer.o \
        $(top_builddir)/server/Peer.o \
        $(top_builddir)/server/CommServer.o \
        $(top_builddir)/server/ScriptFactory.o \
        $(top_builddir)/server/TaskFactory.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/server/Player.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/server/Persistence.o \
        $(top_builddir)/server/ServerAccount.o \
        $(top_builddir)/server/TeleportAuthenticator.o \
        $(top_builddir)/server/PendingTeleport.o \
        $(top_builddir)/server/Ruleset.o \
        $(top_builddir)/server/TaskScriptFactory.o \
        $(top_builddir)/server/TeleportState.o \
        $(top_builddir)/server/WorldRouter.o \
        $(top_builddir)/server/SpawnEntity.o \
        $(top_builddir)/server/Spawn.o \
        $(top_builddir)/server/ArithmeticBuilder.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)
Accounttest_LDFLAGS = $(PYTHON_LINKER_FLAGS)

Admintest_SOURCES = Admintest.cpp
Admintest_LDADD = \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/buildid.o \
        $(top_builddir)/server/ExternalMind.o \
        $(top_builddir)/server/ExternalProperty.o \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/server/CommStreamClient.o \
        $(top_builddir)/server/CommClient.o \
        $(top_builddir)/server/Idle.o \
        $(top_builddir)/server/CommSocket.o \
        $(top_builddir)/server/CommPeer.o \
        $(top_builddir)/server/Peer.o \
        $(top_builddir)/server/CommServer.o \
        $(top_builddir)/server/ScriptFactory.o \
        $(top_builddir)/server/TaskFactory.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/server/Player.o \
        $(top_builddir)/server/ServerAccount.o \
        $(top_builddir)/server/TeleportAuthenticator.o \
        $(top_builddir)/server/PendingTeleport.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/server/Persistence.o \
        $(top_builddir)/server/Ruleset.o \
        $(top_builddir)/server/TaskScriptFactory.o \
        $(top_builddir)/server/TeleportState.o \
        $(top_builddir)/server/WorldRouter.o \
        $(top_builddir)/server/SpawnEntity.o \
        $(top_builddir)/server/Spawn.o \
        $(top_builddir)/server/ArithmeticBuilder.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)
Admintest_LDFLAGS = $(PYTHON_LINKER_FLAGS)

ServerAccounttest_SOURCES = ServerAccounttest.cpp
ServerAccounttest_LDADD = \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/ArithmeticBuilder.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/buildid.o \
        $(top_builddir)/server/ExternalMind.o \
        $(top_builddir)/server/ExternalProperty.o \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/server/CommStreamClient.o \
        $(top_builddir)/server/CommClient.o \
        $(top_builddir)/server/Idle.o \
        $(top_builddir)/server/CommSocket.o \
        $(top_builddir)/server/CommPeer.o \
        $(top_builddir)/server/Peer.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/server/ServerAccount.o \
        $(top_builddir)/server/TeleportAuthenticator.o \
        $(top_builddir)/server/PendingTeleport.o \
        $(top_builddir)/server/CommServer.o \
        $(top_builddir)/server/ScriptFactory.o \
        $(top_builddir)/server/TaskFactory.o \
        $(top_builddir)/server/Player.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/server/Persistence.o \
        $(top_builddir)/server/Ruleset.o \
        $(top_builddir)/server/TaskScriptFactory.o \
        $(top_builddir)/server/TeleportState.o \
        $(top_builddir)/server/WorldRouter.o \
        $(top_builddir)/server/SpawnEntity.o \
        $(top_builddir)/server/Spawn.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)
ServerAccounttest_LDFLAGS = $(PYTHON_LINKER_FLAGS)

Playertest_SOURCES = Playertest.cpp
Playertest_LDADD = \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/buildid.o \
        $(top_builddir)/server/ExternalMind.o \
        $(top_builddir)/server/ExternalProperty.o \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/server/CommStreamClient.o \
        $(top_builddir)/server/CommClient.o \
        $(top_builddir)/server/Idle.o \
        $(top_builddir)/server/CommSocket.o \
        $(top_builddir)/server/CommPeer.o \
        $(top_builddir)/server/Peer.o \
        $(top_builddir)/server/CommServer.o \
        $(top_builddir)/server/ScriptFactory.o \
        $(top_builddir)/server/TaskFactory.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/server/Player.o \
        $(top_builddir)/server/ServerAccount.o \
        $(top_builddir)/server/TeleportAuthenticator.o \
        $(top_builddir)/server/PendingTeleport.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/server/Persistence.o \
        $(top_builddir)/server/Ruleset.o \
        $(top_builddir)/server/TaskScriptFactory.o \
        $(top_builddir)/server/TeleportState.o \
        $(top_builddir)/server/WorldRouter.o \
        $(top_builddir)/server/SpawnEntity.o \
        $(top_builddir)/server/Spawn.o \
        $(top_builddir)/server/ArithmeticBuilder.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)
Playertest_LDFLAGS = $(PYTHON_LINKER_FLAGS)

EntityFactorytest_SOURCES = EntityFactorytest.cpp
EntityFactorytest_LDADD = \
        $(top_builddir)/server/EntityFactory.o

TaskFactorytest_SOURCES = TaskFactorytest.cpp
TaskFactorytest_LDADD = \
        $(top_builddir)/server/TaskFactory.o

buildidtest_SOURCES = buildidtest.cpp
buildidtest_LDADD = \
        $(top_builddir)/server/buildid.o

Connectiontest_SOURCES = Connectiontest.cpp
Connectiontest_LDADD = \
        $(top_builddir)/server/Connection.o \
        $(NETWORK_LIBS)

TrustedConnectiontest_SOURCES = TrustedConnectiontest.cpp
TrustedConnectiontest_LDADD = \
        $(top_builddir)/server/TrustedConnection.o \
        $(NETWORK_LIBS)

WorldRoutertest_SOURCES = WorldRoutertest.cpp
WorldRoutertest_LDADD = \
        $(top_builddir)/server/WorldRouter.o

Peertest_SOURCES = Peertest.cpp
Peertest_LDADD = \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/buildid.o \
        $(top_builddir)/server/ExternalMind.o \
        $(top_builddir)/server/ExternalProperty.o \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/server/CommStreamClient.o \
        $(top_builddir)/server/CommClient.o \
        $(top_builddir)/server/Idle.o \
        $(top_builddir)/server/CommSocket.o \
        $(top_builddir)/server/CommPeer.o \
        $(top_builddir)/server/Peer.o \
        $(top_builddir)/server/CommServer.o \
        $(top_builddir)/server/ScriptFactory.o \
        $(top_builddir)/server/TaskFactory.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/server/Player.o \
        $(top_builddir)/server/ServerAccount.o \
        $(top_builddir)/server/TeleportAuthenticator.o \
        $(top_builddir)/server/PendingTeleport.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/server/Persistence.o \
        $(top_builddir)/server/Ruleset.o \
        $(top_builddir)/server/TaskScriptFactory.o \
        $(top_builddir)/server/TeleportState.o \
        $(top_builddir)/server/WorldRouter.o \
        $(top_builddir)/server/SpawnEntity.o \
        $(top_builddir)/server/Spawn.o \
        $(top_builddir)/server/ArithmeticBuilder.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)

Lobbytest_SOURCES = Lobbytest.cpp
Lobbytest_LDADD = \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/common/libcommon.a

Spawntest_SOURCES = Spawntest.cpp
Spawntest_LDADD = \
        $(top_builddir)/server/Spawn.o

SpawnEntitytest_SOURCES = SpawnEntitytest.cpp
SpawnEntitytest_LDADD = \
        $(top_builddir)/server/SpawnEntity.o

ArithmeticBuildertest_SOURCES = ArithmeticBuildertest.cpp
ArithmeticBuildertest_LDADD = \
        $(top_builddir)/server/ArithmeticBuilder.o

CommClientFactorytest_SOURCES = CommClientFactorytest.cpp
CommClientFactorytest_LDADD = \
        $(top_builddir)/server/CommClientFactory.o \
        $(NETWORK_LIBS)

ServerRoutingtest_SOURCES = ServerRoutingtest.cpp
ServerRoutingtest_LDADD = \
        $(top_builddir)/server/ServerRouting.o

Idletest_SOURCES = Idletest.cpp
Idletest_LDADD = \
        $(top_builddir)/server/Idle.o

StorageManagertest_SOURCES = StorageManagertest.cpp
StorageManagertest_LDADD = \
        $(top_builddir)/server/StorageManager.o

HttpCachetest_SOURCES = HttpCachetest.cpp
HttpCachetest_LDADD = \
        $(top_builddir)/server/HttpCache.o

UpdateTestertest_SOURCES = UpdateTestertest.cpp
UpdateTestertest_LDADD = \
        $(top_builddir)/server/UpdateTester.o

ExternalMindtest_SOURCES = ExternalMindtest.cpp
ExternalMindtest_LDADD = \
        $(top_builddir)/server/ExternalMind.o

# SERVER_COMM_TESTS

CommSockettest_SOURCES = CommSockettest.cpp
CommSockettest_LDADD = \
        $(top_builddir)/server/CommSocket.o

CommStreamClienttest_SOURCES = CommStreamClienttest.cpp
CommStreamClienttest_LDADD = \
        $(top_builddir)/server/CommStreamClient.o \
        $(NETWORK_LIBS)

CommClienttest_SOURCES = CommClienttest.cpp
CommClienttest_LDADD = \
        $(top_builddir)/server/CommClient.o \
        $(NETWORK_LIBS)

CommHttpClienttest_SOURCES = CommHttpClienttest.cpp
CommHttpClienttest_LDADD = \
        $(top_builddir)/server/CommHttpClient.o \
        $(NETWORK_LIBS)

CommStreamListenertest_SOURCES = CommStreamListenertest.cpp
CommStreamListenertest_LDADD = \
        $(top_builddir)/server/CommStreamListener.o \
        $(NETWORK_LIBS)

TeleportAuthenticatortest_SOURCES = TeleportAuthenticatortest.cpp
TeleportAuthenticatortest_LDADD = \
        $(top_builddir)/server/TeleportAuthenticator.o \
        $(top_builddir)/server/PendingTeleport.o \
        $(top_builddir)/common/BaseWorld.o \
        $(top_builddir)/common/id.o \
        $(top_builddir)/common/globals.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/log.o

# SERVER_INTEGRATION_TESTS

WorldRouterintegration_SOURCES = WorldRouterintegration.cpp
WorldRouterintegration_LDADD = \
        $(top_builddir)/server/WorldRouter.o \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/TaskScriptFactory.o \
        $(top_builddir)/server/ArithmeticBuilder.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)
WorldRouterintegration_LDFLAGS = $(PYTHON_LINKER_FLAGS)

Rulesetintegration_SOURCES = Rulesetintegration.cpp
Rulesetintegration_LDADD = \
        $(top_builddir)/server/Ruleset.o \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/server/Player.o \
        $(top_builddir)/server/Persistence.o \
        $(top_builddir)/server/Connection.o \
        $(top_builddir)/server/ServerRouting.o \
        $(top_builddir)/server/buildid.o \
        $(top_builddir)/server/ExternalMind.o \
        $(top_builddir)/server/ExternalProperty.o \
        $(top_builddir)/server/Lobby.o \
        $(top_builddir)/server/CommStreamClient.o \
        $(top_builddir)/server/CommClient.o \
        $(top_builddir)/server/Idle.o \
        $(top_builddir)/server/CommSocket.o \
        $(top_builddir)/server/CommPeer.o \
        $(top_builddir)/server/Peer.o \
        $(top_builddir)/server/CommServer.o \
        $(top_builddir)/server/ScriptFactory.o \
        $(top_builddir)/server/TaskFactory.o \
        $(top_builddir)/server/TaskScriptFactory.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)
Rulesetintegration_LDFLAGS = $(PYTHON_LINKER_FLAGS)

# Other TESTS

Mastertest_SOURCES = Mastertest.cpp
Mastertest_LDADD = \
        $(top_builddir)/server/Master.o \
        $(top_builddir)/common/libcommon.a


# TOOLS_TESTS

WorldLoadertest_SOURCES = WorldLoadertest.cpp
WorldLoadertest_LDADD = \
        $(top_builddir)/tools/WorldLoader.o \
        $(top_builddir)/common/AtlasFileLoader.o \
        $(top_builddir)/common/ClientTask.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/globals.o \
        $(top_builddir)/common/log.o

AdminClienttest_SOURCES = AdminClienttest.cpp
AdminClienttest_LDADD = \
        $(top_builddir)/tools/AdminClient.o

WorldDumpertest_SOURCES = WorldDumpertest.cpp
WorldDumpertest_LDADD = \
        $(top_builddir)/tools/WorldDumper.o

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

