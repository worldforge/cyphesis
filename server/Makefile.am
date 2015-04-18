AM_CPPFLAGS = -I$(top_srcdir) -I${top_builddir} -DPREFIX=\"@prefix@\"

noinst_LIBRARIES = libcomm.a
bin_PROGRAMS = cyphesis
EXTRA_PROGRAMS = cyslave cyprox

LOCAL_LIBS = libcomm.a \
             $(top_builddir)/rulesets/libscriptpython.a \
             $(top_builddir)/rulesets/librulesetmind.a \
             $(top_builddir)/rulesets/librulesetentity.a \
             $(top_builddir)/rulesets/libentityfilter.a \
             $(top_builddir)/rulesets/librulesetbase.a \
             $(top_builddir)/modules/libmodules.a \
             $(top_builddir)/common/libcommon.a \
             $(top_builddir)/physics/libphysics.a

DISTCLEANFILES = buildid.cpp

if LINK_STATIC

SERVER_LIBS = \
    /usr/lib/libmercator-0.2.a \
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

SERVER_LIBS = $(COMMON_LIBS) $(TERRAIN_LIBS) $(NETWORK_LIBS) $(MDNS_LIBS) \
              $(PGSQL_LIBS) $(PYTHON_LIBS) $(PYTHON_UTIL_LIBS)

FRONTEND_LIBS = $(COMMON_LIBS) $(MATH_LIBS) $(NETWORK_LIBS) $(MDNS_LIBS) \
                $(PGSQL_LIBS)

cyphesis_LDFLAGS = $(PYTHON_LINKER_FLAGS)

cyslave_LDFLAGS = $(PYTHON_LINKER_FLAGS)

endif

SERVER_SOURCES = \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		SystemAccount.cpp SystemAccount.h \
		ServerAccount.cpp ServerAccount.h \
		Persistence.cpp Persistence.h \
		EntityFactory.cpp EntityFactory.h \
		EntityFactory_impl.h \
		ServerRouting.cpp ServerRouting.h \
		Spawn.h \
		SpawnEntity.cpp SpawnEntity.h \
		WorldRouter.cpp WorldRouter.h \
		StorageManager.cpp StorageManager.h \
		TaskFactory.cpp TaskFactory.h \
		CorePropertyManager.cpp CorePropertyManager.h \
		Ruleset.cpp Ruleset.h \
		RuleHandler.cpp RuleHandler.h \
		EntityRuleHandler.cpp EntityRuleHandler.h \
		ArchetypeRuleHandler.cpp ArchetypeRuleHandler.h \
		ArchetypeFactory.cpp ArchetypeFactory.h \
		OpRuleHandler.cpp OpRuleHandler.h \
		PropertyRuleHandler.cpp PropertyRuleHandler.h \
		TaskRuleHandler.cpp TaskRuleHandler.h \
		EntityBuilder.cpp EntityBuilder.h \
		ArithmeticBuilder.cpp ArithmeticBuilder.h \
		Lobby.cpp Lobby.h \
		ConnectableRouter.cpp ConnectableRouter.h \
		Connection.cpp Connection.h Connection_methods.h \
		TrustedConnection.cpp TrustedConnection.h \
		Peer.cpp Peer.h \
		Juncture.cpp Juncture.h \
		CommPSQLSocket.cpp CommPSQLSocket.h \
		TeleportState.cpp TeleportState.h \
		TeleportAuthenticator.cpp TeleportAuthenticator.h \
		TeleportProperty.cpp TeleportProperty.h \
		PendingTeleport.cpp PendingTeleport.h \
		CommPeer.cpp CommPeer.h \
		CommPythonClient.cpp CommPythonClient.h \
		server.cpp server.h \
		MindInspector.cpp MindInspector.h

COMM_SOURCES = \
		CommHttpClient.cpp CommHttpClient.h \
		CommMetaClient.cpp CommMetaClient.h \
		CommMDNSPublisher.cpp CommMDNSPublisher.h \
		HttpCache.cpp HttpCache.h \
		CommAsioListener.cpp CommAsioListener.h CommAsioListener_impl.h \
		CommAsioClient.cpp CommAsioClient.h CommAsioClient_impl.h \
		$(top_srcdir)/metaserverapi/MetaServerPacket.cpp \
		$(top_srcdir)/metaserverapi/MetaServerPacket.hpp \
		$(top_srcdir)/metaserverapi/MetaServerAPI.hpp \
		$(top_srcdir)/metaserverapi/MetaServerVersion.hpp \
		$(top_srcdir)/metaserverapi/MetaServerProtocol.hpp \
		IdleConnector.cpp IdleConnector.h

libcomm_a_SOURCES = $(COMM_SOURCES)

cyphesis_SOURCES = $(SERVER_SOURCES) buildid.cpp

cyphesis_LDADD = $(LOCAL_LIBS) $(SERVER_LIBS)

cyprox_SOURCES = frontend.cpp buildid.cpp

cyprox_LDADD = libcomm.a \
               $(top_builddir)/modules/libmodules.a \
               $(top_builddir)/common/libcommon.a \
               $(top_builddir)/physics/libphysics.a \
               $(FRONTEND_LIBS)

buildid.cpp: $(cyphesis_SOURCES) $(LOCAL_LIBS) \
             $(top_srcdir)/scripts/gen_buildid.py $(top_srcdir)/ChangeLog
	$(top_srcdir)/scripts/gen_buildid.py `$(top_srcdir)/scripts/extract_revision.sh $(top_srcdir)` buildid.cpp

cyslave_SOURCES = \
		Account.cpp Account.h \
		Player.cpp Player.h \
		Admin.cpp Admin.h \
		Persistence.cpp Persistence.h \
		EntityFactory.cpp EntityFactory.h \
		EntityFactory_impl.h \
		ServerRouting.cpp ServerRouting.h \
		WorldRouter.cpp WorldRouter.h \
		TaskFactory.cpp TaskFactory.h \
		CorePropertyManager.cpp CorePropertyManager.h \
		EntityBuilder.cpp EntityBuilder.h \
		Lobby.cpp Lobby.h \
		Connection.cpp Connection.h Connection_methods.h \
		TrustedConnection.cpp TrustedConnection.h \
		SlaveClientConnection.cpp SlaveClientConnection.h \
		Peer.cpp Peer.h \
		Master.cpp Master.h \
		HttpCache.cpp HttpCache.h \
		CommPeer.cpp CommPeer.h \
		CommHttpClient.cpp CommHttpClient.h \
		CommMaster.cpp CommMaster.h \
		TeleportState.cpp TeleportState.h \
		slave.cpp

nodist_cyslave_SOURCES = \
		buildid.cpp

cyslave_LDADD = $(LOCAL_LIBS) $(SERVER_LIBS)
