bin_PROGRAMS = cyphesis

INCLUDES = -I.. -I/home/ajr/atlas-install/include
LIBS = -L/home/ajr/atlas-install/lib -lAtlas -lAtlasNet -lAtlasCodecs -lAtlasMessage

cyphesis_SOURCES = server.cpp server.h \
		ServerRouting.cpp ServerRouting.h \
		Routing.cpp Routing.h \
		WorldRouter.cpp WorldRouter.h \
		OOG_Thing.cpp OOG_Thing.h

cyphesis_LDADD = ../common/libcommon.a ../modules/libmodules.a ../physics/libphysics.a
