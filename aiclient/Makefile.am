bin_PROGRAMS = cyphesisc

cyphesisc_SOURCES = \
		ClientConnection.cpp ClientConnection.h \
		ClientAccount.cpp ClientAccount.h \
		client.cpp client.h

if BUILD_STATIC

cyphesisc_LDADD = ../physics/libphysics.a ../rulesets/librulesetsc.a ../modules/libmodules.a ../common/libcommon.a /usr/local/lib/libAtlasObjects.a /usr/local/lib/libAtlasObjectsOperation.a /usr/local/lib/libAtlasObjectsEntity.a /usr/local/lib/libAtlasNet.a /usr/local/lib/libAtlas.a /usr/local/lib/libAtlasCodecs.a /usr/local/lib/libAtlasMessage.a /usr/local/lib/libAtlas.a /usr/local/lib/libvarconf.a /usr/local/lib/libsigc.a /usr/lib/libstdc++-3-libc6.1-2-2.10.0.a

LIBS = -lpython1.5 -ldl -lm -lpthread

cyphesisc_LDFLAGS = -Wl,-export-dynamic

else

cyphesisc_LDADD = ../rulesets/libscriptpython.a ../server/WorldRouter.o ../server/OOG_Thing.o ../rulesets/librulesetmind.a ../rulesets/librulesetentity.a ../rulesets/librulesetbase.a ../rulesets/libscriptpython.a ../modules/libmodules.a ../common/libcommon.a ../physics/libphysics.a

LIBS = -lAtlasObjects -lAtlasObjectsOperation -lAtlasObjectsEntity -lAtlasNet -lAtlas -lAtlasCodecs -lAtlasMessage -lAtlas -lpython1.5 -ldl -lm -lpthread -lvarconf -lsigc -ldb_cxx # -lefence

cyphesisc_LDFLAGS = -export-dynamic

endif
