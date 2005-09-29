INCLUDES = -I$(top_srcdir)

bin_PROGRAMS = cycmd cyconfig cyaddrules cyconvertrules cyloadrules \
               cydumprules cypasswd
noinst_SCRIPTS = cyphesis.sh

EXTRA_DIST = cyphesis.sh copy_python.sh

if LINK_STATIC

TOOL_LIBS = \
    /usr/lib/libvarconf-1.0.a \
    /usr/lib/libAtlasObjects-0.6.a \
    /usr/lib/libAtlasNet-0.6.a \
    /usr/lib/libAtlasCodecs-0.6.a \
    /usr/lib/libAtlasFilters-0.6.a \
    /usr/lib/libAtlasMessage-0.6.a \
    /usr/lib/libAtlas-0.6.a \
    /usr/lib/libsigc-2.0.a \
    $(STATIC_LIBSTDCPP) \
    $(STATIC_LIBGCC) \
    -lc -lm -lgcc_s

DBTOOL_LIBS = \
    /usr/lib/libvarconf-1.0.a \
    /usr/lib/libAtlasObjects-0.6.a \
    /usr/lib/libAtlasNet-0.6.a \
    /usr/lib/libAtlasCodecs-0.6.a \
    /usr/lib/libAtlasFilters-0.6.a \
    /usr/lib/libAtlasMessage-0.6.a \
    /usr/lib/libAtlas-0.6.a \
    /usr/lib/libsigc-2.0.a \
    /usr/lib/libpq.a \
    /usr/lib/libssl.a \
    /usr/lib/libkrb5.a \
    /usr/lib/libgssapi_krb5.a \
    /usr/lib/libk5crypto.a \
    /usr/lib/libgcrypt.a \
    /usr/lib/libgpg-error.a \
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

NETWORKTOOL_LIBS = \
    /usr/lib/libskstream-0.3.a

AM_LDFLAGS = -nodefaultlibs

else

TOOL_LIBS = $(COMMON_LIBS)
DBTOOL_LIBS = $(COMMON_LIBS) $(PGSQL_LIBS)
READLINETOOL_LIBS = $(READLINE_LIBS)
NETWORKTOOL_LIBS = $(NETWORK_LIBS)

endif

cycmd_SOURCES = cycmd.cpp

cycmd_LDADD = $(top_builddir)/modules/DateTime.o \
              $(top_builddir)/common/operations.o \
              $(top_builddir)/common/globals.o \
              $(top_builddir)/common/prefix.o \
              $(top_builddir)/common/log.o \
              $(top_builddir)/common/const.o \
              $(READLINETOOL_LIBS) \
              $(NETWORKTOOL_LIBS) \
              $(TOOL_LIBS)

cyconfig_SOURCES = cyconfig.cpp
cyconfig_LDADD = $(TOOL_LIBS)

cyconvertrules_SOURCES = cyconvertrules.cpp
cyconvertrules_LDADD = $(TOOL_LIBS)

cyaddrules_SOURCES = cyaddrules.cpp AdminClient.cpp AdminClient.h

cyaddrules_LDADD = $(top_builddir)/modules/DateTime.o \
                   $(top_builddir)/common/operations.o \
                   $(top_builddir)/common/globals.o \
                   $(top_builddir)/common/prefix.o \
                   $(top_builddir)/common/log.o \
                   $(NETWORKTOOL_LIBS) \
                   $(TOOL_LIBS)

cyloadrules_SOURCES = cyloadrules.cpp

cyloadrules_LDADD = $(top_builddir)/modules/DateTime.o \
                    $(top_builddir)/common/Database.o \
                    $(top_builddir)/common/globals.o \
                    $(top_builddir)/common/prefix.o \
                    $(top_builddir)/common/log.o \
                    $(DBTOOL_LIBS)

cydumprules_SOURCES = cydumprules.cpp

cydumprules_LDADD = $(top_builddir)/modules/DateTime.o \
                    $(top_builddir)/common/Database.o \
                    $(top_builddir)/common/globals.o \
                    $(top_builddir)/common/prefix.o \
                    $(top_builddir)/common/log.o \
                    $(DBTOOL_LIBS)

cypasswd_SOURCES = cypasswd.cpp

cypasswd_LDADD = $(top_builddir)/modules/DateTime.o \
                 $(top_builddir)/common/Database.o \
                 $(top_builddir)/common/accountbase.o \
                 $(top_builddir)/common/globals.o \
                 $(top_builddir)/common/prefix.o \
                 $(top_builddir)/common/log.o \
                 $(top_builddir)/common/system.o \
                 $(MATH_LIBS) \
                 $(DBTOOL_LIBS)
