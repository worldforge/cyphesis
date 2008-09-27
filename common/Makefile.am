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
		      system.cpp system.h \
		      serialno.cpp serialno.h \
		      TypeNode.cpp TypeNode.h \
		      inheritance.cpp custom.cpp inheritance.h \
		      Property.cpp Property_impl.h Property.h \
		      PropertyFactory.cpp PropertyFactory.h \
		      PropertyFactory_impl.h \
		      PropertyManager.cpp PropertyManager.h \
		      OperationRouter.cpp OperationRouter.h \
		      Identified.cpp Identified.h \
		      BaseWorld.cpp BaseWorld.h \
		      AtlasFileLoader.cpp AtlasFileLoader.h \
		      Monitors.cpp Monitors.h \
		      Actuate.h Add.h Affect.h Attack.h Burn.h Connect.h \
		      Drop.h Eat.h Monitor.h Nourish.h \
		      Pickup.h Setup.h Tick.h Unseen.h Update.h

libtools_a_SOURCES = accountbase.cpp accountbase.h \
		     FormattedXMLWriter.cpp FormattedXMLWriter.h
