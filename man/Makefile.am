man_MANS = cyclient.1 cycmd.1 cyphesis.1 cyphesis-tools.1 \
           cyaddrules.1 cyconvertrules.1 cydumprules.1 cyloadrules.1 cypasswd.1

.sgml.1:
	docbook2man $(top_srcdir)/man/manpages.sgml
	echo ".so man1/cyphesis-tools.1" > cyaddrules.1
	echo ".so man1/cyphesis-tools.1" > cyconvertrules.1
	echo ".so man1/cyphesis-tools.1" > cydumprules.1
	echo ".so man1/cyphesis-tools.1" > cyloadrules.1
	echo ".so man1/cyphesis-tools.1" > cypasswd.1

EXTRA_DIST = cyclient.sgml cycmd.sgml cyphesis.sgml cyphesis-tools.sgml \
             manpages.sgml cyclient.1 cycmd.1 cyphesis.1 cyphesis-tools.1 \
             cyaddrules.1 cyconvertrules.1 cydumprules.1 cyloadrules.1 \
             cypasswd.1

maintainer-clean-local:
	-rm -f $(man_MANS)
