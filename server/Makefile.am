bin_PROGRAMS = cyphesis

cyphesis_SOURCES = \
		ExternalMind.cpp ExternalMind.h \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		Persistance.cpp Persistance.h \
		Routing.cpp Routing.h \
		ServerRouting.cpp ServerRouting.h ServerRouting_methods.h \
		WorldRouter.cpp WorldRouter.h \
		Lobby.cpp Lobby.h \
		Connection.cpp Connection.h Connection_methods.h \
		CommServer.cpp CommServer.h \
		CommClient.cpp CommClient.h \
		protocol_instructions.h \
		server.cpp server.h

if BUILD_STATIC

cyphesis_LDADD = ../rulesets/librulesetbase.a ../rulesets/librulesetentity.a ../rulesets/librulesetmind.a ../rulesets/libscriptpython.a ../modules/libmodules.a ../common/libcommon.a ../physics/libphysics.a /usr/local/lib/libAtlasObjects.a /usr/local/lib/libAtlasObjectsOperation.a /usr/local/lib/libAtlasObjectsEntity.a /usr/local/lib/libAtlasNet.a /usr/local/lib/libAtlas.a /usr/local/lib/libAtlasCodecs.a /usr/local/lib/libAtlasMessage.a /usr/local/lib/libAtlas.a /usr/lib/libdb_cxx-3.1.a /usr/lib/libdb-3.1.a /usr/local/lib/libvarconf.a /usr/lib/libsigc.a /opt/forge/lib/python1.5/config/libpython1.5.a /usr/lib/libdl.a /usr/lib/libpthread.a /usr/lib/libstdc++-3-libc6.2-2-2.10.0.a -lc -lm /usr/lib/gcc-lib/i386-redhat-linux/2.96/libgcc.a

LIBS = # -lpython1.5 -ldl -lm -lpthread

cyphesis_LDFLAGS = -Wl,-export-dynamic -nodefaultlibs

else

cyphesis_LDADD = ../rulesets/libscriptpython.a ../rulesets/librulesetbase.a ../rulesets/librulesetmind.a ../rulesets/librulesetentity.a ../rulesets/librulesetbase.a ../modules/libmodules.a ../common/libcommon.a ../physics/libphysics.a

LIBS = -lAtlasObjects -lAtlasObjectsOperation -lAtlasObjectsEntity -lAtlasNet -lAtlas -lAtlasCodecs -lAtlasMessage -lAtlas -lpython1.5 -ldl -lm -lpthread -lvarconf -lsigc -ldb_cxx -lskstream # -lefence

cyphesis_LDFLAGS = -export-dynamic

endif
