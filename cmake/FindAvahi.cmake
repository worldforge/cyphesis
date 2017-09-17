# - Try to find the avahi service discovery library
# Once done this will define
#
#  AVAHI_FOUND - System has Avahi
#  AVAHI_INCLUDE_DIR - The Avahi include directory
#  AVAHI_LIBRARIES - The libraries needed to use Avahi
#  AVAHI_DEFINITIONS - Definitions to be added to the compiler options

# Copyright (c) 2010 Matthieu Volat. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are
# those of the authors and should not be interpreted as representing
# official policies, either expressed or implied, of Matthieu Volat.

IF(AVAHI_LIBRARIES)
   SET(Avahi_FIND_QUIETLY TRUE)
ENDIF(AVAHI_LIBRARIES)

IF(NOT WIN32)
  FIND_PACKAGE(PkgConfig)
  pkg_check_modules(PC_AVAHI-CLIENT avahi-client)
  SET(AVAHI_DEFINITIONS ${PC_AVAHI-CLIENT_CFLAGS_OTHER})
ENDIF(NOT WIN32)

FIND_PATH(AVAHI_INCLUDE_DIR avahi-client/client.h
    HINTS ${PC_AVAHI-CLIENT_INCLUDEDIR} ${PC_AVAHI-CLIENT_INCLUDE_DIRS})

FIND_LIBRARY(AVAHI_LIBRARIES 
    NAMES avahi-client
    HINTS ${PC_AVAHI-CLIENT_LIBDIR} ${PC_AVAHI-CLIENT_LIBRARY_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Avahi DEFAULT_MSG
    AVAHI_LIBRARIES
    AVAHI_INCLUDE_DIR)
IF(AVAHI_FOUND)
  SET(AVAHI_FOUND 1)
ENDIF(AVAHI_FOUND)

MARK_AS_ADVANCED(AVAHI_INCLUDE_DIR AVAHI_LIBRARIES)

