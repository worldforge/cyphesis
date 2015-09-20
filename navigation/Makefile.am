SUBDIRS = external

noinst_LIBRARIES = libnavigation.a

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(srcdir)/external/RecastDetour/Detour/Include -I$(srcdir)/external/RecastDetour/DetourTileCache/Include -I$(srcdir)/external/RecastDetour/Recast/Include -DPREFIX=\"@prefix@\"

libnavigation_a_SOURCES = Awareness.cpp fastlz.c Steering.cpp 

noinst_HEADERS = Awareness.h fastlz.h Steering.h AwarenessUtils.h IHeightProvider.h