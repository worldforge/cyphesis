noinst_LIBRARIES = libcommon.a

lib_LTLIBRARIES = libcyphesis.la

INCLUDES = -DSHAREDIR=\"@datadir@\"

libcommon_a_SOURCES = const.cpp const.h \
		log.cpp log.h \
		config.h debug.h random.h \
		types.h op_switch.h \
		operations.cpp operations.h \
		globals.cpp globals.h \
		Database.cpp Database.h \
		accountbase.h \
		utility.cpp utility.h \
		serialno.cpp serialno.h \
		inheritance.cpp inheritance.h \
		custom.cpp \
		BaseEntity.cpp BaseEntity.h \
		OOGThing.cpp OOGThing.h \
		BaseWorld.cpp BaseWorld.h \
		Chop.h Cut.h Eat.h Fire.h Load.h \
		Nourish.h Save.h Setup.h Tick.h Generic.h \
		stringstream.h \
		sstream.h iotraits.h

libcyphesis_la_SOURCES = Database.cpp Database.h \
			 stringstream.h debug.h \
			 accountbase.h

libcyphesisincludedir = $(includedir)/cyphesis
libcyphesisinclude_HEADERS = accountbase.h Database.h

EXTRA_DIST = sstream.cc
