
SUBDIRS = physics common modules rulesets server client

EXTRA_DIST = cyphesis.vconf basic.vconf

install-data-local:
	mkdirhier $(datadir)/cyphesis/rulesets
	cp -f *.vconf $(datadir)/cyphesis
