
SUBDIRS = physics common modules rulesets server client tools

EXTRA_DIST = cyphesis.vconf basic.vconf acorn.vconf mason.vconf werewolf.vconf \
    cyphesis.spec.in agrilan_a1.xml agrilan_a2.xml agrilan_a3.xml agrilan.xml \
    TODO.acorn

install-data-local:
	mkdirhier $(DESTDIR)$(datadir)/cyphesis/rulesets
	cp -f *.vconf $(DESTDIR)$(datadir)/cyphesis
	mkdirhier $(DESTDIR)$(localstatedir)/cyphesis
	mkdirhier $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) README $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) COPYING $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) AUTHORS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) THANKS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) NEWS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
