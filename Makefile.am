
SUBDIRS = physics common modules rulesets server client data tools tests

docdir = $(datadir)/doc/cyphesis-$(VERSION)
dist_doc_DATA = README COPYING AUTHORS THANKS NEWS

rulesetdir = $(datadir)/cyphesis/rulesets

socketdir = $(localstatedir)/tmp
socket_DATA =

EXTRA_DIST = cyphesis.spec.in cyphesis.spec Doxyfile \
             TODO.mason acinclude.m4 autogen.sh \
             cyphesis.init cyclient.init cyphesis.sysconfig

docs:
	@echo "running doxygen..."
	@doxygen Doxyfile
	@echo "documentation is in doc/."
