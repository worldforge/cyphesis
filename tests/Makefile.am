COMMON_TESTS = logtest Inheritancetest Propertytest \
               systemtest type_utilstest \
               consttest randomtest Addtest Attacktest Burntest \
               Connecttest Droptest Eattest \
               Monitortest Nourishtest Pickuptest Setuptest \
               Ticktest Unseentest Updatetest AtlasFileLoadertest \
               BaseWorldtest Databasetest idtest AccountBasetest \
               debugtest globalstest OperationRoutertest Routertest \
               client_sockettest customtest Monitorstest \
               operationstest serialnotest newidtest TypeNodetest \
               FormattedXMLWritertest PropertyFactorytest \
               PropertyManagertest

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
                 InternalPropertiestest EntityPropertiestest \
                 Domaintest AtlasPropertiestest \
                 BaseMindtest MemEntitytest MemMaptest Movementtest \
                 Pedestriantest ActivePropertyFactorytest \
                 Python_APItest Py_Quaterniontest Py_Vector3Dtest \
                 Py_Point3Dtest Py_BBoxtest Py_Locationtest \
                 Py_RootEntitytest Py_Operationtest Py_Oplisttest \
                 Py_Thingtest Py_Maptest Py_Mindtest Py_Tasktest \
                 Py_Worldtest Py_Messagetest Py_WorldTimetest


CLIENT_TESTS = Py_CreatorClienttest

SERVER_TESTS = EntityBuildertest PropertyFlagtest

PYTHON_TESTS = python_class

INCLUDES = -I$(top_srcdir) -I$(top_builddir)

LIBS = $(COMMON_LIBS) $(PGSQL_LIBS) $(PYTHON_LIBS) $(PYTHON_UTIL_LIBS) $(MATH_LIBS)

TESTS = $(COMMON_TESTS) $(PHYSICS_TESTS) $(MODULE_TESTS) $(RULESETS_TESTS) $(SERVER_TESTS) $(CLIENT_TESTS)

EXTRA_PROGRAMS = $(PYTHON_TESTS)

check_PROGRAMS = $(TESTS)

# COMMON_TESTS

logtest_SOURCES = logtest.cpp
logtest_LDADD = \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/prefix.o \
        $(top_builddir)/common/globals.o

Inheritancetest_SOURCES = Inheritancetest.cpp
Inheritancetest_LDADD = \
        $(top_builddir)/common/Inheritance.o \
        $(top_builddir)/common/TypeNode.o \
        $(top_builddir)/common/custom.o \
        $(top_builddir)/common/operations.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/prefix.o \
        $(top_builddir)/common/globals.o

Propertytest_SOURCES = Propertytest.cpp
Propertytest_LDADD = $(top_builddir)/common/Property.o

systemtest_SOURCES = systemtest.cpp
systemtest_LDADD = \
        $(top_builddir)/common/system.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/prefix.o \
        $(top_builddir)/common/globals.o \
        $(NETWORK_LIBS)

type_utilstest_SOURCES = type_utilstest.cpp
type_utilstest_LDADD = $(top_builddir)/common/type_utils.o

consttest_SOURCES = consttest.cpp
consttest_LDADD = $(top_builddir)/common/const.o

randomtest_SOURCES = randomtest.cpp

Addtest_SOURCES = Addtest.cpp OperationExerciser.cpp OperationExerciser.h
Addtest_LDADD = $(top_builddir)/common/operations.o

Attacktest_SOURCES = Attacktest.cpp
Attacktest_LDADD = $(top_builddir)/common/operations.o

Burntest_SOURCES = Burntest.cpp
Burntest_LDADD = $(top_builddir)/common/operations.o

Connecttest_SOURCES = Connecttest.cpp
Connecttest_LDADD = $(top_builddir)/common/operations.o

Droptest_SOURCES = Droptest.cpp
Droptest_LDADD = $(top_builddir)/common/operations.o

Eattest_SOURCES = Eattest.cpp
Eattest_LDADD = $(top_builddir)/common/operations.o

Monitortest_SOURCES = Monitortest.cpp
Monitortest_LDADD = $(top_builddir)/common/operations.o

Nourishtest_SOURCES = Nourishtest.cpp
Nourishtest_LDADD = $(top_builddir)/common/operations.o

Pickuptest_SOURCES = Pickuptest.cpp
Pickuptest_LDADD = $(top_builddir)/common/operations.o

Setuptest_SOURCES = Setuptest.cpp
Setuptest_LDADD = $(top_builddir)/common/operations.o

Ticktest_SOURCES = Ticktest.cpp
Ticktest_LDADD = $(top_builddir)/common/operations.o

Unseentest_SOURCES = Unseentest.cpp
Unseentest_LDADD = $(top_builddir)/common/operations.o

Updatetest_SOURCES = Updatetest.cpp
Updatetest_LDADD = $(top_builddir)/common/operations.o

AtlasFileLoadertest_SOURCES = AtlasFileLoadertest.cpp
AtlasFileLoadertest_LDADD = \
        $(top_builddir)/common/AtlasFileLoader.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/prefix.o \
        $(top_builddir)/common/globals.o

BaseWorldtest_SOURCES = BaseWorldtest.cpp
BaseWorldtest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(MATH_LIBS)

Databasetest_SOURCES = Databasetest.cpp
Databasetest_LDADD = \
        $(top_builddir)/common/Database.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/id.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/prefix.o \
        $(top_builddir)/common/globals.o

idtest_SOURCES = idtest.cpp
idtest_LDADD = \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/prefix.o \
        $(top_builddir)/common/globals.o \
        $(top_builddir)/common/id.o

AccountBasetest_SOURCES = AccountBasetest.cpp
AccountBasetest_LDADD = \
        $(top_builddir)/common/AccountBase.o \
        $(top_builddir)/common/Database.o \
        $(top_builddir)/common/system.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/id.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/prefix.o \
        $(top_builddir)/common/globals.o

debugtest_SOURCES = debugtest.cpp
debugtest_LDADD = $(top_builddir)/common/debug.o

globalstest_SOURCES = globalstest.cpp
globalstest_LDADD = \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/prefix.o \
        $(top_builddir)/common/globals.o

OperationRoutertest_SOURCES = OperationRoutertest.cpp
OperationRoutertest_LDADD = $(top_builddir)/common/OperationRouter.o

Routertest_SOURCES = Routertest.cpp
Routertest_LDADD = \
        $(top_builddir)/common/Router.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/prefix.o \
        $(top_builddir)/common/globals.o

client_sockettest_SOURCES = client_sockettest.cpp
client_sockettest_LDADD = \
        $(top_builddir)/common/client_socket.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/globals.o

customtest_SOURCES = customtest.cpp
customtest_LDADD = \
        $(top_builddir)/common/custom.o \
        $(top_builddir)/common/Inheritance.o \
        $(top_builddir)/common/TypeNode.o \
        $(top_builddir)/common/operations.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/globals.o

Monitorstest_SOURCES = Monitorstest.cpp
Monitorstest_LDADD = $(top_builddir)/common/Monitors.o

operationstest_SOURCES = operationstest.cpp
operationstest_LDADD = $(top_builddir)/common/const.o

serialnotest_SOURCES = serialnotest.cpp

newidtest_SOURCES = newidtest.cpp
newidtest_LDADD = \
        $(top_builddir)/common/newid.o \
        $(top_builddir)/common/Database.o \
        $(top_builddir)/common/id.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/globals.o

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
        $(top_builddir)/modules/Location.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/prefix.o \
        $(top_builddir)/common/globals.o \
        $(MATH_LIBS)

emergencetest_SOURCES = emergencetest.cpp
emergencetest_LDADD = \
        $(top_builddir)/physics/Collision.o \
        $(top_builddir)/physics/BBox.o \
        $(top_builddir)/physics/Vector3D.o \
        $(top_builddir)/modules/Location.o \
        $(top_builddir)/common/log.o \
        $(top_builddir)/common/const.o \
        $(top_builddir)/common/prefix.o \
        $(top_builddir)/common/globals.o \
        $(MATH_LIBS)

transformtest_SOURCES = transformtest.cpp
transformtest_LDADD = $(MATH_LIBS)

distancetest_SOURCES = distancetest.cpp
distancetest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(MATH_LIBS)

# MODULE_TESTS

DateTimetest_SOURCES = DateTimetest.cpp
DateTimetest_LDADD = $(top_builddir)/modules/DateTime.o

Locationtest_SOURCES = Locationtest.cpp
Locationtest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(MATH_LIBS)

WorldTimetest_SOURCES = WorldTimetest.cpp
WorldTimetest_LDADD = \
        $(top_builddir)/modules/WorldTime.o \
        $(top_builddir)/modules/DateTime.o

EntityReftest_SOURCES = EntityReftest.cpp
EntityReftest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(MATH_LIBS)

# RULESETS_TESTS

LocatedEntitytest_SOURCES = LocatedEntitytest.cpp
LocatedEntitytest_LDADD = \
        $(top_builddir)/rulesets/LocatedEntity.o \
        $(top_builddir)/rulesets/EntityProperties.o \
        $(top_builddir)/rulesets/AtlasProperties.o \
        $(top_builddir)/rulesets/Script.o \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a

Entitytest_SOURCES = Entitytest.cpp TestPropertyManager.cpp TestPropertyManager.h IGEntityExerciser.h EntityExerciser.h allOperations.h
Entitytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(MATH_LIBS)

Planttest_SOURCES = Planttest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Planttest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(MATH_LIBS)

Stackabletest_SOURCES = Stackabletest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Stackabletest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(MATH_LIBS)

Thingtest_SOURCES = Thingtest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Thingtest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(MATH_LIBS)

Worldtest_SOURCES = Worldtest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h TestWorld.h
Worldtest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Charactertest_SOURCES = Charactertest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Charactertest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)
Charactertest_LDFLAGS = $(PYTHON_LINKER_FLAGS)

Creatortest_SOURCES = Creatortest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h TestWorld.h
Creatortest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)
Creatortest_LDFLAGS = $(PYTHON_LINKER_FLAGS)

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
        $(top_builddir)/rulesets/Python_API.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)
Tasktest_LDFLAGS = $(PYTHON_LINKER_FLAGS)

EntityPropertytest_SOURCES = EntityPropertytest.cpp
EntityPropertytest_LDADD = \
        $(top_builddir)/rulesets/Python_API.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

AllPropertytest_SOURCES = AllPropertytest.cpp PropertyExerciser.cpp \
        PropertyExerciser.h
AllPropertytest_LDADD = \
        $(top_builddir)/rulesets/Python_API.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
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
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

BBoxPropertytest_SOURCES = BBoxPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
BBoxPropertytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

CalendarPropertytest_SOURCES = CalendarPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
CalendarPropertytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

LinePropertytest_SOURCES = LinePropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
LinePropertytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

MindPropertytest_SOURCES = MindPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
MindPropertytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

OutfitPropertytest_SOURCES = OutfitPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
OutfitPropertytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

SolidPropertytest_SOURCES = SolidPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
SolidPropertytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

StatisticsPropertytest_SOURCES = StatisticsPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
StatisticsPropertytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

StatusPropertytest_SOURCES = StatusPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
StatusPropertytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

TerrainModPropertytest_SOURCES = TerrainModPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TerrainModPropertytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

TerrainPropertytest_SOURCES = TerrainPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TerrainPropertytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

TransientPropertytest_SOURCES = TransientPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TransientPropertytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

TasksPropertytest_SOURCES = TasksPropertytest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
TasksPropertytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

InternalPropertiestest_SOURCES = InternalPropertiestest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
InternalPropertiestest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

EntityPropertiestest_SOURCES = EntityPropertiestest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
EntityPropertiestest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

AtlasPropertiestest_SOURCES = AtlasPropertiestest.cpp \
        PropertyCoverage.cpp PropertyCoverage.h
AtlasPropertiestest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Domaintest_SOURCES = Domaintest.cpp
Domaintest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

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
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

MemMaptest_SOURCES = MemMaptest.cpp
MemMaptest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Movementtest_SOURCES = Movementtest.cpp
Movementtest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Pedestriantest_SOURCES = Pedestriantest.cpp
Pedestriantest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

ActivePropertyFactorytest_SOURCES = ActivePropertyFactorytest.cpp
ActivePropertyFactorytest_LDADD = \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Python_APItest_SOURCES = Python_APItest.cpp
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

Py_BBoxtest_SOURCES = Py_BBoxtest.cpp
Py_BBoxtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Locationtest_SOURCES = Py_Locationtest.cpp
Py_Locationtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_RootEntitytest_SOURCES = Py_RootEntitytest.cpp
Py_RootEntitytest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Operationtest_SOURCES = Py_Operationtest.cpp
Py_Operationtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Oplisttest_SOURCES = Py_Oplisttest.cpp
Py_Oplisttest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Thingtest_SOURCES = Py_Thingtest.cpp
Py_Thingtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Maptest_SOURCES = Py_Maptest.cpp
Py_Maptest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Mindtest_SOURCES = Py_Mindtest.cpp
Py_Mindtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Tasktest_SOURCES = Py_Tasktest.cpp
Py_Tasktest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Worldtest_SOURCES = Py_Worldtest.cpp
Py_Worldtest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_Messagetest_SOURCES = Py_Messagetest.cpp
Py_Messagetest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

Py_WorldTimetest_SOURCES = Py_WorldTimetest.cpp
Py_WorldTimetest_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS)

# CLIENT_TESTS

Py_CreatorClienttest_SOURCES = Py_CreatorClienttest.cpp
Py_CreatorClienttest_LDADD = \
        $(top_builddir)/client/Py_CreatorClient.o \
        $(top_builddir)/client/CreatorClient.o \
        $(top_builddir)/client/CharacterClient.o \
        $(top_builddir)/client/ClientConnection.o \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)


# SERVER_TESTS

EntityBuildertest_SOURCES = EntityBuildertest.cpp
EntityBuildertest_LDADD = \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/Persistence.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/server/Player.o \
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
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)
EntityBuildertest_LDFLAGS = $(PYTHON_LINKER_FLAGS)

PropertyFlagtest_SOURCES = PropertyFlagtest.cpp
PropertyFlagtest_LDADD = \
        $(top_builddir)/server/EntityBuilder.o \
        $(top_builddir)/server/CorePropertyManager.o \
        $(top_builddir)/server/EntityFactory.o \
        $(top_builddir)/server/Persistence.o \
        $(top_builddir)/server/Account.o \
        $(top_builddir)/server/Admin.o \
        $(top_builddir)/server/Player.o \
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
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a \
        $(TERRAIN_LIBS) $(NETWORK_LIBS)
PropertyFlagtest_LDFLAGS = $(PYTHON_LINKER_FLAGS)

python_class_SOURCES = python_class.cpp
python_class_LDADD = \
        $(top_builddir)/rulesets/libscriptpython.a \
        $(top_builddir)/rulesets/librulesetentity.a \
        $(top_builddir)/rulesets/librulesetmind.a \
        $(top_builddir)/rulesets/librulesetbase.a \
        $(top_builddir)/modules/libmodules.a \
        $(top_builddir)/physics/libphysics.a \
        $(top_builddir)/common/libcommon.a
