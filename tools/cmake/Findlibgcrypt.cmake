# Try to find libgcrypt library
# Once done, this will define
#
# LIBGCRYPT_FOUND        - system has libgcrypt
# LIBGCRYPT_INCLUDE_DIRS - libgcrypt include directories
# LIBGCRYPT_LIBRARIES    - libraries need to use libgcrypt
#
# and the following imported targets
#
# libgcrypt::libgcrypt

find_path(LIBGCRYPT_INCLUDE_DIR
        NAMES gcrypt.h
        HINTS ${LIBGCRYPT_ROOT})

find_library(LIBGCRYPT_LIBRARY
        NAMES gcrypt
        HINTS ${LIBGCRYPT_ROOT}
        PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libgcrypt
        REQUIRED_VARS LIBGCRYPT_LIBRARY LIBGCRYPT_INCLUDE_DIR)

mark_as_advanced(LIBGCRYPT_FOUND LIBGCRYPT_LIBRARY LIBGCRYPT_INCLUDE_DIR)

if (LIBGCRYPT_FOUND AND NOT TARGET libgcrypt::libgcrypt)
    add_library(libgcrypt::libgcrypt UNKNOWN IMPORTED)
    set_target_properties(libgcrypt::libgcrypt PROPERTIES
            IMPORTED_LOCATION "${LIBGCRYPT_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIBGCRYPT_INCLUDE_DIR}")
endif ()

set(LIBGCRYPT_INCLUDE_DIRS ${LIBGCRYPT_INCLUDE_DIR})
set(LIBGCRYPT_LIBRARIES ${LIBGCRYPT_LIBRARY})
