
SUBDIRS = physics common modules rulesets server client watchdog tools

EXTRA_DIST = cyphesis.vconf basic.vconf acorn.vconf cyphesis.spec

install-data-local:
	mkdirhier $(datadir)/cyphesis/rulesets
	cp -f *.vconf $(datadir)/cyphesis
	mkdirhier $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) README $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) COPYING $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) AUTHORS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) THANKS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) NEWS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
