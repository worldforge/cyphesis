INCLUDES = -I$(top_srcdir) -I${top_builddir}

bin_PROGRAMS = cyphesis
EXTRA_PROGRAMS = cyslave

if LINK_STATIC

SERVER_LIBS = \
    /usr/lib/libmercator-0.2.a \
    /usr/lib/libskstream-0.3.a \
    /usr/lib/libvarconf-1.0.a \
    /usr/lib/libwfmath-0.3.a \
    /usr/lib/libAtlasObjects-0.6.a \
    /usr/lib/libAtlasNet-0.6.a \
    /usr/lib/libAtlasCodecs-0.6.a \
    /usr/lib/libAtlasFilters-0.6.a \
    /usr/lib/libAtlasMessage-0.6.a \
    /usr/lib/libAtlas-0.6.a \
    /usr/lib/libsigc-2.0.a \
    /usr/lib/libpq.a \
    /usr/lib/libssl.a \
    /usr/lib/libkrb5.a \
    /usr/lib/libgssapi_krb5.a \
    /usr/lib/libk5crypto.a \
    /usr/lib/libgcrypt.a \
    /usr/lib/libgpg-error.a \
    /usr/lib/libcrypto.a \
    /usr/lib/libcrypt.a \
    /usr/lib/libresolv.a \
    /usr/lib/libcom_err.a \
    $(PYTHON_LIBS) \
    /usr/lib/libutil.a \
    /usr/lib/libhowl.a \
    $(STATIC_LIBSTDCPP) \
    $(STATIC_LIBGCC) \
    -ldl -lc -lm -lpthread -lgcc_s

cyphesis_LDFLAGS = -nodefaultlibs $(PYTHON_LINKER_FLAGS)

cyslave_LDFLAGS = -nodefaultlibs $(PYTHON_LINKER_FLAGS)

else

SERVER_LIBS = $(COMMON_LIBS) $(TERRAIN_LIBS) $(NETWORK_LIBS) $(MDNS_LIBS) $(PGSQL_LIBS) $(PYTHON_LIBS) $(PYTHON_UTIL_LIBS)

cyphesis_LDFLAGS = $(PYTHON_LINKER_FLAGS)

cyslave_LDFLAGS = $(PYTHON_LINKER_FLAGS)

endif

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
		ScriptFactory.cpp ScriptFactory.h \
		TaskFactory.cpp TaskFactory.h \
		ArithmeticFactory.cpp ArithmeticFactory.h \
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
                 $(top_builddir)/physics/libphysics.a \
                 $(SERVER_LIBS)

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
		ScriptFactory.cpp ScriptFactory.h \
		TaskFactory.cpp TaskFactory.h \
		ArithmeticFactory.cpp ArithmeticFactory.h \
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
                $(top_builddir)/rulesets/librulesetmind.a \
                $(top_builddir)/rulesets/librulesetentity.a \
                $(top_builddir)/rulesets/librulesetbase.a \
                $(top_builddir)/modules/libmodules.a \
                $(top_builddir)/common/libcommon.a \
                $(top_builddir)/physics/libphysics.a \
                $(SERVER_LIBS)
