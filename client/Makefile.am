atlas:
	(cd ../../../protocols; tar cf - atlas/libatlas/libAtlasPy/*.py) | tar xf -

dist-hook: atlas
	tar cf - `find . -name \*.py` | (cd $(DESTDIR)$(distdir); tar xf -)
	cp acorn acorn_graphic $(DESTDIR)$(distdir)/


install-data-local: atlas
	mkdirhier $(DESTDIR)$(datadir)/cyphesis/client
	tar cf - `find . -name \*.py` | (cd $(DESTDIR)$(datadir)/cyphesis/client; tar xf -)
	cat acorn | sed 's%PREFIX%$(datadir)\/cyphesis%' > $(DESTDIR)$(bindir)/acorn
	cat acorn_graphic | sed 's%PREFIX%$(datadir)\/cyphesis%' > $(DESTDIR)$(bindir)/acorn_graphic
	chmod 755 $(DESTDIR)$(bindir)/acorn $(DESTDIR)$(bindir)/acorn_graphic
