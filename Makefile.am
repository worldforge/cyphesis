
SUBDIRS = physics common modules rulesets server client tools

bin_SCRIPTS = cyphesis-config

docdir = $(datadir)/doc/cyphesis-$(VERSION)
doc_DATA = README COPYING AUTHORS THANKS NEWS

confdir = $(sysconfdir)/cyphesis
conf_DATA = cyphesis.vconf basic.xml acorn.xml mason.xml werewolf.xml moraf.xml

rulesetdir = $(datadir)/cyphesis/rulesets

EXTRA_DIST = cyphesis.vconf basic.xml acorn.xml mason.xml werewolf.xml \
             cyphesis-config.in cyphesis.spec.in TODO.mason acinclude.m4 autogen.sh \
             agrilan_a1.xml agrilan_a2.xml agrilan_a3.xml agrilan_a4.xml agrilan.xml moraf.xml
