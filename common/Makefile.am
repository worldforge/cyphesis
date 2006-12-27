noinst_LIBRARIES = libcommon.a libtools.a

if BIN_RELOC

INCLUDES = -I$(top_srcdir) -I${top_builddir}

else

INCLUDES = -I$(top_srcdir)  -I${top_builddir} \
           -DDATADIR=\"@datadir@\" \
           -DSYSCONFDIR=\"@sysconfdir@\" \
           -DLOCALSTATEDIR=\"@localstatedir@\"

endif

libcommon_a_SOURCES = const.cpp const.h \
		      prefix.c prefix.h \
		      newid.cpp id.cpp id.h \
		      log.cpp log.h \
		      debug.cpp debug.h \
		      random.h nls.h \
		      compose.hpp \
		      types.h type_utils.h op_switch.h \
		      operations.cpp \
		      globals.cpp globals.h \
		      Database.cpp Database.h \
		      terrain_utils.cpp terrain_utils.h \
		      system.cpp system.h \
		      serialno.cpp serialno.h \
		      inheritance.cpp custom.cpp inheritance.h \
		      Property.cpp Property_impl.h Property.h \
		      DynamicProperty.cpp DynamicProperty.h \
		      DynamicProperty_impl.h \
		      PropertyFactory.cpp PropertyFactory.h \
		      PropertyFactory_impl.h \
		      PropertyManager.cpp PropertyManager.h \
		      BaseEntity.cpp BaseEntity.h \
		      OOGThing.cpp OOGThing.h \
		      BaseWorld.cpp BaseWorld.h \
		      AtlasFileLoader.cpp AtlasFileLoader.h \
		      Add.h Affect.h Attack.h Burn.h Chop.h Connect.h Cut.h \
		      Delve.h Dig.h Drop.h Eat.h Monitor.h Mow.h Nourish.h \
		      Pickup.h Setup.h Tick.h Unseen.h Update.h

libtools_a_SOURCES = accountbase.cpp accountbase.h \
		     FormattedXMLWriter.cpp FormattedXMLWriter.h
