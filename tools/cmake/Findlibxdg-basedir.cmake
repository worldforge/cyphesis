# Try to find libxdg-basedir library
# Once done, this will define
#
# LIBXDG-BASEDIR_FOUND        - system has libxdg-basedir
# LIBXDG-BASEDIR_INCLUDE_DIRS - libxdg-basedir include directories
# LIBXDG-BASEDIR_LIBRARIES    - libraries need to use libxdg-basedir
#
# and the following imported targets
#
# libxdg-basedir::libxdg-basedir

find_path(LIBXDG-BASEDIR_INCLUDE_DIR
        NAMES basedir.h
        HINTS ${LIBXDG-BASEDIR_ROOT})

find_library(LIBXDG-BASEDIR_LIBRARY
        NAMES xdg-basedir
        HINTS ${LIBXDG-BASEDIR_ROOT}
        PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libxdg-basedir
        REQUIRED_VARS LIBXDG-BASEDIR_LIBRARY LIBXDG-BASEDIR_INCLUDE_DIR)

mark_as_advanced(LIBXDG-BASEDIR_FOUND LIBXDG-BASEDIR_LIBRARY LIBXDG-BASEDIR_INCLUDE_DIR)

if (LIBXDG-BASEDIR_FOUND AND NOT TARGET libxdg-basedir::libxdg-basedir)
    add_library(libxdg-basedir::libxdg-basedir UNKNOWN IMPORTED)
    set_target_properties(libxdg-basedir::libxdg-basedir PROPERTIES
            IMPORTED_LOCATION "${LIBXDG-BASEDIR_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIBXDG-BASEDIR_INCLUDE_DIR}")
endif ()

set(LIBXDG-BASEDIR_INCLUDE_DIRS ${LIBXDG-BASEDIR_INCLUDE_DIR})
set(LIBXDG-BASEDIR_LIBRARIES ${LIBXDG-BASEDIR_LIBRARY})
