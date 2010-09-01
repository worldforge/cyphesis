noinst_LIBRARIES = libcommon.a libtools.a

INCLUDES = -I$(top_srcdir)  -I${top_builddir} $(BINRELOC_CFLAGS) \
           -DDATADIR=\"@datadir@\" \
           -DSYSCONFDIR=\"@sysconfdir@\" \
           -DLOCALSTATEDIR=\"@localstatedir@\"

libcommon_a_SOURCES = const.cpp const.h \
		      binreloc.c binreloc.h \
		      newid.cpp id.cpp id.h \
		      log.cpp log.h \
		      debug.cpp debug.h \
		      random.h nls.h \
		      compose.hpp \
		      types.h op_switch.h \
		      type_utils.cpp type_utils.h \
		      type_utils_impl.h \
		      operations.cpp \
		      custom.cpp custom.h \
		      client_socket.cpp sockets.h \
		      globals.cpp globals.h \
		      Database.cpp Database.h \
		      system.cpp system.h \
		      system_net.cpp system_uid.cpp \
		      system_prefix.cpp \
		      serialno.cpp serialno.h \
		      TypeNode.cpp TypeNode.h \
		      Inheritance.cpp Inheritance.h \
		      Property.cpp Property_impl.h Property.h \
		      PropertyFactory.cpp PropertyFactory.h \
		      PropertyFactory_impl.h \
		      PropertyManager.cpp PropertyManager.h \
		      OperationRouter.cpp OperationRouter.h \
		      Router.cpp Router.h \
		      BaseWorld.cpp BaseWorld.h \
		      AtlasFileLoader.cpp AtlasFileLoader.h \
		      Monitors.cpp Monitors.h \
		      Variable.cpp Variable.h \
		      AtlasStreamClient.cpp AtlasStreamClient.h \
		      ClientTask.cpp ClientTask.h \
		      Actuate.h Add.h Affect.h Attack.h Burn.h Connect.h \
		      Drop.h Eat.h Monitor.h Nourish.h \
		      Pickup.h Setup.h Tick.h Unseen.h Update.h

libtools_a_SOURCES = AccountBase.cpp AccountBase.h \
		     FormattedXMLWriter.cpp FormattedXMLWriter.h
