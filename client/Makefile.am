atlas:
	(cd ../../../protocols; tar cf - atlas/libatlas/libAtlasPy/*.py) | tar xf -

dist-hook: atlas
	tar cf - `find . -name \*.py` | (cd $(distdir); tar xf -)


install-data-local: atlas
	mkdirhier $(datadir)/cyphesis/client
	tar cf - `find . -name \*.py` | (cd $(datadir)/cyphesis/client; tar xf -)
	cat acorn | sed 's%PREFIX%$(datadir)\/cyphesis%' > $(bindir)/acorn
	cat acorn_text | sed 's%PREFIX%$(datadir)\/cyphesis%' > $(bindir)/acorn_text
