TESTS = logtest BaseEntitytest inheritancetest OOGThingtest \
        systemtest utilitytest

check_PROGRAMS = $(TESTS)

logtest_SOURCES = logtest.cpp
logtest_LDADD = ../common/log.o ../common/globals.o

BaseEntitytest_SOURCES = BaseEntitytest.cpp EntityExerciser.h
BaseEntitytest_LDADD = ../common/BaseEntity.o ../common/operations.o \
                       ../common/log.o ../common/globals.o ../common/serialno.o

inheritancetest_SOURCES = inheritancetest.cpp
inheritancetest_LDADD = ../common/inheritance.o ../common/custom.o \
                        ../common/operations.o ../common/log.o \
                        ../common/globals.o

OOGThingtest_SOURCES = OOGThingtest.cpp EntityExerciser.h
OOGThingtest_LDADD = ../common/OOGThing.o ../common/BaseEntity.o \
                     ../common/operations.o ../common/log.o \
                     ../common/globals.o ../common/serialno.o

systemtest_SOURCES = systemtest.cpp
systemtest_LDADD = ../common/system.o ../common/log.o ../common/globals.o

utilitytest_SOURCES = utilitytest.cpp
utilitytest_LDADD = ../common/utility.o

