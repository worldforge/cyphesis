bin_PROGRAMS = cyclient

cyclient_SOURCES = \
		CharacterClient.cpp CharacterClient.h \
		CreatorClient.cpp CreatorClient.h \
		CommClient.cpp CommClient.h \
		ObserverClient.cpp ObserverClient.h \
		ClientConnection.cpp ClientConnection.h \
		Py_CreatorClient.cpp Py_CreatorClient.h \
		client.cpp

cyclient_LDFLAGS = -export-dynamic

cyclient_LDADD = ../rulesets/librulesetbase.a ../rulesets/librulesetentity.a ../rulesets/librulesetmind.a ../rulesets/libscriptpython.a ../rulesets/librulesetentity.a ../rulesets/librulesetbase.a ../modules/libmodules.a ../common/libcommon.a ../physics/libphysics.a
