TESTS = logtest BaseEntitytest

check_PROGRAMS = $(TESTS)

logtest_SOURCES = logtest.cpp
logtest_LDADD = ../common/log.o ../common/globals.o

BaseEntitytest_SOURCES = BaseEntitytest.cpp EntityExerciser.h
BaseEntitytest_LDADD = ../common/BaseEntity.o ../common/operations.o \
                       ../common/log.o ../common/globals.o ../common/serialno.o
