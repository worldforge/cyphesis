bin_PROGRAMS = cycmd cypasswd cydbload cyloadrules cywatchdog cyctrl cyconfig

bin_SCRIPTS = cyphesis-setup

INCLUDES = -DBINDIR=\"@bindir@\" -DVARDIR=\"@localstatedir@\"

cypasswd_SOURCES = cypasswd.cpp sstream.h iotraits.h

cypasswd_LDADD = ../common/Database.o ../common/accountbase.o ../common/globals.o ../common/log.o

cycmd_SOURCES = cycmd.cpp

cycmd_LDADD = ../common/Database.o ../common/accountbase.o ../common/operations.o ../common/globals.o ../common/log.o ../common/const.o $(READLINE_LIBS)

cydbload_SOURCES = cydbload.cpp

cydbload_LDADD = ../common/Database.o ../common/globals.o ../common/log.o

cyloadrules_SOURCES = cyloadrules.cpp

cyloadrules_LDADD = ../common/Database.o ../common/globals.o ../common/log.o

cywatchdog_SOURCES = cywatchdog.cpp

cyctrl_SOURCES = cyctrl.cpp

cyconfig_SOURCES = cyconfig.cpp

EXTRA_DIST = sstream.cc cyphesis-setup
