
SUBDIRS = physics common modules rulesets server client tests tools

docdir = $(datadir)/doc/cyphesis-$(VERSION)
dist_doc_DATA = README COPYING AUTHORS THANKS NEWS

confdir = $(sysconfdir)/cyphesis
dist_conf_DATA = cyphesis.vconf basic.xml acorn.xml mason.xml werewolf.xml

rulesetdir = $(datadir)/cyphesis/rulesets

socketdir = $(localstatedir)/tmp
socket_DATA =

EXTRA_DIST = cyphesis.spec.in cyphesis.spec \
             TODO.mason acinclude.m4 autogen.sh \
             cyphesis.init cyclient.init cyphesis.sysconfig
