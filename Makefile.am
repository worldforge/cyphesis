
SUBDIRS = physics modules common rulesets server client tools

EXTRA_DIST = cyphesis.vconf basic.xml acorn.xml mason.xml werewolf.xml \
             cyphesis.spec.in TODO.mason acinclude.m4 autogen.sh

install-data-local:
	mkdir -p $(DESTDIR)$(datadir)/cyphesis/rulesets
	mkdir -p $(DESTDIR)$(sysconfdir)/cyphesis
	cp -f cyphesis.vconf $(DESTDIR)$(sysconfdir)/cyphesis
	cp -f basic.xml acorn.xml mason.xml werewolf.xml $(DESTDIR)$(sysconfdir)/cyphesis
	cp -f agrilan*.xml $(DESTDIR)$(sysconfdir)/cyphesis
	mkdir -p $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) README $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) COPYING $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) AUTHORS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) THANKS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) NEWS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
