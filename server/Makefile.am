INCLUDES = -I$(top_srcdir)

bin_PROGRAMS = cyphesis
EXTRA_PROGRAMS = cyslave

cyphesis_SOURCES = \
		ExternalMind.cpp ExternalMind.h \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		Persistor.cpp Persistor_impl.h Persistor.h \
		Persistance.cpp Persistance.h \
		PersistantThingFactory.cpp PersistantThingFactory.h \
		PersistantThingFactory_impl.h \
		Restoration.cpp Restoration.h \
		Restorer.cpp Restorer_impl.h Restorer.h \
		ServerRouting.cpp ServerRouting.h ServerRouting_methods.h \
		WorldRouter.cpp WorldRouter.h \
		EntityFactory.cpp EntityFactory.h \
		Lobby.cpp Lobby.h \
		Connection.cpp Connection.h Connection_methods.h \
		TrustedConnection.cpp TrustedConnection.h \
		SlaveClientConnection.cpp SlaveClientConnection.h \
		Peer.cpp Peer.h \
		Idle.cpp Idle.h \
		CommSocket.cpp CommSocket.h \
		CommPSQLSocket.cpp CommPSQLSocket.h \
		CommListener.cpp CommListener.h \
		CommPeerListener.cpp CommPeerListener.h \
		CommUnixListener.cpp CommUnixListener.h \
		CommClient.cpp CommClient.h \
		CommLocalClient.cpp CommLocalClient.h \
		CommRemoteClient.cpp CommRemoteClient.h \
		CommPeer.cpp CommPeer.h \
		CommMetaClient.cpp CommMetaClient.h \
		CommMDNSPublisher.cpp CommMDNSPublisher.h \
		CommServer.cpp CommServer.h \
		protocol_instructions.h \
		server.cpp server.h

cyphesis_LDADD = $(top_builddir)/rulesets/libscriptpython.a \
                 $(top_builddir)/rulesets/librulesetmind.a \
                 $(top_builddir)/rulesets/librulesetentity.a \
                 $(top_builddir)/rulesets/librulesetbase.a \
                 $(top_builddir)/modules/libmodules.a \
                 $(top_builddir)/common/libcommon.a \
                 $(top_builddir)/physics/libphysics.a

cyphesis_LDFLAGS = -export-dynamic

cyslave_SOURCES = \
		ExternalMind.cpp ExternalMind.h \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		Persistor.cpp Persistor_impl.h Persistor.h \
		Persistance.cpp Persistance.h \
		PersistantThingFactory.cpp PersistantThingFactory.h \
		PersistantThingFactory_impl.h \
		Restoration.cpp Restoration.h \
		Restorer.cpp Restorer_impl.h Restorer.h \
		ServerRouting.cpp ServerRouting.h ServerRouting_methods.h \
		WorldRouter.cpp WorldRouter.h \
		EntityFactory.cpp EntityFactory.h \
		Lobby.cpp Lobby.h \
		Connection.cpp Connection.h Connection_methods.h \
		TrustedConnection.cpp TrustedConnection.h \
		SlaveClientConnection.cpp SlaveClientConnection.h \
		Peer.cpp Peer.h \
		Master.cpp Master.h \
		Idle.cpp Idle.h \
		CommSocket.cpp CommSocket.h \
		CommUnixListener.cpp CommUnixListener.h \
		CommSlaveListener.cpp CommSlaveListener.h \
		CommClient.cpp CommClient.h \
		CommPeer.cpp CommPeer.h \
		CommMaster.cpp CommMaster.h \
		CommLocalClient.cpp CommLocalClient.h \
		CommSlaveClient.cpp CommSlaveClient.h \
		CommServer.cpp CommServer.h \
		protocol_instructions.h \
		slave.cpp

cyslave_LDADD = $(top_builddir)/rulesets/libscriptpython.a \
                $(top_builddir)/rulesets/librulesetbase.a \
                $(top_builddir)/rulesets/librulesetmind.a \
                $(top_builddir)/rulesets/librulesetentity.a \
                $(top_builddir)/rulesets/librulesetbase.a \
                $(top_builddir)/modules/libmodules.a \
                $(top_builddir)/common/libcommon.a \
                $(top_builddir)/physics/libphysics.a

cyslave_LDFLAGS = -export-dynamic
