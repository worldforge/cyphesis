COMMON_TESTS = logtest inheritancetest Propertytest BaseEntitytest \
               OOGThingtest systemtest type_utilstest \
               consttest randomtest Addtest Attacktest Burntest Choptest \
               Connecttest Cuttest Delvetest Digtest Droptest Eattest \
               Monitortest Mowtest Nourishtest Pickuptest Setuptest \
               Ticktest Unseentest Updatetest AtlasFileLoadertest \
               BaseWorldtest Databasetest idtest accountbasetest \
               debugtest

PHYSICS_TESTS = transformtest collisiontest emergencetest distancetest

MODULE_TESTS = Locationtest WorldTimetest EntityReftest

RULESETS_TESTS = Entitytest Areatest Foodtest Linetest Planttest \
                 Stackabletest Structuretest Thingtest Worldtest \
                 Charactertest Creatortest ThingupdatePropertiestest \
                 Containertest Tasktest EntityPropertytest \
                 AllPropertytest

SERVER_TESTS = EntityFactorytest

PYTHON_TESTS = python_class

INCLUDES = -I$(top_srcdir) -I$(top_builddir)

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

Addtest_SOURCES = Addtest.cpp OperationExerciser.cpp OperationExerciser.h
Addtest_LDADD = $(top_builddir)/common/operations.o

Attacktest_SOURCES = Attacktest.cpp
Attacktest_LDADD = $(top_builddir)/common/operations.o

Burntest_SOURCES = Burntest.cpp
Burntest_LDADD = $(top_builddir)/common/operations.o

Choptest_SOURCES = Choptest.cpp
Choptest_LDADD = $(top_builddir)/common/operations.o

Connecttest_SOURCES = Connecttest.cpp
Connecttest_LDADD = $(top_builddir)/common/operations.o

Cuttest_SOURCES = Cuttest.cpp
Cuttest_LDADD = $(top_builddir)/common/operations.o

Delvetest_SOURCES = Delvetest.cpp
Delvetest_LDADD = $(top_builddir)/common/operations.o

Digtest_SOURCES = Digtest.cpp
Digtest_LDADD = $(top_builddir)/common/operations.o

Droptest_SOURCES = Droptest.cpp
Droptest_LDADD = $(top_builddir)/common/operations.o

Eattest_SOURCES = Eattest.cpp
Eattest_LDADD = $(top_builddir)/common/operations.o

Monitortest_SOURCES = Monitortest.cpp
Monitortest_LDADD = $(top_builddir)/common/operations.o

Mowtest_SOURCES = Mowtest.cpp
Mowtest_LDADD = $(top_builddir)/common/operations.o

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
AtlasFileLoadertest_LDADD = $(top_builddir)/common/AtlasFileLoader.o \
                            $(top_builddir)/modules/DateTime.o \
                            $(top_builddir)/common/log.o \
                            $(top_builddir)/common/prefix.o \
                            $(top_builddir)/common/globals.o

BaseWorldtest_SOURCES = BaseWorldtest.cpp
BaseWorldtest_LDADD = $(top_builddir)/modules/Location.o \
                      $(top_builddir)/modules/DateTime.o \
                      $(top_builddir)/rulesets/Entity.o \
                      $(top_builddir)/rulesets/EntityProperties.o \
                      $(top_builddir)/rulesets/AtlasProperties.o \
                      $(top_builddir)/rulesets/Script.o \
                      $(top_builddir)/common/libcommon.a \
                      $(MATH_LIBS)

Databasetest_SOURCES = Databasetest.cpp
Databasetest_LDADD = $(top_builddir)/common/Database.o \
                     $(top_builddir)/modules/DateTime.o \
                     $(top_builddir)/common/log.o \
                     $(top_builddir)/common/id.o \
                     $(top_builddir)/common/prefix.o \
                     $(top_builddir)/common/globals.o

idtest_SOURCES = idtest.cpp
idtest_LDADD = $(top_builddir)/modules/DateTime.o \
               $(top_builddir)/common/log.o \
               $(top_builddir)/common/prefix.o \
               $(top_builddir)/common/globals.o \
               $(top_builddir)/common/id.o

accountbasetest_SOURCES = accountbasetest.cpp
accountbasetest_LDADD = $(top_builddir)/common/accountbase.o \
                        $(top_builddir)/common/Database.o \
                        $(top_builddir)/modules/DateTime.o \
                        $(top_builddir)/common/system.o \
                        $(top_builddir)/common/log.o \
                        $(top_builddir)/common/id.o \
                        $(top_builddir)/common/prefix.o \
                        $(top_builddir)/common/globals.o

debugtest_SOURCES = debugtest.cpp
debugtest_LDADD = $(top_builddir)/common/debug.o

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
                     $(top_builddir)/rulesets/AtlasProperties.o \
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
                     $(top_builddir)/rulesets/AtlasProperties.o \
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
                      $(top_builddir)/rulesets/AtlasProperties.o \
                      $(top_builddir)/rulesets/Script.o \
                      $(top_builddir)/common/libcommon.a \
                      $(MATH_LIBS)

# RULESETS_TESTS

Entitytest_SOURCES = Entitytest.cpp TestPropertyManager.cpp TestPropertyManager.h IGEntityExerciser.h EntityExerciser.h allOperations.h
Entitytest_LDADD = $(top_builddir)/modules/Location.o \
                   $(top_builddir)/modules/DateTime.o \
                   $(top_builddir)/rulesets/Entity.o \
                   $(top_builddir)/rulesets/EntityProperties.o \
                   $(top_builddir)/rulesets/AtlasProperties.o \
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
                 $(top_builddir)/rulesets/AtlasProperties.o \
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
                 $(top_builddir)/rulesets/AtlasProperties.o \
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
                 $(top_builddir)/rulesets/AtlasProperties.o \
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
                  $(top_builddir)/rulesets/AtlasProperties.o \
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
                      $(top_builddir)/rulesets/AtlasProperties.o \
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
                      $(top_builddir)/rulesets/AtlasProperties.o \
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
                  $(top_builddir)/rulesets/AtlasProperties.o \
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
                  $(top_builddir)/rulesets/AtlasProperties.o \
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

ThingupdatePropertiestest_SOURCES = ThingupdatePropertiestest.cpp TestPropertyManager.cpp allOperations.h
ThingupdatePropertiestest_LDADD = $(top_builddir)/modules/Location.o \
                                  $(top_builddir)/modules/DateTime.o \
                                  $(top_builddir)/physics/libphysics.a \
                                  $(top_builddir)/rulesets/Entity.o \
                                  $(top_builddir)/rulesets/EntityProperties.o \
                                  $(top_builddir)/rulesets/AtlasProperties.o \
                                  $(top_builddir)/rulesets/Thing.o \
                                  $(top_builddir)/rulesets/Motion.o \
                                  $(top_builddir)/rulesets/Script.o \
                                  $(top_builddir)/common/libcommon.a \
                                  $(MATH_LIBS)

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

AllPropertytest_SOURCES = AllPropertytest.cpp PropertyExerciser.cpp \
                                              PropertyExerciser.h
AllPropertytest_LDADD = $(top_builddir)/rulesets/libscriptpython.a \
                        $(top_builddir)/rulesets/librulesetentity.a \
                        $(top_builddir)/rulesets/librulesetmind.a \
                        $(top_builddir)/rulesets/librulesetbase.a \
                        $(top_builddir)/rulesets/Python_API.o \
                        $(top_builddir)/modules/libmodules.a \
                        $(top_builddir)/physics/libphysics.a \
                        $(top_builddir)/common/libcommon.a \
                        $(TERRAIN_LIBS) $(MATH_LIBS)

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
                          $(top_builddir)/server/ExternalProperty.o \
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
