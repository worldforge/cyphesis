INCLUDES = -I$(top_srcdir)

bin_PROGRAMS = cyphesis

cyphesis_SOURCES = \
		ExternalMind.cpp ExternalMind.h \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		Persistance.cpp Persistance.h \
		ServerRouting.cpp ServerRouting.h ServerRouting_methods.h \
		WorldRouter.cpp WorldRouter.h \
		EntityFactory.cpp EntityFactory.h \
		Lobby.cpp Lobby.h \
		Connection.cpp Connection.h Connection_methods.h \
		CommServer.cpp CommServer.h \
		CommClient.cpp CommClient.h \
		protocol_instructions.h \
		server.cpp server.h

cyphesis_LDADD = ../rulesets/libscriptpython.a ../rulesets/librulesetbase.a ../rulesets/librulesetmind.a ../rulesets/librulesetentity.a ../rulesets/librulesetbase.a ../modules/libmodules.a ../common/libcommon.a ../physics/libphysics.a

cyphesis_LDFLAGS = -export-dynamic
