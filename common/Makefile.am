noinst_LIBRARIES = libcommon.a libtools.a

INCLUDES = -I$(top_srcdir) -DSHAREDIR=\"@datadir@\" -DETCDIR=\"@sysconfdir@\" \
           -DVARDIR=\"@localstatedir@\"

libcommon_a_SOURCES = const.cpp const.h \
		      log.cpp log.h \
		      debug.h random.h nls.h \
		      types.h type_utils.h op_switch.h \
		      operations.cpp operations.h \
		      globals.cpp globals.h \
		      Database.cpp Database.h \
		      utility.cpp utility.h \
		      terrain_utils.cpp terrain_utils.h \
		      system.cpp system.h \
		      serialno.cpp serialno.h refno.h \
		      inheritance.cpp custom.cpp \
		      inheritance_impl.h inheritance.h \
		      Property.cpp Property_impl.h Property.h \
		      BaseEntity.cpp BaseEntity.h \
		      OOGThing.cpp OOGThing.h \
		      BaseWorld.cpp BaseWorld.h \
		      Burn.h Chop.h Cut.h Eat.h \
		      Nourish.h Setup.h Tick.h Generic.h \
		      Unseen.h Use.h Wield.h

libtools_a_SOURCES = accountbase.cpp accountbase.h \
		     FormattedXMLWriter.cpp FormattedXMLWriter.h
