EXTRA_DIST = acorn.xml werewolf.xml

MASON_RULES = animals.xml basic.xml buildings.xml clothing.xml mason.xml \
              plants.xml

confdir = $(sysconfdir)/cyphesis
dist_conf_DATA = cyphesis.vconf

rulesetdir = $(sysconfdir)/cyphesis/mason.d
dist_ruleset_DATA = $(MASON_RULES)

confbackupdir = $(datadir)/doc/cyphesis-$(VERSION)/conf
confbackup_DATA = cyphesis.vconf $(MASON_RULES) acorn.xml werewolf.xml
