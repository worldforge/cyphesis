bin_PROGRAMS = cyphesisd cyphesis-control

#if BUILD_STATIC

LDFLAGS = -Wl,-static

#endif

INCLUDES = -DSHAREDIR=\"@datadir@\"

LIBS=

cyphesisd_SOURCES = watchdog.c
cyphesis_control_SOURCES = control.c
