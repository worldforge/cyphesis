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

cyclient_LDFLAGS = -export-dynamic

cyclient_LDADD = ../rulesets/librulesetbase.a ../rulesets/librulesetentity.a ../rulesets/librulesetmind.a ../rulesets/libscriptpython.a ../rulesets/librulesetentity.a ../rulesets/librulesetbase.a ../modules/libmodules.a ../common/libcommon.a ../physics/libphysics.a

#atlas:
#	(cd ../../../protocols; tar cf - atlas/libatlas/libAtlasPy/*.py) | tar xf -
#
#dist-hook: atlas
#	tar cf - `find . -name \*.py` | (cd $(DESTDIR)$(distdir); tar xf -)
#	cp acorn acorn_graphic $(DESTDIR)$(distdir)/
#
#
#install-data-local: atlas
#	mkdirhier $(DESTDIR)$(datadir)/cyphesis/client
#	tar cf - `find . -name \*.py` | (cd $(DESTDIR)$(datadir)/cyphesis/client; tar xf -)
#	cat acorn | sed 's%PREFIX%$(datadir)\/cyphesis%' > $(DESTDIR)$(bindir)/acorn
#	cat acorn_graphic | sed 's%PREFIX%$(datadir)\/cyphesis%' > $(DESTDIR)$(bindir)/acorn_graphic
#	chmod 755 $(DESTDIR)$(bindir)/acorn $(DESTDIR)$(bindir)/acorn_graphic
