bin_PROGRAMS = cypasswd cycmd cydbload cyisoload

if BUILD_STATIC

LDFLAGS = -Wl,-static

LIBS = -ldb_cxx-3.1 -ldb-3.1 /usr/local/lib/libAtlasObjects.a /usr/local/lib/libAtlasObjectsOperation.a /usr/local/lib/libAtlasObjectsEntity.a /usr/local/lib/libAtlasNet.a /usr/local/lib/libAtlas.a /usr/local/lib/libAtlasCodecs.a /usr/local/lib/libAtlasMessage.a /usr/local/lib/libAtlas.a  -lreadline -ltermcap -lcoal -lcoaliso

else

LIBS = -ldb_cxx -lAtlas -lAtlasCodecs -lAtlasMessage -lAtlasNet -lAtlasObjects -lAtlasObjectsEntity -lAtlasObjectsOperation -lreadline -ltermcap -lcoal -lcoaliso -lskstream

endif

cypasswd_SOURCES = cypasswd.cpp sstream.h iotraits.h

cypasswd_LDADD = ../common/database.o

cycmd_SOURCES = cycmd.cpp

cycmd_LIBS = -lAtlas -lAtlasCodecs -lAtlasMessage -lAtlasNet -lAtlasObjects -lAtlasObjectsEntity -lAtlasObjectsOperation

cydbload_SOURCES = cydbload.cpp

cydbload_LDADD = ../common/database.o

cyisoload_SOURCES = cyisoload.cpp sstream.h iotraits.h

cyisoload_LDADD = ../common/database.o

EXTRA_DIST = sstream.cc
