RULES = acorn.xml

rulesetdir = $(sysconfdir)/cyphesis/acorn.d
dist_ruleset_DATA = $(RULES)

confbackupdir = $(datadir)/doc/cyphesis-$(VERSION)/conf/acorn
confbackup_DATA = $(RULES)

check-local:
	@for rule in $(MINIMAL_RULES) ; do \
	  echo $(XMLLINT) -noout $(srcdir)/$$rule; \
	  $(XMLLINT) -noout $(srcdir)/$$rule; \
	done;
