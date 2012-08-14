MARS_RULES = basic.xml characters.xml environment.xml terrain.xml

confdir = $(sysconfdir)/cyphesis

rulesetdir = $(sysconfdir)/cyphesis/mars.d
dist_ruleset_DATA = $(MARS_RULES)

confbackupdir = $(datadir)/doc/cyphesis-$(VERSION)/conf
confbackup_DATA = $(MARS_RULES)

check-local:
	@for rule in $(MARS_RULES) ; do \
	  echo $(XMLLINT) -noout $(srcdir)/$$rule; \
	  $(XMLLINT) -noout $(srcdir)/$$rule; \
	done;
