ACLOCAL_AMFLAGS = -I m4

SUBDIRS = libraries physics common modules rulesets server client aiclient data \
          tools tests man dox

docdir = $(datadir)/doc/cyphesis-$(VERSION)
dist_doc_DATA = README COPYING AUTHORS THANKS NEWS FIXME

rulesetdir = $(datadir)/cyphesis/rulesets

socketdir = $(localstatedir)/tmp
socket_DATA =

# FIXME Remove the mkdir hack if it gets fixed in a future automake
install-data-hook:
	mkdir -p $(DESTDIR)$(socketdir)
	chmod 1777 $(DESTDIR)$(socketdir)
	@echo 
	@echo "Cyphesis is not yet ready to run."
	@echo "Unless you have configured the server to run without a database, Cyphesis"
	@echo "requires access to a PostgreSQL database to store rules and account data."
	@echo "Please run 'sudo $(top_srcdir)/scripts/cyphesis-setup.sh' to create the"
	@echo "PostgreSQL account and database required, or run it as root."
	@echo 
	@echo "For detailed instructions please see the README file."
	@echo 

EXTRA_DIST = cyphesis.spec Doxyfile autogen.sh \
             cyphesis.init cyclient.init cyphesis.sysconfig \
             scripts/compile_python.py scripts/gen_op.py \
             scripts/gen_buildid.py scripts/cyphesis-setup.sh \
             scripts/extract_revision.sh

docs:
	@echo "running doxygen..."
	@doxygen Doxyfile
	@doxygen Doxyfile-python
	@echo "documentation is in doc/."
	@echo "python documentation is in python_doc/."
