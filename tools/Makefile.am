bin_PROGRAMS = cypasswd cycmd cydbload cyisoload

cypasswd_SOURCES = cypasswd.cpp sstream.h iotraits.h

cypasswd_LDADD = 

cycmd_SOURCES = cycmd.cpp

cycmd_LIBS = -lAtlas -lAtlasCodecs -lAtlasMessage -lAtlasNet -lAtlasObjects -lAtlasObjectsEntity -lAtlasObjectsOperation

cydbload_SOURCES = cydbload.cpp

cydbload_LDADD = ../common/database.o

cyisoload_SOURCES = cyisoload.cpp sstream.h iotraits.h

cyisoload_LDADD = ../common/database.o

EXTRA_DIST = sstream.cc
