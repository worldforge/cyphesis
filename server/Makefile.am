bin_PROGRAMS = cyphesis

cyphesis_SOURCES = \
		OOG_Thing.cpp OOG_Thing.h \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		Routing.cpp Routing.h \
		ServerRouting.cpp ServerRouting.h \
		WorldRouter.cpp WorldRouter.h \
		Connection.cpp Connection.h \
		WorldTime.cpp WorldTime.h \
		CommClient.h  CommServer.h \
		server.cpp server.h

if BUILD_STATIC

cyphesis_LDADD = ../physics/libphysics.a ../rulesets/librulesets.a ../modules/libmodules.a ../common/libcommon.a /usr/local/lib/libAtlasObjects.a /usr/local/lib/libAtlasObjectsOperation.a /usr/local/lib/libAtlasObjectsEntity.a /usr/local/lib/libAtlasNet.a /usr/local/lib/libAtlas.a /usr/local/lib/libAtlasCodecs.a /usr/local/lib/libAtlasMessage.a /usr/local/lib/libAtlas.a /usr/local/lib/libvarconf.a /usr/local/lib/libsigc.a /usr/lib/libstdc++-3-libc6.1-2-2.10.0.a

LIBS = -lpython1.5 -ldl -lm -lpthread

cyphesis_LDFLAGS = -Wl,-export-dynamic

else

cyphesis_LDADD = ../physics/libphysics.a ../rulesets/librulesets.a ../modules/libmodules.a ../common/libcommon.a

LIBS = -lAtlasObjects -lAtlasObjectsOperation -lAtlasObjectsEntity -lAtlasNet -lAtlas -lAtlasCodecs -lAtlasMessage -lAtlas -lpython1.5 -ldl -lm -lpthread -lvarconf -lsigc -ldb_cxx # -lefence

cyphesis_LDFLAGS = -export-dynamic

endif
