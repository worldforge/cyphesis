bin_PROGRAMS = cyphesis

INCLUDES = -I..
LIBS = -lAtlas -lAtlasNet -lAtlasCodecs -lAtlasMessage -lAtlasObjects -lAtlasObjectsOperation -lAtlasObjectsEntity -lpython1.5 -ldl -lpthread -lvarconf # -lefence

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
		server.cpp server.h

cyphesis_LDADD = ../modules/libmodules.a ../physics/libphysics.a ../rulesets/librulesets.a ../common/libcommon.a

cyphesis_LDFLAGS = -Xlinker -export-dynamic
