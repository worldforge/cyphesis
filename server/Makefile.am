bin_PROGRAMS = cyphesis cyslave

cyphesis_SOURCES = \
		ExternalMind.cpp ExternalMind.h \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		Persistor.cpp Persistor.h \
		Persistance.cpp Persistance.h \
		PersistantThingFactory.cpp PersistantThingFactory.h \
		Restoration.cpp Restoration.h \
		Restorer.cpp Restorer_impl.h Restorer.h \
		ServerRouting.cpp ServerRouting.h ServerRouting_methods.h \
		WorldRouter.cpp WorldRouter.h \
		EntityFactory.cpp EntityFactory.h \
		Lobby.cpp Lobby.h \
		Connection.cpp Connection.h Connection_methods.h \
		CommSocket.cpp CommSocket.h \
		CommListener.cpp CommListener.h \
		CommClient.cpp CommClient.h \
		CommMetaClient.cpp CommMetaClient.h \
		CommServer.cpp CommServer.h \
		protocol_instructions.h \
		server.cpp server.h

cyphesis_LDADD = ../rulesets/libscriptpython.a ../rulesets/librulesetbase.a ../rulesets/librulesetmind.a ../rulesets/librulesetentity.a ../rulesets/librulesetbase.a ../modules/libmodules.a ../common/libcommon.a ../physics/libphysics.a

cyphesis_LDFLAGS = -export-dynamic

cyslave_SOURCES = \
		ExternalMind.cpp ExternalMind.h \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		Persistor.cpp Persistor.h \
		Persistance.cpp Persistance.h \
		PersistantThingFactory.cpp PersistantThingFactory.h \
		Restoration.cpp Restoration.h \
		Restorer.cpp Restorer_impl.h Restorer.h \
		ServerRouting.cpp ServerRouting.h ServerRouting_methods.h \
		WorldRouter.cpp WorldRouter.h \
		EntityFactory.cpp EntityFactory.h \
		Lobby.cpp Lobby.h \
		Connection.cpp Connection.h Connection_methods.h \
		CommSocket.cpp CommSocket.h \
		CommListener.cpp CommListener.h \
		CommClient.cpp CommClient.h \
		CommMetaClient.cpp CommMetaClient.h \
		CommServer.cpp CommServer.h \
		protocol_instructions.h \
		slave.cpp slave.h

cyslave_LDADD = ../rulesets/libscriptpython.a ../rulesets/librulesetbase.a ../rulesets/librulesetmind.a ../rulesets/librulesetentity.a ../rulesets/librulesetbase.a ../modules/libmodules.a ../common/libcommon.a ../physics/libphysics.a

cyslave_LDFLAGS = -export-dynamic
