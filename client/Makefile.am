atlas:
	(cd ../../../protocols; tar cf - atlas/libatlas/libAtlasPy/*.py) | tar xf -

dist-hook: atlas
	tar cf - `find . -name \*.py` | (cd $(distdir); tar xf -)
	cp acorn acorn_graphic $(distdir)/


install-data-local: atlas
	mkdirhier $(datadir)/cyphesis/client
	tar cf - `find . -name \*.py` | (cd $(datadir)/cyphesis/client; tar xf -)
	cat acorn | sed 's%PREFIX%$(datadir)\/cyphesis%' > $(bindir)/acorn
	cat acorn_graphic | sed 's%PREFIX%$(datadir)\/cyphesis%' > $(bindir)/acorn_graphic
	chmod 755 $(bindir)/acorn $(bindir)/acorn_graphic
