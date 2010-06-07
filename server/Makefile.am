INCLUDES = -I$(top_srcdir) -I${top_builddir}

bin_PROGRAMS = cyphesis
EXTRA_PROGRAMS = cyslave

LOCAL_LIBS = $(top_builddir)/rulesets/libscriptpython.a \
             $(top_builddir)/rulesets/librulesetmind.a \
             $(top_builddir)/rulesets/librulesetentity.a \
             $(top_builddir)/rulesets/librulesetbase.a \
             $(top_builddir)/modules/libmodules.a \
             $(top_builddir)/common/libcommon.a \
             $(top_builddir)/physics/libphysics.a

DISTCLEANFILES = buildid.cpp

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
    /usr/lib/libldap.a \
    /usr/lib/liblber.a \
    /usr/lib/libsasl2.a \
    /usr/lib/libdb-4.8.a \
    /usr/lib/libssl.a \
    /usr/lib/libgssapi_krb5.a \
    /usr/lib/libkrb5.a \
    /usr/lib/libk5crypto.a \
    /usr/lib/libkrb5support.a \
    /usr/lib/libkeyutils.a \
    /usr/lib/libgcrypt.a \
    /usr/lib/libgpg-error.a \
    /usr/lib/libcrypto.a \
    /usr/lib/libcrypt.a \
    /usr/lib/libselinux.a \
    /usr/lib/libsepol.a \
    /usr/lib/libresolv.a \
    /usr/lib/libcom_err.a \
    $(PYTHON_LIBS) \
    /usr/lib/libutil.a \
    /usr/lib/libz.a \
    $(STATIC_LIBSTDCPP) \
    $(STATIC_LIBGCC) \
    -ldl -lc -lm -lpthread -lgcc_s

#    /usr/lib/libavahi-client.a \
#    /usr/lib/libavahi-common.a \
#    /usr/lib/libdbus-1.a 

cyphesis_LDFLAGS = -nodefaultlibs $(PYTHON_LINKER_FLAGS)

cyslave_LDFLAGS = -nodefaultlibs $(PYTHON_LINKER_FLAGS)

else

SERVER_LIBS = $(COMMON_LIBS) $(TERRAIN_LIBS) $(NETWORK_LIBS) $(MDNS_LIBS) $(PGSQL_LIBS) $(PYTHON_LIBS) $(PYTHON_UTIL_LIBS)

cyphesis_LDFLAGS = $(PYTHON_LINKER_FLAGS)

cyslave_LDFLAGS = $(PYTHON_LINKER_FLAGS)

endif

cyphesis_SOURCES = \
		ExternalMind.cpp ExternalMind.h \
		ExternalProperty.cpp ExternalProperty.h \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		SystemAccount.cpp SystemAccount.h \
		Server.cpp Server.h \
		Persistence.cpp Persistence.h \
		EntityFactory.cpp EntityFactory.h \
		EntityFactory_impl.h \
		ServerRouting.cpp ServerRouting.h \
		Spawn.cpp Spawn.h \
		SpawnEntity.cpp SpawnEntity.h \
		WorldRouter.cpp WorldRouter.h \
		StorageManager.cpp StorageManager.h \
		ScriptFactory.cpp ScriptFactory.h \
		TaskScriptFactory.cpp TaskScriptFactory.h \
		TaskFactory.cpp TaskFactory.h \
		CorePropertyManager.cpp CorePropertyManager.h \
		Ruleset.cpp Ruleset.h \
		EntityBuilder.cpp EntityBuilder.h \
		ArithmeticBuilder.cpp ArithmeticBuilder.h \
		Lobby.cpp Lobby.h \
		Connection.cpp Connection.h Connection_methods.h \
		TrustedConnection.cpp TrustedConnection.h \
		SlaveClientConnection.cpp SlaveClientConnection.h \
		Peer.cpp Peer.h \
		Idle.cpp Idle.h \
		IdleConnector.cpp IdleConnector.h \
		UpdateTester.cpp UpdateTester.h \
		CommSocket.cpp CommSocket.h \
		CommPSQLSocket.cpp CommPSQLSocket.h \
		IdlePSQLConnector.cpp IdlePSQLConnector.h \
		HttpCache.cpp HttpCache.h \
		CommStreamListener.cpp CommStreamListener.h \
		CommTCPListener.cpp CommTCPListener.h \
		CommUnixListener.cpp CommUnixListener.h \
		CommStreamClient.cpp CommStreamClient.h \
		CommClient.cpp CommClient.h \
		CommPeer.cpp CommPeer.h \
		CommHttpClient.cpp CommHttpClient.h \
		CommPythonClient.cpp CommPythonClient.h \
		CommMetaClient.cpp CommMetaClient.h \
		CommMDNSPublisher.cpp CommMDNSPublisher.h \
		CommServer.cpp CommServer.h \
		CommClientFactory.cpp CommClientFactory.h \
		server.cpp server.h \
		buildid.cpp

cyphesis_LDADD = $(LOCAL_LIBS) $(SERVER_LIBS)

buildid.cpp: $(cyphesis_SOURCES) $(LOCAL_LIBS) \
             $(top_srcdir)/scripts/gen_buildid.py $(top_srcdir)/ChangeLog
	$(top_srcdir)/scripts/gen_buildid.py `$(top_srcdir)/scripts/extract_revision.sh $(top_srcdir)` > buildid.cpp

cyslave_SOURCES = \
		ExternalMind.cpp ExternalMind.h \
		ExternalProperty.cpp ExternalProperty.h \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		Persistence.cpp Persistence.h \
		EntityFactory.cpp EntityFactory.h \
		EntityFactory_impl.h \
		ServerRouting.cpp ServerRouting.h \
		WorldRouter.cpp WorldRouter.h \
		ScriptFactory.cpp ScriptFactory.h \
		TaskFactory.cpp TaskFactory.h \
		CorePropertyManager.cpp CorePropertyManager.h \
		EntityBuilder.cpp EntityBuilder.h \
		Lobby.cpp Lobby.h \
		Connection.cpp Connection.h Connection_methods.h \
		TrustedConnection.cpp TrustedConnection.h \
		SlaveClientConnection.cpp SlaveClientConnection.h \
		Peer.cpp Peer.h \
		Master.cpp Master.h \
		Idle.cpp Idle.h \
		HttpCache.cpp HttpCache.h \
		CommSocket.cpp CommSocket.h \
		CommStreamListener.cpp CommStreamListener.h \
		CommUnixListener.cpp CommUnixListener.h \
		CommStreamClient.cpp CommStreamClient.h \
		CommClient.cpp CommClient.h \
		CommPeer.cpp CommPeer.h \
		CommHttpClient.cpp CommHttpClient.h \
		CommMaster.cpp CommMaster.h \
		CommServer.cpp CommServer.h \
		CommClientFactory.cpp CommClientFactory.h \
		slave.cpp

nodist_cyslave_SOURCES = \
		buildid.cpp

cyslave_LDADD = $(LOCAL_LIBS) $(SERVER_LIBS)
