
SUBDIRS = physics common modules rulesets server client tests tools

bin_SCRIPTS = cyphesis-config

docdir = $(datadir)/doc/cyphesis-$(VERSION)
doc_DATA = README COPYING AUTHORS THANKS NEWS

confdir = $(sysconfdir)/cyphesis
conf_DATA = cyphesis.vconf basic.xml acorn.xml mason.xml werewolf.xml moraf.xml \
            agrilan_a1.xml agrilan_a2.xml agrilan_a3.xml agrilan_a4.xml agrilan.xml

rulesetdir = $(datadir)/cyphesis/rulesets

socketdir = $(localstatedir)
socket_DATA =

EXTRA_DIST = cyphesis.vconf basic.xml acorn.xml mason.xml werewolf.xml \
             cyphesis-config.in cyphesis.spec.in TODO.mason acinclude.m4 \
             autogen.sh cyphesis.init cyphesis.sysconfig \
             agrilan_a1.xml agrilan_a2.xml agrilan_a3.xml agrilan_a4.xml \
             agrilan.xml moraf.xml
