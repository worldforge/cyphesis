bin_PROGRAMS = cycmd cypasswd cyloadrules cywatchdog cyctrl cyconfig

bin_SCRIPTS = cyphesis-setup

INCLUDES = -I$(top_srcdir) -DBINDIR=\"@bindir@\" -DVARDIR=\"@localstatedir@\"

cypasswd_SOURCES = cypasswd.cpp sstream.h iotraits.h

cypasswd_LDADD = $(top_builddir)/common/Database.o \
                 $(top_builddir)/common/accountbase.o \
                 $(top_builddir)/common/globals.o \
                 $(top_builddir)/common/log.o \
                 $(top_builddir)/common/system.o

cycmd_SOURCES = cycmd.cpp

cycmd_LDADD = $(top_builddir)/common/operations.o \
              $(top_builddir)/common/globals.o \
              $(top_builddir)/common/utility.o \
              $(top_builddir)/common/const.o $(READLINE_LIBS)

# cydbload_SOURCES = cydbload.cpp

# cydbload_LDADD = $(top_builddir)/common/Database.o \
                   $(top_builddir)/common/globals.o \
                   $(top_builddir)/common/log.o

cyloadrules_SOURCES = cyloadrules.cpp

cyloadrules_LDADD = $(top_builddir)/common/Database.o \
                    $(top_builddir)/common/globals.o \
                    $(top_builddir)/common/log.o

cywatchdog_SOURCES = cywatchdog.cpp

cyctrl_SOURCES = cyctrl.cpp

cyconfig_SOURCES = cyconfig.cpp

EXTRA_DIST = sstream.cc cyphesis-setup
