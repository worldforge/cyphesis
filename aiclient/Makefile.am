AM_CPPFLAGS = -I$(top_srcdir) -I${top_builddir}

bin_PROGRAMS = cyaiclient

if LINK_STATIC

CLIENT_LIBS = \
    /usr/lib/libmercator-0.2.a \
    /usr/lib/libskstream-0.3.a \
    /usr/lib/libvarconf-1.0.a \
    /usr/lib/libwfmath-0.3.a \
    /usr/lib/libAtlasObjects-0.6.a \
    /usr/lib/libAtlasNet-0.6.a \
    /usr/lib/libAtlasCodecs-0.6.a \
    /usr/lib/libAtlasFilters-0.6.a \
    /usr/lib/libAtlasMessage-0.6.a \
    /usr/lib/libAtlas-0.6.a \
    /usr/lib/libsigc-2.0.a \
    $(PYTHON_LIBS) \
    /usr/lib/libutil.a \
    $(STATIC_LIBSTDCPP) \
    $(STATIC_LIBGCC) \
    -ldl -lc -lm -lpthread -lgcc_s

cyaiclient_LDFLAGS = -nodefaultlibs $(PYTHON_LINKER_FLAGS)

else

CLIENT_LIBS = $(COMMON_LIBS) $(TERRAIN_LIBS) $(NETWORK_LIBS) $(PYTHON_LIBS) $(PYTHON_UTIL_LIBS)

cyaiclient_LDFLAGS = $(PYTHON_LINKER_FLAGS)

endif

cyaiclient_SOURCES = ClientConnection.cpp BaseClient.cpp \
						PossessionClient.cpp MindClient.cpp \
						aiclient.cpp

noinst_HEADERS = ClientConnection.h BaseClient.h \
						PossessionClient.h MindClient.h

cyaiclient_LDADD = \
                 $(top_builddir)/rulesets/libscriptpython.a \
                 $(top_builddir)/rulesets/librulesetmind.a \
                 $(top_builddir)/rulesets/librulesetentity.a \
                 $(top_builddir)/rulesets/librulesetbase.a \
                 $(top_builddir)/modules/libmodules.a \
                 $(top_builddir)/common/libcommon.a \
                 $(top_builddir)/physics/libphysics.a \
                 $(CLIENT_LIBS)
