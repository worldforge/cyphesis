rulesetdir = $(sysconfdir)/cyphesis/deeds.d
confbackupdir = $(datadir)/doc/cyphesis-$(VERSION)/conf/deeds

dist-hook:
	mkdir -p $(rulesetdir)
	mkdir -p $(confbackupdir)
	(cd $(srcdir) && tar cf - `find . -name \*.xml` | ( cd $(current) && cd $(rulesetdir) && tar xf -))
	(cd $(srcdir) && tar cf - `find . -name \*.xml` | ( cd $(current) && cd $(confbackupdir) && tar xf -))

install-data-local:
	mkdir -p $(rulesetdir) 
	mkdir -p $(confbackupdir) 
	(cd $(srcdir) && tar cf - `find . -name \*.xml`) | (cd $(rulesetdir) && tar xpf -)
	(cd $(srcdir) && tar cf - `find . -name \*.xml`) | (cd $(confbackupdir) && tar xpf -)

uninstall-local:
	find $(rulesetdir) -name  \*.xml | xargs rm -f
	find $(confbackupdir) -name  \*.xml | xargs rm -f
	
check-local:
	@set -e; for rule in `find $(srcdir) -name \*.xml` ; do \
		echo $(XMLLINT) -noout $$rule; \
		$(XMLLINT) -noout $$rule; \
	done;	