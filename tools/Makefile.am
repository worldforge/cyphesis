INCLUDES = -I$(top_srcdir)

bin_PROGRAMS = cycmd cyconfig cyconvertrules cyloadrules cypasswd
noinst_SCRIPTS = cyphesis.sh

EXTRA_DIST = cyphesis.sh

if LINK_STATIC

TOOL_LIBS = \
    /usr/lib/libskstream-0.3.a \
    /usr/lib/libvarconf-1.0.a \
    /usr/lib/libAtlasObjectsOperation-0.5.a \
    /usr/lib/libAtlasObjectsEntity-0.5.a \
    /usr/lib/libAtlasObjects-0.5.a \
    /usr/lib/libAtlasNet-0.5.a \
    /usr/lib/libAtlasCodecs-0.5.a \
    /usr/lib/libAtlasMessage-0.5.a \
    /usr/lib/libAtlas-0.5.a \
    /usr/lib/libsigc-2.0.a \
    $(STATIC_LIBSTDCPP) \
    $(STATIC_LIBGCC) \
    -lc -lm -lgcc_s

DBTOOL_LIBS = \
    /usr/lib/libvarconf-1.0.a \
    /usr/lib/libAtlasObjectsOperation-0.5.a \
    /usr/lib/libAtlasObjectsEntity-0.5.a \
    /usr/lib/libAtlasObjects-0.5.a \
    /usr/lib/libAtlasNet-0.5.a \
    /usr/lib/libAtlasCodecs-0.5.a \
    /usr/lib/libAtlasMessage-0.5.a \
    /usr/lib/libAtlas-0.5.a \
    /usr/lib/libsigc-2.0.a \
    /usr/lib/libpq.a \
    /usr/lib/libssl.a \
    /usr/lib/libkrb5.a \
    /usr/lib/libgssapi_krb5.a \
    /usr/lib/libk5crypto.a \
    /usr/lib/libcrypto.a \
    /usr/lib/libcrypt.a \
    /usr/lib/libresolv.a \
    /usr/lib/libcom_err.a \
    $(STATIC_LIBSTDCPP) \
    $(STATIC_LIBGCC) \
    -lc -lm -lgcc_s

READLINETOOL_LIBS = \
    /usr/lib/libreadline.a \
    /usr/lib/libtermcap.a

AM_LDFLAGS = -nodefaultlibs

else

TOOL_LIBS = $(COMMON_LIBS)
DBTOOL_LIBS = $(COMMON_LIBS) $(PGSQL_LIBS)
READLINETOOL_LIBS = $(READLINE_LIBS)

endif

cycmd_SOURCES = cycmd.cpp

cycmd_LDADD = $(top_builddir)/modules/DateTime.o \
              $(top_builddir)/common/operations.o \
              $(top_builddir)/common/globals.o \
              $(top_builddir)/common/log.o \
              $(top_builddir)/common/utility.o \
              $(top_builddir)/common/const.o \
              $(READLINETOOL_LIBS) \
              $(TOOL_LIBS)

cyconfig_SOURCES = cyconfig.cpp
cyconfig_LDADD = $(TOOL_LIBS)

cyconvertrules_SOURCES = cyconvertrules.cpp
cyconvertrules_LDADD = $(TOOL_LIBS)

cyloadrules_SOURCES = cyloadrules.cpp

cyloadrules_LDADD = $(top_builddir)/modules/DateTime.o \
                    $(top_builddir)/common/Database.o \
                    $(top_builddir)/common/globals.o \
                    $(top_builddir)/common/log.o \
                    $(DBTOOL_LIBS)

cypasswd_SOURCES = cypasswd.cpp

cypasswd_LDADD = $(top_builddir)/modules/DateTime.o \
                 $(top_builddir)/common/Database.o \
                 $(top_builddir)/common/accountbase.o \
                 $(top_builddir)/common/globals.o \
                 $(top_builddir)/common/log.o \
                 $(top_builddir)/common/system.o \
                 $(DBTOOL_LIBS)
