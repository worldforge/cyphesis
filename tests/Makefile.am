COMMON_TESTS = logtest inheritancetest Propertytest BaseEntitytest \
               OOGThingtest systemtest utilitytest type_utilstest \
               consttest randomtest

PHYSICS_TESTS = transformtest collisiontest emergencetest distancetest

MODULE_TESTS = Locationtest WorldTimetest

RULESETS_TESTS = Entitytest Areatest Foodtest Linetest Planttest \
                 Stackabletest Structuretest Thingtest Worldtest \
                 Charactertest Creatortest Containertest Tasktest

INCLUDES = -I$(top_srcdir)

LIBS = $(COMMON_LIBS) $(PGSQL_LIBS) $(PYTHON_LIBS) $(PYTHON_UTIL_LIBS)

TESTS = $(COMMON_TESTS) $(PHYSICS_TESTS) $(MODULE_TESTS) $(RULESETS_TESTS)

check_PROGRAMS = $(TESTS)

# COMMON_TESTS

logtest_SOURCES = logtest.cpp
logtest_LDADD = $(top_builddir)/modules/DateTime.o \
                $(top_builddir)/common/log.o \
                $(top_builddir)/common/globals.o

inheritancetest_SOURCES = inheritancetest.cpp
inheritancetest_LDADD = $(top_builddir)/modules/DateTime.o \
                        $(top_builddir)/common/inheritance.o \
                        $(top_builddir)/common/custom.o \
                        $(top_builddir)/common/operations.o \
                        $(top_builddir)/common/log.o \
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
                     $(top_builddir)/common/globals.o \
                     $(top_builddir)/common/serialno.o

systemtest_SOURCES = systemtest.cpp
systemtest_LDADD = $(top_builddir)/modules/DateTime.o \
                   $(top_builddir)/common/system.o \
                   $(top_builddir)/common/log.o \
                   $(top_builddir)/common/globals.o

utilitytest_SOURCES = utilitytest.cpp
utilitytest_LDADD = $(top_builddir)/common/utility.o

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
                      $(top_builddir)/common/globals.o

emergencetest_SOURCES = emergencetest.cpp
emergencetest_LDADD = $(top_builddir)/physics/Collision.o \
                      $(top_builddir)/physics/BBox.o \
                      $(top_builddir)/modules/Location.o \
                      $(top_builddir)/modules/DateTime.o \
                      $(top_builddir)/common/log.o \
                      $(top_builddir)/common/const.o \
                      $(top_builddir)/common/globals.o

transformtest_SOURCES = transformtest.cpp

distancetest_SOURCES = distancetest.cpp
distancetest_LDADD = $(top_builddir)/rulesets/Entity.o \
                     $(top_builddir)/rulesets/EntityProperties.o \
                     $(top_builddir)/rulesets/Script.o \
                     $(top_builddir)/modules/Location.o \
                     $(top_builddir)/modules/DateTime.o \
                     $(top_builddir)/common/inheritance.o \
                     $(top_builddir)/common/custom.o \
                     $(top_builddir)/common/operations.o \
                     $(top_builddir)/common/serialno.o \
                     $(top_builddir)/common/log.o \
                     $(top_builddir)/common/globals.o \
                     $(top_builddir)/common/Property.o \
                     $(top_builddir)/common/BaseEntity.o

# MODULE_TESTS

Locationtest_SOURCES = Locationtest.cpp
Locationtest_LDADD = $(top_builddir)/modules/Location.o \
                     $(top_builddir)/modules/DateTime.o \
                     $(top_builddir)/rulesets/Entity.o \
                     $(top_builddir)/rulesets/EntityProperties.o \
                     $(top_builddir)/rulesets/Script.o \
                     $(top_builddir)/common/inheritance.o \
                     $(top_builddir)/common/BaseEntity.o \
                     $(top_builddir)/common/log.o \
                     $(top_builddir)/common/globals.o \
                     $(top_builddir)/common/serialno.o \
                     $(top_builddir)/common/custom.o \
                     $(top_builddir)/common/Property.o \
                     $(top_builddir)/common/operations.o

WorldTimetest_SOURCES = WorldTimetest.cpp
WorldTimetest_LDADD = $(top_builddir)/modules/WorldTime.o \
                      $(top_builddir)/modules/DateTime.o

# RULESETS_TESTS

Entitytest_SOURCES = Entitytest.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Entitytest_LDADD = $(top_builddir)/modules/Location.o \
                   $(top_builddir)/modules/DateTime.o \
                   $(top_builddir)/rulesets/Entity.o \
                   $(top_builddir)/rulesets/EntityProperties.o \
                   $(top_builddir)/rulesets/Script.o \
                   $(top_builddir)/common/inheritance.o \
                   $(top_builddir)/common/Property.o \
                   $(top_builddir)/common/BaseEntity.o \
                   $(top_builddir)/common/log.o \
                   $(top_builddir)/common/globals.o \
                   $(top_builddir)/common/serialno.o \
                   $(top_builddir)/common/custom.o \
                   $(top_builddir)/common/operations.o

Areatest_SOURCES = Areatest.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Areatest_LDADD = $(top_builddir)/modules/Location.o \
                 $(top_builddir)/modules/DateTime.o \
                 $(top_builddir)/physics/BBox.o \
                 $(top_builddir)/physics/Vector3D.o \
                 $(top_builddir)/rulesets/Area.o \
                 $(top_builddir)/rulesets/Entity.o \
                 $(top_builddir)/rulesets/EntityProperties.o \
                 $(top_builddir)/rulesets/Thing.o \
                 $(top_builddir)/rulesets/Script.o \
                 $(top_builddir)/common/inheritance.o \
                 $(top_builddir)/common/Property.o \
                 $(top_builddir)/common/BaseEntity.o \
                 $(top_builddir)/common/log.o \
                 $(top_builddir)/common/globals.o \
                 $(top_builddir)/common/serialno.o \
                 $(top_builddir)/common/custom.o \
                 $(top_builddir)/common/operations.o \
                 $(top_builddir)/common/const.o

Foodtest_SOURCES = Foodtest.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Foodtest_LDADD = $(top_builddir)/modules/Location.o \
                 $(top_builddir)/modules/DateTime.o \
                 $(top_builddir)/physics/BBox.o \
                 $(top_builddir)/physics/Vector3D.o \
                 $(top_builddir)/rulesets/Food.o \
                 $(top_builddir)/rulesets/Entity.o \
                 $(top_builddir)/rulesets/EntityProperties.o \
                 $(top_builddir)/rulesets/Thing.o \
                 $(top_builddir)/rulesets/Script.o \
                 $(top_builddir)/common/inheritance.o \
                 $(top_builddir)/common/Property.o \
                 $(top_builddir)/common/BaseEntity.o \
                 $(top_builddir)/common/log.o \
                 $(top_builddir)/common/globals.o \
                 $(top_builddir)/common/serialno.o \
                 $(top_builddir)/common/custom.o \
                 $(top_builddir)/common/operations.o \
                 $(top_builddir)/common/const.o

Linetest_SOURCES = Linetest.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Linetest_LDADD = $(top_builddir)/modules/Location.o \
                 $(top_builddir)/modules/DateTime.o \
                 $(top_builddir)/physics/BBox.o \
                 $(top_builddir)/physics/Vector3D.o \
                 $(top_builddir)/rulesets/Line.o \
                 $(top_builddir)/rulesets/LineProperty.o \
                 $(top_builddir)/rulesets/Entity.o \
                 $(top_builddir)/rulesets/EntityProperties.o \
                 $(top_builddir)/rulesets/Thing.o \
                 $(top_builddir)/rulesets/Script.o \
                 $(top_builddir)/common/inheritance.o \
                 $(top_builddir)/common/Property.o \
                 $(top_builddir)/common/BaseEntity.o \
                 $(top_builddir)/common/log.o \
                 $(top_builddir)/common/globals.o \
                 $(top_builddir)/common/serialno.o \
                 $(top_builddir)/common/custom.o \
                 $(top_builddir)/common/operations.o \
                 $(top_builddir)/common/const.o

Planttest_SOURCES = Planttest.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Planttest_LDADD = $(top_builddir)/modules/Location.o \
                  $(top_builddir)/modules/DateTime.o \
                  $(top_builddir)/physics/BBox.o \
                  $(top_builddir)/physics/Vector3D.o \
                  $(top_builddir)/rulesets/Plant.o \
                  $(top_builddir)/rulesets/Entity.o \
                  $(top_builddir)/rulesets/EntityProperties.o \
                  $(top_builddir)/rulesets/Thing.o \
                  $(top_builddir)/rulesets/Script.o \
                  $(top_builddir)/common/inheritance.o \
                  $(top_builddir)/common/Property.o \
                  $(top_builddir)/common/BaseEntity.o \
                  $(top_builddir)/common/log.o \
                  $(top_builddir)/common/globals.o \
                  $(top_builddir)/common/serialno.o \
                  $(top_builddir)/common/custom.o \
                  $(top_builddir)/common/operations.o \
                  $(top_builddir)/common/const.o

Stackabletest_SOURCES = Stackabletest.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Stackabletest_LDADD = $(top_builddir)/modules/Location.o \
                      $(top_builddir)/modules/DateTime.o \
                      $(top_builddir)/physics/BBox.o \
                      $(top_builddir)/physics/Vector3D.o \
                      $(top_builddir)/rulesets/Stackable.o \
                      $(top_builddir)/rulesets/Entity.o \
                      $(top_builddir)/rulesets/EntityProperties.o \
                      $(top_builddir)/rulesets/Thing.o \
                      $(top_builddir)/rulesets/Script.o \
                      $(top_builddir)/common/inheritance.o \
                      $(top_builddir)/common/Property.o \
                      $(top_builddir)/common/BaseEntity.o \
                      $(top_builddir)/common/log.o \
                      $(top_builddir)/common/globals.o \
                      $(top_builddir)/common/serialno.o \
                      $(top_builddir)/common/custom.o \
                      $(top_builddir)/common/operations.o \
                      $(top_builddir)/common/const.o

Structuretest_SOURCES = Structuretest.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Structuretest_LDADD = $(top_builddir)/modules/Location.o \
                      $(top_builddir)/modules/DateTime.o \
                      $(top_builddir)/physics/BBox.o \
                      $(top_builddir)/physics/Vector3D.o \
                      $(top_builddir)/rulesets/Structure.o \
                      $(top_builddir)/rulesets/Entity.o \
                      $(top_builddir)/rulesets/EntityProperties.o \
                      $(top_builddir)/rulesets/Thing.o \
                      $(top_builddir)/rulesets/Script.o \
                      $(top_builddir)/common/inheritance.o \
                      $(top_builddir)/common/Property.o \
                      $(top_builddir)/common/BaseEntity.o \
                      $(top_builddir)/common/log.o \
                      $(top_builddir)/common/globals.o \
                      $(top_builddir)/common/serialno.o \
                      $(top_builddir)/common/custom.o \
                      $(top_builddir)/common/operations.o \
                      $(top_builddir)/common/const.o

Thingtest_SOURCES = Thingtest.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Thingtest_LDADD = $(top_builddir)/modules/Location.o \
                  $(top_builddir)/modules/DateTime.o \
                  $(top_builddir)/physics/BBox.o \
                  $(top_builddir)/physics/Vector3D.o \
                  $(top_builddir)/rulesets/Entity.o \
                  $(top_builddir)/rulesets/EntityProperties.o \
                  $(top_builddir)/rulesets/Thing.o \
                  $(top_builddir)/rulesets/Script.o \
                  $(top_builddir)/common/inheritance.o \
                  $(top_builddir)/common/Property.o \
                  $(top_builddir)/common/BaseEntity.o \
                  $(top_builddir)/common/log.o \
                  $(top_builddir)/common/globals.o \
                  $(top_builddir)/common/serialno.o \
                  $(top_builddir)/common/custom.o \
                  $(top_builddir)/common/operations.o \
                  $(top_builddir)/common/const.o

Worldtest_SOURCES = Worldtest.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Worldtest_LDADD = $(top_builddir)/modules/Location.o \
                  $(top_builddir)/modules/DateTime.o \
                  $(top_builddir)/physics/BBox.o \
                  $(top_builddir)/physics/Vector3D.o \
                  $(top_builddir)/rulesets/Entity.o \
                  $(top_builddir)/rulesets/EntityProperties.o \
                  $(top_builddir)/rulesets/World.o \
                  $(top_builddir)/rulesets/TerrainProperty.o \
                  $(top_builddir)/rulesets/Thing.o \
                  $(top_builddir)/rulesets/Script.o \
                  $(top_builddir)/common/inheritance.o \
                  $(top_builddir)/common/Property.o \
                  $(top_builddir)/common/BaseEntity.o \
                  $(top_builddir)/common/log.o \
                  $(top_builddir)/common/globals.o \
                  $(top_builddir)/common/serialno.o \
                  $(top_builddir)/common/custom.o \
                  $(top_builddir)/common/operations.o \
                  $(top_builddir)/common/OOGThing.o \
                  $(top_builddir)/common/BaseWorld.o \
                  $(top_builddir)/common/const.o

Charactertest_SOURCES = Charactertest.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Charactertest_LDADD = $(top_builddir)/rulesets/librulesetentity.a \
                      $(top_builddir)/rulesets/librulesetmind.a \
                      $(top_builddir)/rulesets/librulesetbase.a \
                      $(top_builddir)/rulesets/libscriptpython.a \
                      $(top_builddir)/modules/libmodules.a \
                      $(top_builddir)/physics/libphysics.a \
                      $(top_builddir)/common/inheritance.o \
                      $(top_builddir)/common/Property.o \
                      $(top_builddir)/common/BaseEntity.o \
                      $(top_builddir)/common/log.o \
                      $(top_builddir)/common/globals.o \
                      $(top_builddir)/common/serialno.o \
                      $(top_builddir)/common/custom.o \
                      $(top_builddir)/common/operations.o \
                      $(top_builddir)/common/OOGThing.o \
                      $(top_builddir)/common/BaseWorld.o \
                      $(top_builddir)/common/utility.o \
                      $(top_builddir)/common/const.o
Charactertest_LDFLAGS = -export-dynamic

Creatortest_SOURCES = Creatortest.cpp IGEntityExerciser.h EntityExerciser.h allOperations.h
Creatortest_LDADD = $(top_builddir)/rulesets/librulesetentity.a \
                    $(top_builddir)/rulesets/librulesetmind.a \
                    $(top_builddir)/rulesets/librulesetbase.a \
                    $(top_builddir)/rulesets/libscriptpython.a \
                    $(top_builddir)/modules/libmodules.a \
                    $(top_builddir)/physics/libphysics.a \
                    $(top_builddir)/common/inheritance.o \
                    $(top_builddir)/common/Property.o \
                    $(top_builddir)/common/BaseEntity.o \
                    $(top_builddir)/common/log.o \
                    $(top_builddir)/common/globals.o \
                    $(top_builddir)/common/serialno.o \
                    $(top_builddir)/common/custom.o \
                    $(top_builddir)/common/operations.o \
                    $(top_builddir)/common/OOGThing.o \
                    $(top_builddir)/common/BaseWorld.o \
                    $(top_builddir)/common/utility.o \
                    $(top_builddir)/common/const.o
Creatortest_LDFLAGS = -export-dynamic

Containertest_SOURCES = Containertest.cpp
Containertest_LDADD = $(top_builddir)/rulesets/Container.o

Tasktest_SOURCES = Tasktest.cpp
Tasktest_LDADD = $(top_builddir)/rulesets/librulesetentity.a \
                 $(top_builddir)/rulesets/librulesetmind.a \
                 $(top_builddir)/rulesets/librulesetbase.a \
                 $(top_builddir)/rulesets/libscriptpython.a \
                 $(top_builddir)/modules/libmodules.a \
                 $(top_builddir)/physics/libphysics.a \
                 $(top_builddir)/common/inheritance.o \
                 $(top_builddir)/common/Property.o \
                 $(top_builddir)/common/BaseEntity.o \
                 $(top_builddir)/common/log.o \
                 $(top_builddir)/common/globals.o \
                 $(top_builddir)/common/serialno.o \
                 $(top_builddir)/common/custom.o \
                 $(top_builddir)/common/operations.o \
                 $(top_builddir)/common/OOGThing.o \
                 $(top_builddir)/common/BaseWorld.o \
                 $(top_builddir)/common/utility.o \
                 $(top_builddir)/common/const.o
Tasktest_LDFLAGS = -export-dynamic
