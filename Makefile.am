
SUBDIRS = physics common modules rulesets server client data tools tests man

docdir = $(datadir)/doc/cyphesis-$(VERSION)
dist_doc_DATA = README COPYING AUTHORS THANKS NEWS FIXME

rulesetdir = $(datadir)/cyphesis/rulesets

socketdir = $(localstatedir)/tmp
socket_DATA =

install-data-hook:
	chmod 1777 $(DESTDIR)$(socketdir)

EXTRA_DIST = cyphesis.spec Doxyfile autogen.sh \
             cyphesis.init cyclient.init cyphesis.sysconfig

docs:
	@echo "running doxygen..."
	@doxygen Doxyfile
	@echo "documentation is in doc/."
