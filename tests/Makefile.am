COMMON_TESTS = logtest inheritancetest BaseEntitytest OOGThingtest \
               systemtest utilitytest

PHYSICS_TESTS = collisiontest

MODULE_TESTS = Locationtest

RULESETS_TESTS = Entitytest Areatest Foodtest Linetest Planttest \
                 Stackabletest Structuretest Thingtest Worldtest

TESTS = $(COMMON_TESTS) $(PHYSICS_TESTS) $(MODULE_TESTS) $(RULESETS_TESTS)

check_PROGRAMS = $(TESTS)

# COMMON_TESTS

logtest_SOURCES = logtest.cpp
logtest_LDADD = ../common/log.o ../common/globals.o

inheritancetest_SOURCES = inheritancetest.cpp
inheritancetest_LDADD = ../common/inheritance.o ../common/custom.o \
                        ../common/operations.o ../common/log.o \
                        ../common/globals.o

BaseEntitytest_SOURCES = BaseEntitytest.cpp EntityExerciser.h
BaseEntitytest_LDADD = ../common/BaseEntity.o ../common/operations.o \
                       ../common/inheritance.o ../common/custom.o \
                       ../common/log.o ../common/globals.o ../common/serialno.o

OOGThingtest_SOURCES = OOGThingtest.cpp EntityExerciser.h
OOGThingtest_LDADD = ../common/OOGThing.o ../common/BaseEntity.o \
                     ../common/inheritance.o ../common/custom.o \
                     ../common/operations.o ../common/log.o \
                     ../common/globals.o ../common/serialno.o

systemtest_SOURCES = systemtest.cpp
systemtest_LDADD = ../common/system.o ../common/log.o ../common/globals.o

utilitytest_SOURCES = utilitytest.cpp
utilitytest_LDADD = ../common/utility.o

# PHYSICS_TESTS

collisiontest_SOURCES = collisiontest.cpp
collisiontest_LDADD = ../common/utility.o


# MODULE_TESTS

Locationtest_SOURCES = Locationtest.cpp
Locationtest_LDADD = ../modules/Location.o ../rulesets/Entity.o \
                     ../rulesets/Script.o ../common/inheritance.o \
                     ../common/BaseEntity.o ../common/log.o \
                     ../common/globals.o ../common/serialno.o \
                     ../common/custom.o ../common/operations.o

# RULESETS_TESTS

Entitytest_SOURCES = Entitytest.cpp
Entitytest_LDADD = ../modules/Location.o ../rulesets/Entity.o \
                   ../rulesets/Script.o ../common/inheritance.o \
                   ../common/BaseEntity.o ../common/log.o \
                   ../common/globals.o ../common/serialno.o \
                   ../common/custom.o ../common/operations.o

Areatest_SOURCES = Areatest.cpp
Areatest_LDADD = ../modules/Location.o ../rulesets/Area.o \
                 ../rulesets/Entity.o ../rulesets/Thing.o \
                 ../rulesets/Script.o ../common/inheritance.o \
                 ../common/BaseEntity.o ../common/log.o \
                 ../common/globals.o ../common/serialno.o \
                 ../common/custom.o ../common/operations.o ../common/const.o

Foodtest_SOURCES = Foodtest.cpp
Foodtest_LDADD = ../modules/Location.o ../rulesets/Food.o \
                 ../rulesets/Entity.o ../rulesets/Thing.o \
                 ../rulesets/Script.o ../common/inheritance.o \
                 ../common/BaseEntity.o ../common/log.o \
                 ../common/globals.o ../common/serialno.o \
                 ../common/custom.o ../common/operations.o ../common/const.o

Linetest_SOURCES = Linetest.cpp
Linetest_LDADD = ../modules/Location.o ../rulesets/Line.o \
                 ../rulesets/Entity.o ../rulesets/Thing.o \
                 ../rulesets/Script.o ../common/inheritance.o \
                 ../common/BaseEntity.o ../common/log.o \
                 ../common/globals.o ../common/serialno.o \
                 ../common/custom.o ../common/operations.o ../common/const.o

Planttest_SOURCES = Planttest.cpp
Planttest_LDADD = ../modules/Location.o ../rulesets/Plant.o \
                  ../rulesets/Entity.o ../rulesets/Thing.o \
                  ../rulesets/Script.o ../common/inheritance.o \
                  ../common/BaseEntity.o ../common/log.o \
                  ../common/globals.o ../common/serialno.o \
                  ../common/custom.o ../common/operations.o ../common/const.o

Stackabletest_SOURCES = Stackabletest.cpp
Stackabletest_LDADD = ../modules/Location.o ../rulesets/Stackable.o \
                      ../rulesets/Entity.o ../rulesets/Thing.o \
                      ../rulesets/Script.o ../common/inheritance.o \
                      ../common/BaseEntity.o ../common/log.o \
                      ../common/globals.o ../common/serialno.o \
                      ../common/custom.o ../common/operations.o \
                      ../common/const.o

Structuretest_SOURCES = Structuretest.cpp
Structuretest_LDADD = ../modules/Location.o \
                      ../rulesets/Entity.o ../rulesets/Thing.o \
                      ../rulesets/Script.o ../common/inheritance.o \
                      ../common/BaseEntity.o ../common/log.o \
                      ../common/globals.o ../common/serialno.o \
                      ../common/custom.o ../common/operations.o \
                      ../common/const.o

Thingtest_SOURCES = Thingtest.cpp
Thingtest_LDADD = ../modules/Location.o \
                  ../rulesets/Entity.o ../rulesets/Thing.o \
                  ../rulesets/Script.o ../common/inheritance.o \
                  ../common/BaseEntity.o ../common/log.o \
                  ../common/globals.o ../common/serialno.o \
                  ../common/custom.o ../common/operations.o ../common/const.o

Worldtest_SOURCES = Worldtest.cpp
Worldtest_LDADD = ../modules/Location.o ../rulesets/World.o \
                  ../rulesets/Entity.o ../rulesets/Thing.o \
                  ../rulesets/Script.o ../common/inheritance.o \
                  ../common/BaseEntity.o ../common/log.o \
                  ../common/globals.o ../common/serialno.o \
                  ../common/custom.o ../common/operations.o \
                  ../common/OOGThing.o ../common/BaseWorld.o ../common/const.o

