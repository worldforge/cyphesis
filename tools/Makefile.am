bin_PROGRAMS = cycmd cypasswd cydbload cyloadrules cywatchdog cyctrl

INCLUDES = -DBINDIR=\"@bindir@\" -DVARDIR=\"@localstatedir@\"

cypasswd_SOURCES = cypasswd.cpp sstream.h iotraits.h

cypasswd_LDADD = ../common/Database.o ../common/accountbase.o ../common/globals.o

cycmd_SOURCES = cycmd.cpp

cycmd_LIBS = -lAtlas -lAtlasCodecs -lAtlasMessage -lAtlasNet -lAtlasObjects -lAtlasObjectsEntity -lAtlasObjectsOperation

cycmd_LDADD = ../common/Database.o ../common/accountbase.o ../common/operations.o ../common/globals.o

cydbload_SOURCES = cydbload.cpp

cydbload_LDADD = ../common/Database.o ../common/globals.o

cyloadrules_SOURCES = cyloadrules.cpp

cyloadrules_LDADD = ../common/Database.o ../common/globals.o

cywatchdog_SOURCES = cywatchdog.cpp

cyctrl_SOURCES = cyctrl.cpp

EXTRA_DIST = sstream.cc
