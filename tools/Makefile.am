bin_PROGRAMS = cypasswd cycmd cydbload cyloadrules cywatchdog cyctrl

INCLUDES = -DBINDIR=\"@bindir@\" -DVARDIR=\"@localstatedir@\"

if BUILD_STATIC

LDFLAGS = -all-static

# LIBS = -ldb_cxx-3.2 -ldb-3.2 /usr/local/lib/libAtlasObjects.a /usr/local/lib/libAtlasObjectsOperation.a /usr/local/lib/libAtlasObjectsEntity.a /usr/local/lib/libAtlasNet.a /usr/local/lib/libAtlas.a /usr/local/lib/libAtlasCodecs.a /usr/local/lib/libAtlasMessage.a /usr/local/lib/libAtlas.a  -lreadline -ltermcap
# LIBS = /usr/lib/libdb_cxx-3.2.a /usr/lib/libdb-3.2.a -lAtlasObjects -lAtlasObjectsOperation -lAtlasObjectsEntity -lAtlasNet -lAtlas -lAtlasCodecs -lAtlasMessage -lAtlas -lreadline -ltermcap -lskstream

else

# LIBS = -ldb_cxx -lAtlas -lAtlasCodecs -lAtlasMessage -lAtlasNet -lAtlasObjects -lAtlasObjectsEntity -lAtlasObjectsOperation -lreadline -ltermcap -lskstream

endif

cypasswd_SOURCES = cypasswd.cpp sstream.h iotraits.h

cypasswd_LDADD = ../common/Database.o ../common/globals.o

cycmd_SOURCES = cycmd.cpp

cycmd_LIBS = -lAtlas -lAtlasCodecs -lAtlasMessage -lAtlasNet -lAtlasObjects -lAtlasObjectsEntity -lAtlasObjectsOperation

cycmd_LDADD = ../common/Database.o ../common/operations.o ../common/globals.o

cydbload_SOURCES = cydbload.cpp

cydbload_LDADD = ../common/Database.o ../common/globals.o

cyloadrules_SOURCES = cyloadrules.cpp

cyloadrules_LDADD = ../common/Database.o ../common/globals.o

cywatchdog_SOURCES = cywatchdog.cpp

cyctrl_SOURCES = cyctrl.cpp

EXTRA_DIST = sstream.cc
