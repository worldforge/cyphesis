bin_PROGRAMS = cyclient

cyclient_SOURCES = \
		CharacterClient.cpp CharacterClient.h \
		CreatorClient.cpp CreatorClient.h \
		CommClient.cpp CommClient.h \
		ObserverClient.cpp ObserverClient.h \
		ClientConnection.cpp ClientConnection.h \
		define_world.cpp define_world.h \
		Py_CreatorClient.cpp Py_CreatorClient.h \
		client.cpp

if BUILD_STATIC

cyclient_LDADD = ../rulesets/librulesetbase.a ../rulesets/librulesetentity.a ../rulesets/librulesetmind.a ../rulesets/libscriptpython.a ../modules/libmodules.a ../common/libcommon.a ../physics/libphysics.a /usr/local/lib/libAtlasObjects.a /usr/local/lib/libAtlasObjectsOperation.a /usr/local/lib/libAtlasObjectsEntity.a /usr/local/lib/libAtlasNet.a /usr/local/lib/libAtlas.a /usr/local/lib/libAtlasCodecs.a /usr/local/lib/libAtlasMessage.a /usr/local/lib/libAtlas.a /usr/lib/libdb_cxx-3.2.a /usr/lib/libdb-3.2.a /usr/local/lib/libvarconf.a /usr/lib/libsigc.a /opt/forge/lib/python2.1/config/libpython2.1.a /usr/lib/libutil.a /usr/lib/libdl.a /usr/lib/libpthread.a /usr/lib/libstdc++-3-libc6.2-2-2.10.0.a -lc -lm /usr/lib/gcc-lib/i386-redhat-linux/2.96/libgcc.a /usr/local/lib/libskstream.a

# LIBS = # -ldl -lm -lpthread

cyclient_LDFLAGS = -Wl,-export-dynamic -nodefaultlibs

else

cyclient_LDFLAGS = -export-dynamic

cyclient_LDADD = ../rulesets/librulesetbase.a ../rulesets/librulesetentity.a ../rulesets/librulesetmind.a ../rulesets/libscriptpython.a ../rulesets/librulesetentity.a ../rulesets/librulesetbase.a ../modules/libmodules.a ../common/libcommon.a ../physics/libphysics.a

endif
