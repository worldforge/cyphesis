MINIMAL_RULES = basic.xml characters.xml environment.xml terrain.xml

confdir = $(sysconfdir)/cyphesis

rulesetdir = $(sysconfdir)/cyphesis/minimal.d
dist_ruleset_DATA = $(MINIMAL_RULES)

confbackupdir = $(datadir)/doc/cyphesis-$(VERSION)/conf/minimal
confbackup_DATA = $(MINIMAL_RULES)

check-local:
	@for rule in $(MINIMAL_RULES) ; do \
	  echo $(XMLLINT) -noout $(srcdir)/$$rule; \
	  $(XMLLINT) -noout $(srcdir)/$$rule; \
	done;
