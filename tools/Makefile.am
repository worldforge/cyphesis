bin_PROGRAMS = cycmd cyconfig cyconvertrules cyloadrules cypasswd

INCLUDES = -I$(top_srcdir)

cycmd_SOURCES = cycmd.cpp

cycmd_LDADD = $(top_builddir)/modules/DateTime.o \
              $(top_builddir)/common/operations.o \
              $(top_builddir)/common/globals.o \
              $(top_builddir)/common/log.o \
              $(top_builddir)/common/utility.o \
              $(top_builddir)/common/const.o $(READLINE_LIBS)

cyconfig_SOURCES = cyconfig.cpp

cyconvertrules_SOURCES = cyconvertrules.cpp

cyloadrules_SOURCES = cyloadrules.cpp

cyloadrules_LDADD = $(top_builddir)/modules/DateTime.o \
                    $(top_builddir)/common/Database.o \
                    $(top_builddir)/common/globals.o \
                    $(top_builddir)/common/log.o

cypasswd_SOURCES = cypasswd.cpp

cypasswd_LDADD = $(top_builddir)/modules/DateTime.o \
                 $(top_builddir)/common/Database.o \
                 $(top_builddir)/common/accountbase.o \
                 $(top_builddir)/common/globals.o \
                 $(top_builddir)/common/log.o \
                 $(top_builddir)/common/system.o
