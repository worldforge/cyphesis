
SUBDIRS = physics modules common rulesets server client tools

EXTRA_DIST = cyphesis.vconf basic.xml acorn.xml mason.xml werewolf.xml \
    cyphesis.spec.in agrilan_a1.xml agrilan_a2.xml agrilan_a3.xml agrilan.xml \
    TODO.acorn acinclude.m4 autogen.sh

install-data-local:
	mkdirhier $(DESTDIR)$(datadir)/cyphesis/rulesets
	cp -f cyphesis.vconf $(DESTDIR)$(datadir)/cyphesis
	cp -f *.xml $(DESTDIR)$(datadir)/cyphesis
	mkdirhier $(DESTDIR)$(localstatedir)/cyphesis
	mkdirhier $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) README $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) COPYING $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) AUTHORS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) THANKS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
	$(INSTALL_DATA) NEWS $(DESTDIR)$(datadir)/doc/cyphesis-$(VERSION)
