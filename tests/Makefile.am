COMMON_TESTS = logtest inheritancetest Propertytest BaseEntitytest \
               OOGThingtest systemtest type_utilstest \
               consttest randomtest

PHYSICS_TESTS = transformtest collisiontest emergencetest distancetest

MODULE_TESTS = Locationtest WorldTimetest EntityReftest

RULESETS_TESTS = Entitytest Areatest Foodtest Linetest Planttest \
                 Stackabletest Structuretest Thingtest Worldtest \
                 Charactertest Creatortest Containertest Tasktest \
                 EntityPropertytest

SERVER_TESTS = EntityFactorytest

PYTHON_TESTS = python_class

INCLUDES = -I$(top_srcdir)

LIBS = $(COMMON_LIBS) $(PGSQL_LIBS) $(PYTHON_LIBS) $(PYTHON_UTIL_LIBS) $(MATH_LIBS)

TESTS = $(COMMON_TESTS) $(PHYSICS_TESTS) $(MODULE_TESTS) $(RULESETS_TESTS) $(SERVER_TESTS)

EXTRA_PROGRAMS = $(PYTHON_TESTS)

check_PROGRAMS = $(TESTS)

# COMMON_TESTS

logtest_SOURCES = logtest.cpp
logtest_LDADD = $(top_builddir)/modules/DateTime.o \
                $(top_builddir)/common/log.o \
                $(top_builddir)/common/prefix.o \
                $(top_builddir)/common/globals.o

inheritancetest_SOURCES = inheritancetest.cpp
inheritancetest_LDADD = $(top_builddir)/modules/DateTime.o \
                        $(top_builddir)/common/inheritance.o \
                        $(top_builddir)/common/custom.o \
                        $(top_builddir)/common/operations.o \
                        $(top_builddir)/common/log.o \
                        $(top_builddir)/common/prefix.o \
                        $(top_builddir)/common/globals.o

Propertytest_SOURCES = Propertytest.cpp
Propertytest_LDADD = $(top_builddir)/common/Property.o

BaseEntitytest_SOURCES = BaseEntitytest.cpp EntityExerciser.h allOperations.h
BaseEntitytest_LDADD = $(top_builddir)/modules/DateTime.o \
                       $(top_builddir)/common/BaseEntity.o \
                       $(top_builddir)/common/operations.o \
                       $(top_builddir)/common/inheritance.o \
                       $(top_builddir)/common/custom.o \
                       $(top_builddir)/common/log.o \
                       $(top_builddir)/common/prefix.o \
                       $(top_builddir)/common/globals.o \
                       $(top_builddir)/common/serialno.o

OOGThingtest_SOURCES = OOGThingtest.cpp EntityExerciser.h allOperations.h
OOGThingtest_LDADD = $(top_builddir)/modules/DateTime.o \
                     $(top_builddir)/common/OOGThing.o \
                     $(top_builddir)/common/BaseEntity.o \
                     $(top_builddir)/common/inheritance.o \
                     $(top_builddir)/common/custom.o \
                     $(top_builddir)/common/operations.o \
                     $(top_builddir)/common/log.o \
                     $(top_builddir)/common/prefix.o \
                     $(top_builddir)/common/globals.o \
                     $(top_builddir)/common/serialno.o

systemtest_SOURCES = systemtest.cpp
systemtest_LDADD = $(top_builddir)/modules/DateTime.o \
                   $(top_builddir)/common/system.o \
                   $(top_builddir)/common/log.o \
                   $(top_builddir)/common/prefix.o \
                   $(top_builddir)/common/globals.o \
                   $(NETWORK_LIBS)

type_utilstest_SOURCES = type_utilstest.cpp

consttest_SOURCES = consttest.cpp
consttest_LDADD = $(top_builddir)/common/const.o

randomtest_SOURCES = randomtest.cpp

# PHYSICS_TESTS

collisiontest_SOURCES = collisiontest.cpp
collisiontest_LDADD = $(top_builddir)/physics/Collision.o \
                      $(top_builddir)/physics/BBox.o \
                      $(top_builddir)/modules/Location.o \
                      $(top_builddir)/modules/DateTime.o \
                      $(top_builddir)/common/log.o \
                      $(top_builddir)/common/const.o \
                      $(top_builddir)/common/prefix.o \
                      $(top_builddir)/common/globals.o \
                      $(MATH_LIBS)

emergencetest_SOURCES = emergencetest.cpp
emergencetest_LDADD = $(top_builddir)/physics/Collision.o \
                      $(top_builddir)/physics/BBox.o \
                      $(top_builddir)/modules/Location.o \
                      $(top_builddir)/modules/DateTime.o \
                      $(top_builddir)/common/log.o \
                      $(top_builddir)/common/const.o \
                      $(top_builddir)/common/prefix.o \
                      $(top_builddir)/common/globals.o \
                      $(MATH_LIBS)

transformtest_SOURCES = transformtest.cpp
transformtest_LDADD = $(MATH_LIBS)

distancetest_SOURCES = distancetest.cpp
distancetest_LDADD = $(top_builddir)/rulesets/Entity.o \
                     $(top_builddir)/rulesets/EntityProperties.o \
                     $(top_builddir)/rulesets/Script.o \
                     $(top_builddir)/modules/Location.o \
                     $(top_builddir)/modules/DateTime.o \
                     $(top_builddir)/common/libcommon.a \
                     $(MATH_LIBS)

# MODULE_TESTS

Locationtest_SOURCES = Locationtest.cpp
Locationtest_LDADD = $(top_builddir)/modules/Location.o \
                     $(top_builddir)/modules/DateTime.o \
                     $(top_builddir)/rulesets/Entity.o \
                     $(top_builddir)/rulesets/EntityProperties.o \
                     $(top_builddir)/rulesets/Script.o \
                     $(top_builddir)/common/libcommon.a \
                     $(MATH_LIBS)

WorldTimetest_SOURCES = WorldTimetest.cpp
WorldTimetest_LDADD = $(top_builddir)/modules/WorldTime.o \
                      $(top_builddir)/modules/DateTime.o

EntityReftest_SOURCES = EntityReftest.cpp
EntityReftest_LDADD = $(top_builddir)/modules/EntityRef.o \
                      $(top_builddir)/modules/Location.o \
                      $(top_builddir)/modules/DateTime.o \
                      $(top_builddir)/rulesets/Entity.o \
                      $(top_builddir)/rulesets/EntityProperties.o \
                      $(top_builddir)/rulesets/Script.o \
                      $(top_builddir)/common/libcommon.a \
                      $(MATH_LIBS)

# RULESETS_TESTS

Entitytest_SOURCES = Entitytest.cpp TestPropertyManager.cpp TestPropertyManager.h IGEntityExerciser.h EntityExerciser.h allOperations.h
Entitytest_LDADD = $(top_builddir)/modules/Location.o \
                   $(top_builddir)/modules/DateTime.o \
                   $(top_builddir)/rulesets/Entity.o \
                   $(top_builddir)/rulesets/EntityProperties.o \
                   $(top_builddir)/rulesets/Script.o \
                   $(top_builddir)/common/libcommon.a \
                   $(MATH_LIBS)

Areatest_SOURCES = Areatest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Areatest_LDADD = $(top_builddir)/modules/Location.o \
                 $(top_builddir)/modules/DateTime.o \
                 $(top_builddir)/physics/libphysics.a \
                 $(top_builddir)/rulesets/Area.o \
                 $(top_builddir)/rulesets/Entity.o \
                 $(top_builddir)/rulesets/EntityProperties.o \
                 $(top_builddir)/rulesets/Thing.o \
                 $(top_builddir)/rulesets/Motion.o \
                 $(top_builddir)/rulesets/Script.o \
                 $(top_builddir)/common/libcommon.a \
                 $(MATH_LIBS)

Foodtest_SOURCES = Foodtest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Foodtest_LDADD = $(top_builddir)/modules/Location.o \
                 $(top_builddir)/modules/DateTime.o \
                 $(top_builddir)/physics/libphysics.a \
                 $(top_builddir)/rulesets/Food.o \
                 $(top_builddir)/rulesets/Entity.o \
                 $(top_builddir)/rulesets/EntityProperties.o \
                 $(top_builddir)/rulesets/Thing.o \
                 $(top_builddir)/rulesets/Motion.o \
                 $(top_builddir)/rulesets/Script.o \
                 $(top_builddir)/common/libcommon.a \
                 $(MATH_LIBS)

Linetest_SOURCES = Linetest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Linetest_LDADD = $(top_builddir)/modules/Location.o \
                 $(top_builddir)/modules/DateTime.o \
                 $(top_builddir)/physics/libphysics.a \
                 $(top_builddir)/rulesets/Line.o \
                 $(top_builddir)/rulesets/LineProperty.o \
                 $(top_builddir)/rulesets/Entity.o \
                 $(top_builddir)/rulesets/EntityProperties.o \
                 $(top_builddir)/rulesets/Thing.o \
                 $(top_builddir)/rulesets/Motion.o \
                 $(top_builddir)/rulesets/Script.o \
                 $(top_builddir)/common/libcommon.a \
                 $(MATH_LIBS)

Planttest_SOURCES = Planttest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Planttest_LDADD = $(top_builddir)/modules/Location.o \
                  $(top_builddir)/modules/DateTime.o \
                  $(top_builddir)/physics/libphysics.a \
                  $(top_builddir)/rulesets/Plant.o \
                  $(top_builddir)/rulesets/Entity.o \
                  $(top_builddir)/rulesets/EntityProperties.o \
                  $(top_builddir)/rulesets/Thing.o \
                  $(top_builddir)/rulesets/Motion.o \
                  $(top_builddir)/rulesets/Script.o \
                  $(top_builddir)/common/libcommon.a \
                  $(MATH_LIBS)

Stackabletest_SOURCES = Stackabletest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Stackabletest_LDADD = $(top_builddir)/modules/Location.o \
                      $(top_builddir)/modules/DateTime.o \
                      $(top_builddir)/physics/libphysics.a \
                      $(top_builddir)/rulesets/Stackable.o \
                      $(top_builddir)/rulesets/Entity.o \
                      $(top_builddir)/rulesets/EntityProperties.o \
                      $(top_builddir)/rulesets/Thing.o \
                      $(top_builddir)/rulesets/Motion.o \
                      $(top_builddir)/rulesets/Script.o \
                      $(top_builddir)/common/libcommon.a \
                      $(MATH_LIBS)

Structuretest_SOURCES = Structuretest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Structuretest_LDADD = $(top_builddir)/modules/Location.o \
                      $(top_builddir)/modules/DateTime.o \
                      $(top_builddir)/physics/libphysics.a \
                      $(top_builddir)/rulesets/Structure.o \
                      $(top_builddir)/rulesets/Entity.o \
                      $(top_builddir)/rulesets/EntityProperties.o \
                      $(top_builddir)/rulesets/Thing.o \
                      $(top_builddir)/rulesets/Motion.o \
                      $(top_builddir)/rulesets/Script.o \
                      $(top_builddir)/common/libcommon.a \
                      $(MATH_LIBS)

Thingtest_SOURCES = Thingtest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Thingtest_LDADD = $(top_builddir)/modules/Location.o \
                  $(top_builddir)/modules/DateTime.o \
                  $(top_builddir)/physics/libphysics.a \
                  $(top_builddir)/rulesets/Entity.o \
                  $(top_builddir)/rulesets/EntityProperties.o \
                  $(top_builddir)/rulesets/Thing.o \
                  $(top_builddir)/rulesets/Motion.o \
                  $(top_builddir)/rulesets/Script.o \
                  $(top_builddir)/common/libcommon.a \
                  $(MATH_LIBS)

Worldtest_SOURCES = Worldtest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h TestWorld.h
Worldtest_LDADD = $(top_builddir)/modules/Location.o \
                  $(top_builddir)/modules/DateTime.o \
                  $(top_builddir)/physics/libphysics.a \
                  $(top_builddir)/rulesets/Entity.o \
                  $(top_builddir)/rulesets/EntityProperties.o \
                  $(top_builddir)/rulesets/World.o \
                  $(top_builddir)/rulesets/TerrainProperty.o \
                  $(top_builddir)/rulesets/CalendarProperty.o \
                  $(top_builddir)/rulesets/Thing.o \
                  $(top_builddir)/rulesets/Motion.o \
                  $(top_builddir)/rulesets/Script.o \
                  $(top_builddir)/common/libcommon.a \
                  $(TERRAIN_LIBS)

Charactertest_SOURCES = Charactertest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Charactertest_LDADD = $(top_builddir)/rulesets/libscriptpython.a \
                      $(top_builddir)/rulesets/librulesetentity.a \
                      $(top_builddir)/rulesets/librulesetmind.a \
                      $(top_builddir)/rulesets/librulesetbase.a \
                      $(top_builddir)/modules/libmodules.a \
                      $(top_builddir)/physics/libphysics.a \
                      $(top_builddir)/common/libcommon.a \
                      $(MATH_LIBS)
Charactertest_LDFLAGS = -export-dynamic

Creatortest_SOURCES = Creatortest.cpp TestPropertyManager.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h TestWorld.h
Creatortest_LDADD = $(top_builddir)/rulesets/libscriptpython.a \
                    $(top_builddir)/rulesets/librulesetentity.a \
                    $(top_builddir)/rulesets/librulesetmind.a \
                    $(top_builddir)/rulesets/librulesetbase.a \
                    $(top_builddir)/modules/libmodules.a \
                    $(top_builddir)/physics/libphysics.a \
                    $(top_builddir)/common/libcommon.a \
                    $(MATH_LIBS)
Creatortest_LDFLAGS = -export-dynamic

Containertest_SOURCES = Containertest.cpp
Containertest_LDADD = $(top_builddir)/rulesets/Container.o

Tasktest_SOURCES = Tasktest.cpp Fell.cpp Fell.h
Tasktest_LDADD = $(top_builddir)/rulesets/libscriptpython.a \
                 $(top_builddir)/rulesets/librulesetentity.a \
                 $(top_builddir)/rulesets/librulesetmind.a \
                 $(top_builddir)/rulesets/librulesetbase.a \
                 $(top_builddir)/rulesets/Python_API.o \
                 $(top_builddir)/modules/libmodules.a \
                 $(top_builddir)/physics/libphysics.a \
                 $(top_builddir)/common/libcommon.a \
                 $(MATH_LIBS)
Tasktest_LDFLAGS = -export-dynamic

EntityPropertytest_SOURCES = EntityPropertytest.cpp
EntityPropertytest_LDADD = $(top_builddir)/rulesets/libscriptpython.a \
                           $(top_builddir)/rulesets/librulesetentity.a \
                           $(top_builddir)/rulesets/librulesetmind.a \
                           $(top_builddir)/rulesets/librulesetbase.a \
                           $(top_builddir)/rulesets/Python_API.o \
                           $(top_builddir)/modules/libmodules.a \
                           $(top_builddir)/physics/libphysics.a \
                           $(top_builddir)/common/libcommon.a \
                           $(MATH_LIBS)

EntityFactorytest_SOURCES = EntityFactorytest.cpp
EntityFactorytest_LDADD = $(top_builddir)/server/EntityFactory.o \
                          $(top_builddir)/server/CorePropertyManager.o \
                          $(top_builddir)/server/PersistantThingFactory.o \
                          $(top_builddir)/server/Persistor.o \
                          $(top_builddir)/server/Persistance.o \
                          $(top_builddir)/server/Account.o \
                          $(top_builddir)/server/Admin.o \
                          $(top_builddir)/server/Player.o \
                          $(top_builddir)/server/Connection.o \
                          $(top_builddir)/server/ExternalMind.o \
                          $(top_builddir)/server/Lobby.o \
                          $(top_builddir)/server/CommClient.o \
                          $(top_builddir)/server/Idle.o \
                          $(top_builddir)/server/CommSocket.o \
                          $(top_builddir)/server/CommPeer.o \
                          $(top_builddir)/server/Peer.o \
                          $(top_builddir)/server/CommServer.o \
                          $(top_builddir)/server/ScriptFactory.o \
                          $(top_builddir)/server/TaskFactory.o \
                          $(top_builddir)/server/ArithmeticFactory.o \
                          $(top_builddir)/rulesets/libscriptpython.a \
                          $(top_builddir)/rulesets/librulesetentity.a \
                          $(top_builddir)/rulesets/librulesetmind.a \
                          $(top_builddir)/rulesets/librulesetbase.a \
                          $(top_builddir)/modules/libmodules.a \
                          $(top_builddir)/physics/libphysics.a \
                          $(top_builddir)/common/libcommon.a \
                          $(TERRAIN_LIBS) $(NETWORK_LIBS)
EntityFactorytest_LDFLAGS = -export-dynamic

python_class_SOURCES = python_class.cpp
python_class_LDADD = $(top_builddir)/rulesets/libscriptpython.a \
                     $(top_builddir)/rulesets/librulesetentity.a \
                     $(top_builddir)/rulesets/librulesetmind.a \
                     $(top_builddir)/rulesets/librulesetbase.a \
                     $(top_builddir)/modules/libmodules.a \
                     $(top_builddir)/physics/libphysics.a \
                     $(top_builddir)/common/libcommon.a
