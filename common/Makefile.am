noinst_LIBRARIES = libcommon.a

# lib_LTLIBRARIES = libcyphesis.la

INCLUDES = -I$(top_srcdir) -DSHAREDIR=\"@datadir@\" -DETCDIR=\"@sysconfdir@\" \
           -DVARDIR=\"@localstatedir@\"

libcommon_a_SOURCES = const.cpp const.h \
		log.cpp log.h \
		debug.h random.h nls.h \
		types.h type_utils.h op_switch.h \
		operations.cpp operations.h \
		globals.cpp globals.h \
		Database.cpp Database.h \
		accountbase.cpp accountbase.h \
		utility.cpp utility.h \
		system.cpp system.h \
		serialno.cpp serialno.h \
		inheritance.cpp inheritance.h \
		custom.cpp \
		BaseEntity.cpp BaseEntity.h \
		OOGThing.cpp OOGThing.h \
		BaseWorld.cpp BaseWorld.h \
		Burn.h Chop.h Cut.h Eat.h \
		Nourish.h Setup.h Tick.h Generic.h \
		stringstream.h \
		sstream.h iotraits.h

# libcyphesis_la_SOURCES = Database.cpp Database.h \
			 # stringstream.h debug.h \
			 # globals.cpp globals.h \
			 # log.cpp log.h \
			 # accountbase.h

# libcyphesisincludedir = $(includedir)/cyphesis
# libcyphesisinclude_HEADERS = accountbase.h Database.h globals.h

EXTRA_DIST = sstream.cc
