noinst_LIBRARIES = libcommon.a

INCLUDES = -DVARDIR=\"@localstatedir@\" -DSHAREDIR=\"@datadir@\"

libcommon_a_SOURCES = const.cpp const.h \
		log.cpp log.h \
		config.h debug.h random.h \
		types.h operations.h op_switch.h \
		globals.cpp globals.h \
		database.cpp database.h \
		persistance.cpp persistance.h \
		utility.cpp utility.h \
		inheritance.cpp inheritance.h \
		BaseEntity.cpp BaseEntity.h \
		Chop.h Cut.h Eat.h Fire.h Load.h \
		Nourish.h Save.h Setup.h Tick.h Generic.h \
		sstream.h iotraits.h

EXTRA_DIST = sstream.cc
