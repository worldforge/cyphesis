INCLUDES = -I$(top_srcdir)

bin_PROGRAMS = cyclient

cyclient_SOURCES = \
		CharacterClient.cpp CharacterClient.h \
		CreatorClient.cpp CreatorClient.h \
		BaseClient.cpp BaseClient.h \
		ObserverClient.cpp ObserverClient.h \
		ClientConnection.cpp ClientConnection.h \
		Py_CreatorClient.cpp Py_CreatorClient.h \
		client.cpp

cyclient_LDFLAGS = -export-dynamic

cyclient_LDADD = $(top_builddir)/rulesets/librulesetbase.a \
                 $(top_builddir)/rulesets/librulesetentity.a \
                 $(top_builddir)/rulesets/librulesetmind.a \
                 $(top_builddir)/rulesets/libscriptpython.a \
                 $(top_builddir)/rulesets/librulesetentity.a \
                 $(top_builddir)/rulesets/librulesetbase.a \
                 $(top_builddir)/modules/libmodules.a \
                 $(top_builddir)/common/libcommon.a \
                 $(top_builddir)/physics/libphysics.a
