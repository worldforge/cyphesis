bin_PROGRAMS = cyphesis

INCLUDES = -I..
LIBS = -lAtlas -lAtlasNet -lAtlasCodecs -lAtlasMessage -lAtlasObjects -lAtlasObjectsOperation -lAtlasObjectsEntity -lpython1.5 -ldl -lm -lpthread -lvarconf # -lefence

cyphesis_SOURCES = \
		OOG_Thing.cpp OOG_Thing.h \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		Routing.cpp Routing.h \
		ServerRouting.cpp ServerRouting.h \
		WorldRouter.cpp WorldRouter.h \
		Connection.cpp Connection.h \
		WorldTime.h \
		CommClient.h  CommServer.h \
		server.cpp server.h

cyphesis_LDADD = ../physics/libphysics.a ../rulesets/librulesets.a ../modules/libmodules.a ../common/libcommon.a # LeakTracer.o

cyphesis_LDFLAGS = -Xlinker -export-dynamic
