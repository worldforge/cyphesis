
SUBDIRS = physics common modules rulesets server client

EXTRA_DIST = cyphesis.vconf basic.vconf acorn.vconf cyphesis.spec

install-data-local:
	mkdirhier $(datadir)/cyphesis/rulesets
	cp -f *.vconf $(datadir)/cyphesis
